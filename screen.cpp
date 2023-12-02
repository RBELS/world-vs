#include "screen.h"

const int STARS_COUNT = 10000;

RECT screen::rect = { 0, 0, 0, 0 };
float screen::aspect;
glm::vec3 stars[STARS_COUNT];

void screen::InitScreen()
{
    screen::rect.right = GetSystemMetrics(SM_CXSCREEN);
    screen::rect.bottom = GetSystemMetrics(SM_CYSCREEN);
    screen::aspect = (float)(screen::rect.right - screen::rect.left) / (screen::rect.bottom - screen::rect.top);
}

float randomFloat()
{
    int num = rand() % RAND_MAX;
    float fl = (float) num / RAND_MAX;
    float sig = fl * 2.0 - 1.0;
    return sig;
}

void screen::InitStars()
{
    for (int i = 0; i < STARS_COUNT; i++)
    {
        glm::vec3 buf(0.0, 0.0, 0.0);
        buf.x = randomFloat();
        buf.y = randomFloat();
        buf.z = randomFloat();
        stars[i] = glm::normalize(buf);
        stars[i] *= 1500.0;
    }
}

void screen::DrawStars()
{
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);

    glm::mat4 viewMatrix = glm::lookAt(
        cameraPos,
        cameraPoint,
        cameraUp
    );

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0.0, 0.0));
    modelMatrix = glm::inverse(glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0)) * modelMatrix);

    glm::mat4 mvMatrix = viewMatrix * modelMatrix;
    glLoadMatrixf(glm::value_ptr(mvMatrix));

    glPointSize(1.0);
    glEnable(GL_POINT_SMOOTH);
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, stars);

    glColor3f(1.0, 1.0, 1.0);
    glDrawArrays(GL_POINTS, 0, STARS_COUNT / 2);

    glColor3f(0.4, 0.4, 0.6);
    glDrawArrays(GL_POINTS, STARS_COUNT / 4, STARS_COUNT / 2);

    glColor3f(0.4, 0.4, 0.6);
    glDrawArrays(GL_POINTS, STARS_COUNT / 2, STARS_COUNT / 4);

    glColor3f(0.6, 0.4, 0.4);
    glDrawArrays(GL_POINTS, STARS_COUNT * 3 / 4, STARS_COUNT / 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glEnable(GL_LIGHTING);
}