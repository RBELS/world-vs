#include <windows.h>
#include <iostream>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "headers/tie.h"
#include "headers/tie_n.h"
#include "headers/destroyer.h"
#include "headers/destroyer_n.h"
#include "headers/planet_v.h"
#include "headers/planet_n.h"
#include "gametime.h"
#include "data.h"
#include "control.h"
#include "screen.h"

// #define WIN_W 1200
// #define WIN_H 900

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Draw();
void ProcessKeyDown(WPARAM wParam);
void ProcessKeyUp(WPARAM wParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    screen::InitScreen();
    const CHAR CLASS_NAME[] = "Star Wars World";
    WNDCLASS drawWindowClass = { };

    drawWindowClass.lpfnWndProc = WindowProc;
    drawWindowClass.hInstance = hInstance;
    drawWindowClass.lpszClassName = CLASS_NAME;

    RegisterClass(&drawWindowClass);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Star Wars World",
        WS_VISIBLE | WS_POPUP,
        screen::rect.left, screen::rect.top, screen::rect.right - screen::rect.left, screen::rect.bottom - screen::rect.top,
        NULL, NULL, hInstance, NULL
    );
    if (hwnd == NULL) {
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


HDC hdc;
HGLRC hrc;
PIXELFORMATDESCRIPTOR pfd;
RECT WIN_RECT = {};
HANDLE hControlThread;

FILE* g_ic_file_cout_stream; FILE* g_ic_file_cin_stream;

glm::vec4 light0Pos(600.0, 50.0, 200.0, 1.0);
glm::vec4 light0Diffuse(1.0, 1.0, 1.0, 1.0);

glm::vec4 light1Pos(-600.0, -50.0, -200.0, 1.0);
glm::vec4 light1Diffuse(1.0, 1.0, 1.0, 1.0);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int choosePixelFormatResult;

    switch (uMsg) {
    case WM_CREATE:
        hdc = GetDC(hwnd);
        // SetCursor(NULL);
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_SUPPORT_OPENGL + PFD_DOUBLEBUFFER + PFD_DRAW_TO_WINDOW;
        pfd.iLayerType = PFD_MAIN_PLANE;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 16;
        pfd.cDepthBits = 16;
        pfd.cAccumBits = 0;
        pfd.cStencilBits = 0;

        choosePixelFormatResult = ChoosePixelFormat(hdc, &pfd);
        SetPixelFormat(hdc, choosePixelFormatResult, &pfd);
        hrc = wglCreateContext(hdc);
        wglMakeCurrent(hdc, hrc);
        GetClientRect(hwnd, &WIN_RECT);
        glViewport(0, 0, WIN_RECT.right, WIN_RECT.bottom);

        glEnable(GL_DEPTH_TEST);
        glShadeModel(GL_SMOOTH);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        InvalidateRect(hwnd, &WIN_RECT, true);
        srand(time(NULL));
        gametime::InitTicks();
        screen::InitStars();

        // Установка света
        glEnable(GL_NORMALIZE);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);

        /*glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(light0Diffuse));
        glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(light0Pos));*/


        AllocConsole();
        freopen_s(&g_ic_file_cout_stream, "CONOUT$", "w", stdout);
        freopen_s(&g_ic_file_cin_stream, "CONIN$", "w+", stdin);

        // create read input thread
        std::cout << "Starting control thread" << std::endl;
        hControlThread = CreateThread(NULL, NULL, controlThreadProc, NULL, 0, NULL);

        break;
    case WM_SIZE:
        GetClientRect(hwnd, &WIN_RECT);
        glViewport(0, 0, WIN_RECT.right, WIN_RECT.bottom);
        break;
    case WM_PAINT:
        gametime::UpdateTicks();
        moveTie();
        Draw();
        break;
    case WM_MOUSEMOVE:
        break;
    case WM_KEYUP:
        ProcessKeyUp(wParam);
        break;
    case WM_KEYDOWN:
        ProcessKeyDown(wParam);
        if (wParam != VK_ESCAPE) {
            break;
        }
    case WM_DESTROY:
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hrc);
        ReleaseDC(hwnd, hdc);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}


