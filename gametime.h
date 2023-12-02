#pragma once

#include <windows.h>

namespace gametime
{
    extern int startTicks, prevTicks, ticks, deltaTicks;
    extern float startTicksF, prevTicksF, ticksF, deltaTicksF;

    void InitTicks();
    void UpdateTicks();
}