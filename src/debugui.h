// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    debugui.h - Debug UI overlay                       *
// *************************************************************************

#pragma once

#include <vector>

// **********************************
// * IDebugTabContributor Interface *
// **********************************

class IDebugTabContributor
{
  public:
    virtual ~IDebugTabContributor() = default;
    virtual const char* GetDebugTabName() const = 0;
    virtual void DrawDebugTab() = 0;
};

// ******************
// * DebugUI Module *
// ******************

namespace DebugUI {
void RegisterContributor(IDebugTabContributor* contributor);
void UnregisterContributor(IDebugTabContributor* contributor);
void DrawFrame();
}  // namespace DebugUI
