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

    // Read the camera's current level / sector / world position. -1 in
    // any field means "not available yet" (renderer not initialised).
    void GetCameraStatus(int32_t& level, int32_t& sector_x, int32_t& sector_y,
                         int32_t& world_x, int32_t& world_y, int32_t& world_z) const;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
