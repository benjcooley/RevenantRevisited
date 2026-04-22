// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 bitmapatlas.h - Bitmap atlas helpers                  *
// *************************************************************************

#pragma once

#include "revenant.h"

#include <sokol_gfx.h>

#include <string>
#include <vector>

struct SBitmapAtlasFrame
{
    int32_t ax = 0;
    int32_t ay = 0;
    int32_t w = 0;
    int32_t h = 0;
    int16_t dx = 0;
    int16_t dy = 0;
    int16_t regx = 0;
    int16_t regy = 0;
};

struct SBitmapAtlasItem
{
    int32_t firstFrame = 0;
    int32_t numFrames = 0;
    int32_t maxW = 0;
    int32_t maxH = 0;
    std::string label;
};

struct SBitmapAtlas
{
    sg_image image = {};
    int32_t width = 0;
    int32_t height = 0;
    std::vector<SBitmapAtlasItem> items;
    std::vector<SBitmapAtlasFrame> frames;
};

bool BuildImageryBitmapAtlas(SBitmapAtlas* atlas);
  // Scans the imagery registry for 2D bitmap-backed entries, packs their
  // inventory/world frames into one RGBA8 atlas image, and fills the
  // per-item/per-frame metadata used by preview tools.

void DestroyBitmapAtlas(SBitmapAtlas* atlas);
  // Releases the atlas image and clears all packed metadata.
