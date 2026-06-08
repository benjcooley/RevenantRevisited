// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 bitmapatlas.h - Bitmap atlas helpers                  *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "render3d_types.h"

#include <string>
#include <vector>

_CLASSDEF(TMulti)

struct SBitmapAtlasSlice
{
    TTextureHandle texture = kInvalidTexture;
    int32_t src_x = 0;
    int32_t src_y = 0;
    int32_t width = 0;
    int32_t height = 0;
    int32_t tex_width = 0;
    int32_t tex_height = 0;
    int32_t page_index = -1;
};

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
    TTextureHandle texture = kInvalidTexture;
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

uint64_t UIBitmapAtlasKey(PTBitmap bm, bool prefer_alias = false);
  // Stable key for one decoded bitmap form. Includes bitmap identity,
  // structure, backing buffers, and alias/data decode mode.

void BeginUIBitmapAtlasBuild();
  // Starts a fresh shared UI bitmap atlas collection pass. Call before
  // initializing the HUD panes that will register their loaded art.

void RegisterUIBitmapAtlasBitmap(PTBitmap bm, bool prefer_alias = false);
void RegisterUIBitmapAtlasArchive(TMulti* multi);
int32_t RegisterUIBitmapAtlasInventoryImagery();
  // Registers every inventory icon/animated-icon frame exposed by the
  // object imagery registry. This is intentionally a whole-world pass so
  // inventory, bottom bar, equipment, and drag visuals share one atlas.

bool BuildUIBitmapAtlas();
  // Packs registered HUD/static art, item icons, animated icon frames,
  // spell icons, rings, buttons, arrows, cursor pieces, etc. into shared
  // atlas pages. Small sprites are grouped before larger pane art.

void ClearUIBitmapAtlas();
bool LookupUIBitmapAtlasSlice(PTBitmap bm, bool prefer_alias, SBitmapAtlasSlice* out);
