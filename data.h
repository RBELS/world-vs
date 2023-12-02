#pragma once

#include "glm/glm.hpp"

typedef struct keys {
    bool straight, back, left, right;
} keys;

struct gamepad_state
{
    volatile double rtPow;
    volatile double ltPow;
    volatile double lStickX, lStickY;
    volatile double rStickX, rStickY;
    volatile double lb, rb;
};