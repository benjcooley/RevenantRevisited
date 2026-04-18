// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   timer.cpp - Timer oject module                      *
// *************************************************************************

#include "revenant.h"
#include "screen.h"
#include "timer.h"

uint32_t TTimer::clockticks;

TTimer::TTimer() { clockticks = 0; }
TTimer::~TTimer() {}

bool TTimer::Initialize() { return true; }
void TTimer::Close() {}

void TTimer::WaitForTick()
{
    TickOccured = true;
}

void TTimer::ResetTick()
{
    TickOccured = false;
}
