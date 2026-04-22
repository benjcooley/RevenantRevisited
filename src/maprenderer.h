// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  maprenderer.h - Map renderer object                  *
// *************************************************************************

#pragma once

#include <cstdint>
#include <memory>

#include "debugui.h"

class TMapRenderer
    : public IDebugTabContributor
{
  public:
    TMapRenderer();
    ~TMapRenderer();

    bool InitializeFromStartupArgs();
    void Shutdown();
    void RenderFrame();
    void HandleMouseClick(int32_t button, int32_t x, int32_t y);
    void HandleMouseMove(int32_t button, int32_t x, int32_t y);
    const char* GetDebugTabName() const override;
    void DrawDebugTab() override;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