void DrawTie()
{
    glMatrixMode(GL_MODELVIEW);

    glm::dmat4 viewMatrix = glm::lookAt(
        cameraPos,
        cameraPoint,
        cameraUp
    );

    glm::dmat4 modelMatrix = glm::dmat4(1.0);
    modelMatrix = glm::translate(modelMatrix, tiePos);
    modelMatrix = glm::inverse(glm::lookAt(glm::dvec3(0.0), tieDir, tieUp) * modelMatrix);

    glm::dmat4 mvMatrix = viewMatrix * modelMatrix;
    glLoadMatrixd(glm::value_ptr(mvMatrix));

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glColor3f(1.0, 1.0, 1.0);
    glVertexPointer(3, GL_FLOAT, 3 * 4, __files_raw_tie_v);
    glNormalPointer(GL_FLOAT, 3 * 4, __files_raw_tie_n);

    glDrawArrays(GL_TRIANGLES, 0, __files_raw_tie_v_len / 4 / 3);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void DrawPlanet()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Установка матрицы вида
    glm::dmat4 viewMatrix = glm::lookAt(
        cameraPos,
        cameraPoint,
        cameraUp
    );

    // Установка матрицы модели
    glm::dmat4 modelMatrix = glm::dmat4(1.0);
    glm::dvec3 planetPos = glm::dvec3(1.0, -1.0, 2000.0);
    const float scale = 1000.0;

    modelMatrix = glm::translate(modelMatrix, planetPos);
    modelMatrix = glm::inverse(glm::lookAt(glm::dvec3(0.0f), glm::dvec3(0.0, 0.0, -1.0), glm::dvec3(0.0, 1.0, 0.0)) * modelMatrix);
    modelMatrix = glm::scale(modelMatrix, glm::dvec3(scale, scale, scale));
    modelMatrix = glm::rotate(modelMatrix, gametime::ticksF/200.0, glm::dvec3(0.0, 1.0, 0.0));

    // Сначала умножаем матрицу вида, затем матрицу модели
    glm::dmat4 mvMatrix = viewMatrix * modelMatrix;

    glLoadMatrixd(glm::value_ptr(mvMatrix));

    glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(light0Diffuse));
    glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(light0Pos));

    glLightfv(GL_LIGHT1, GL_DIFFUSE, glm::value_ptr(light1Diffuse));
    glLightfv(GL_LIGHT1, GL_POSITION, glm::value_ptr(light1Pos));

    // Рендеринг объекта
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glColor3f(1.0, 1.0, 1.0);
    glVertexPointer(3, GL_FLOAT, 3 * 4, planet_v);
    glNormalPointer(GL_FLOAT, 3 * 4, planet_n);

    glDrawArrays(GL_TRIANGLES, 0, planet_v_len / 4 / 3);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void DrawDestroyer(glm::dvec3 *destroyerPos)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Установка матрицы вида
    glm::dmat4 viewMatrix = glm::lookAt(
        cameraPos,
        cameraPoint,
        cameraUp
    );

    // Установка матрицы модели
    glm::dmat4 modelMatrix = glm::dmat4(1.0);
    //glm::vec3 destroyerPos = glm::vec3(100.0, -50.0, 200.0);
    const double scale = 10.0;

    modelMatrix = glm::translate(modelMatrix, *destroyerPos);
    modelMatrix = glm::inverse(glm::lookAt(glm::dvec3(0.0), glm::dvec3(0.0, 0.0, -1.0), glm::dvec3(0.0, 1.0, 0.0)) * modelMatrix);
    modelMatrix = glm::scale(modelMatrix, glm::dvec3(scale, scale, scale));

    // Сначала умножаем матрицу вида, затем матрицу модели
    glm::dmat4 mvMatrix = viewMatrix * modelMatrix;

    glLoadMatrixd(glm::value_ptr(mvMatrix));

    // Рендеринг объекта
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glColor3f(1.0, 1.0, 1.0);
    glVertexPointer(3, GL_FLOAT, 3 * 4, __destroyer_v);
    glNormalPointer(GL_FLOAT, 3 * 4, __destroyer_n);

    glDrawArrays(GL_TRIANGLES, 0, __destroyer_v_len / 4 / 3);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

glm::dvec3 destroyer1Pos = glm::dvec3(100.0, -50.0, 200.0);
glm::dvec3 destroyer2Pos = glm::dvec3(-160.0, 50.0, 280.0);
glm::dvec3 destroyer3Pos = glm::dvec3(0.0, 100.0, 150.0);

void Draw()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, screen::aspect, 0.01, 3200.0);

    screen::DrawStars();

    DrawDestroyer(&destroyer1Pos);
    DrawDestroyer(&destroyer2Pos);
    DrawDestroyer(&destroyer3Pos);

    DrawPlanet();

    DrawTie();

    SwapBuffers(hdc);
}


void ProcessKeyDown(WPARAM wParam)
{
    switch (wParam)
    {
    case VK_UP:
        activeKeys.straight = true;
        break;
    case VK_DOWN:
        activeKeys.back = true;
        break;
    case VK_LEFT:
        activeKeys.left = true;
        break;
    case VK_RIGHT:
        activeKeys.right = true;
        break;
    }
}

void ProcessKeyUp(WPARAM wParam)
{
    switch (wParam)
    {
    case VK_UP:
        activeKeys.straight = false;
        break;
    case VK_DOWN:
        activeKeys.back = false;
        break;
    case VK_LEFT:
        activeKeys.left = false;
        break;
    case VK_RIGHT:
        activeKeys.right = false;
        break;
    }
}