#include "gametime.h"

UINT64 gametime::startTicks, gametime::prevTicks, gametime::ticks, gametime::deltaTicks;
double gametime::startTicksF, gametime::prevTicksF, gametime::ticksF, gametime::deltaTicksF;

void gametime::InitTicks()
{
    LARGE_INTEGER perfCounter;
    QueryPerformanceCounter(&perfCounter);

    UINT64 temp = perfCounter.QuadPart;
    startTicks = temp;
    startTicksF = temp / 100.0;

    prevTicks = 0;
    prevTicksF = 0.0;

    ticks = 0;
    ticksF = 0.0;

    deltaTicks = 0;
    deltaTicksF = 0.0;
}

void gametime::UpdateTicks()
{
    LARGE_INTEGER perfCounter;
    QueryPerformanceCounter(&perfCounter);

    UINT64 temp = perfCounter.QuadPart;
    ticks = temp - startTicks;
    ticksF = ticks / 1000000.0;

    deltaTicks = ticks - prevTicks;

    prevTicks = ticks;
    deltaTicksF = ticksF - prevTicksF;

    prevTicksF = ticksF;
}