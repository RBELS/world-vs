#pragma once

#include <windows.h>

namespace gametime
{
    extern UINT64 startTicks, prevTicks, ticks, deltaTicks;
    extern double startTicksF, prevTicksF, ticksF, deltaTicksF;

    void InitTicks();
    void UpdateTicks();
}