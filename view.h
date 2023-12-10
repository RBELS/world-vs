#pragma once

#include <windows.h>
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "controller.h"
#include "gametime.h"

namespace view
{
    extern RECT rect;
    extern float aspect;

    void InitScreen();
    void DrawStars();
    void InitStars();
    void DrawTie();
    void DrawPlanet();
    void DrawDestroyer(glm::dvec3* destroyerPos);
    void SetViewMatrix();
}