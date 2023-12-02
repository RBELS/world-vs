#include "gametime.h"

int gametime::startTicks, gametime::prevTicks, gametime::ticks, gametime::deltaTicks;
float gametime::startTicksF, gametime::prevTicksF, gametime::ticksF, gametime::deltaTicksF;

void gametime::InitTicks()
{
    int temp = GetTickCount();
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
    int temp = GetTickCount();
    ticks = temp - startTicks;
    ticksF = ticks / 100.0;

    deltaTicks = ticks - prevTicks;

    prevTicks = ticks;
    deltaTicksF = ticksF - prevTicksF;

    prevTicksF = ticksF;
}