#include "view.h"
#include "models.h"

const int STARS_COUNT = 10000;

RECT view::rect = { 0, 0, 0, 0 };
float view::aspect;
glm::vec3 stars[STARS_COUNT];

glm::vec4 light0Pos(600.0, 50.0, 200.0, 1.0);
glm::vec4 light0Diffuse(1.0, 1.0, 1.0, 1.0);

glm::vec4 light1Pos(-600.0, -50.0, -200.0, 1.0);
glm::vec4 light1Diffuse(1.0, 1.0, 1.0, 1.0);

void view::InitScreen()
{
    view::rect.right = GetSystemMetrics(SM_CXSCREEN);
    view::rect.bottom = GetSystemMetrics(SM_CYSCREEN);
    view::aspect = (float)(view::rect.right - view::rect.left) / (view::rect.bottom - view::rect.top);
}

float randomFloat()
{
    int num = rand() % RAND_MAX;
    float fl = (float) num / RAND_MAX;
    float sig = fl * 2.0 - 1.0;
    return sig;
}

void view::InitStars()
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

glm::dmat4 viewMatrix(0.0);

void view::SetViewMatrix()
{
    viewMatrix = glm::lookAt(
        controller::cameraPos,
        controller::cameraPoint,
        controller::cameraUp
    );
}

void view::DrawStars()
{
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);

    glm::dmat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::dvec3(0.0, 0.0, 0.0));
    modelMatrix = glm::inverse(glm::lookAt(glm::dvec3(0.0f), glm::dvec3(0.0, 0.0, 1.0), glm::dvec3(0.0, 1.0, 0.0)) * modelMatrix);

    glm::dmat4 mvMatrix = viewMatrix * modelMatrix;
    glLoadMatrixd(glm::value_ptr(mvMatrix));

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

void view::DrawTie()
{
    glMatrixMode(GL_MODELVIEW);

    glm::dmat4 modelMatrix = glm::dmat4(1.0);
    modelMatrix = glm::translate(modelMatrix, controller::tiePos);
    modelMatrix = glm::inverse(glm::lookAt(glm::dvec3(0.0), controller::tieDir, controller::tieUp) * modelMatrix);

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

void view::DrawPlanet()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Установка матрицы модели
    glm::dmat4 modelMatrix = glm::dmat4(1.0);
    glm::dvec3 planetPos = glm::dvec3(1.0, -1.0, 2000.0);
    const float scale = 1000.0;

    modelMatrix = glm::translate(modelMatrix, planetPos);
    modelMatrix = glm::inverse(glm::lookAt(glm::dvec3(0.0f), glm::dvec3(0.0, 0.0, -1.0), glm::dvec3(0.0, 1.0, 0.0)) * modelMatrix);
    modelMatrix = glm::scale(modelMatrix, glm::dvec3(scale, scale, scale));
    modelMatrix = glm::rotate(modelMatrix, gametime::ticksF / 200.0, glm::dvec3(0.0, 1.0, 0.0));

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

void view::DrawDestroyer(glm::dvec3* destroyerPos)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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
