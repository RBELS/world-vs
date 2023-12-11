#include <windows.h>
#include <iostream>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gametime.h"
#include "controller.h"
#include "view.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Draw();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    view::InitScreen();
    const CHAR CLASS_NAME[] = "Star Wars World";
    WNDCLASS drawWindowClass = { };

    drawWindowClass.lpfnWndProc = WindowProc;
    drawWindowClass.hInstance = hInstance;
    drawWindowClass.lpszClassName = CLASS_NAME;

    RegisterClass(&drawWindowClass);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Star Wars World",
        WS_VISIBLE | WS_POPUP,
        view::rect.left, view::rect.top, view::rect.right - view::rect.left, view::rect.bottom - view::rect.top,
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


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int choosePixelFormatResult;

    switch (uMsg) {
    case WM_CREATE:
        hdc = GetDC(hwnd);
        SetCursor(NULL);
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
        view::InitStars();

        // Установка света
        glEnable(GL_NORMALIZE);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(90.0, view::aspect, 0.01, 3200.0);

        hControlThread = CreateThread(NULL, NULL, controller::controlThreadProc, NULL, 0, NULL);

        break;
    case WM_SIZE:
        view::InitScreen();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(90.0, view::aspect, 0.01, 3200.0);
        GetClientRect(hwnd, &WIN_RECT);
        glViewport(0, 0, WIN_RECT.right, WIN_RECT.bottom);
        break;
    case WM_PAINT:
        gametime::UpdateTicks();
        controller::Move();
        Draw();
        break;
    case WM_MOUSEMOVE:
        break;
    case WM_KEYDOWN:
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




glm::dvec3 destroyer1Pos = glm::dvec3(100.0, -50.0, 200.0);
glm::dvec3 destroyer2Pos = glm::dvec3(-160.0, 50.0, 280.0);
glm::dvec3 destroyer3Pos = glm::dvec3(0.0, 100.0, 150.0);

void Draw()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view::SetViewMatrix();

    view::DrawStars();

    view::DrawDestroyer(&destroyer1Pos);
    view::DrawDestroyer(&destroyer2Pos);
    view::DrawDestroyer(&destroyer3Pos);

    view::DrawPlanet();

    view::DrawTie();

    SwapBuffers(hdc);
}
