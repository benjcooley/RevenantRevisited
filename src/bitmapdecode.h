// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 bitmapdecode.h - Bitmap decode helpers                *
// *************************************************************************

#pragma once

#include "revenant.h"

// Decode a bitmap into an RGBA8 region of `dst`.
//
// prefer_alias: when true AND the bitmap carries a BM_ALIAS RLE coverage
// buffer, decode from THAT buffer (anti-aliased sprite: cursor/wedge
// ground shadow, soft UI text shadow, glow) instead of the data array.
// Default false = decode the normal data8/data16 array. The distinction
// is per-DRAW, not per-bitmap: many sprites (the cursor itself) carry
// both a real data array AND a supplementary alias buffer, and must
// decode from the data array; only shadow/glow draws want the alias
// buffer (retail keyed this off the DM_ALIAS draw mode, not the
// BM_ALIAS bitmap flag).
bool DecodeBitmapToRGBA(PTBitmap bm, uint8_t* dst, int32_t dst_pitch,
                        int32_t ox, int32_t oy, bool prefer_alias = false);
