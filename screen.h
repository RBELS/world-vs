#pragma once

#include <windows.h>
#include "glm/glm.hpp"
#include <gl/GL.h>
#include <gl/GLU.h>

namespace screen
{
    extern RECT rect;
    extern float aspect;

    void InitScreen();
    void DrawStars();
    void InitStars();
}