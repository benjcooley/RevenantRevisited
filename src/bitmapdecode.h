// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 bitmapdecode.h - Bitmap decode helpers                *
// *************************************************************************

#pragma once

#include "revenant.h"

// How a bitmap becomes RGBA: which of its buffers it is read from, and which
// of its pixels are transparent. Retail decided both per draw, not per
// bitmap -- the DM_ALIAS draw mode, the key of the surface a bitmap was put
// through -- so one bitmap can decode more than one way; each way is its
// own texture.
enum class EBitmapDecode : uint8_t
{
    // The data array. 15/16-bit pixels equal to the bitmap's key or to
    // magenta (0x7c1f / 0xf81f, retail's surface key) are transparent: a
    // DM_TRANSPARENT draw.
    Pixels,
    // The data array with nothing keyed: a draw without DM_TRANSPARENT, which
    // puts the key colour like any other (the bottom bar's plates and boxes;
    // its rings, which blend through their BM_ALPHA coverage with DM_ALPHA).
    Unkeyed,
    // The data array with only magenta transparent: a DM_TRANSPARENT draw of a
    // bitmap retail keys on the display's own key first, so its stored key
    // (often 0, black) stays opaque (the quick-spell circles, 0x00542ab3).
    MagentaKeyed,
    // The BM_ALIAS RLE coverage buffer, for the draws that want the
    // anti-aliased form (the cursor's ground shadow, soft text shadows,
    // glows); the data array when the bitmap has none. Sprites such as the
    // cursor carry both and draw from the data array otherwise.
    Alias,
    // A 15/16-bit bitmap as one of retail's ARGB4444 overlay textures holds
    // it after it went in through a 16-bit surface (the HUD's portraits):
    // only magenta is transparent -- the surface's key, so the bitmap's own
    // key (often 0, black) stays opaque -- and each colour channel is
    // truncated to 4 bits (R5 >> 1, G6 >> 2, B5 >> 1; 555 green widens by a
    // shift first), shown as ARGB4444 decodes (c4 << 4).
    Overlay4444,
};

// Decode a bitmap into an RGBA8 region of `dst`.
bool DecodeBitmapToRGBA(PTBitmap bm, uint8_t* dst, int32_t dst_pitch,
                        int32_t ox, int32_t oy, EBitmapDecode decode = EBitmapDecode::Pixels);

// Decode a 15/16-bit bitmap the way retail's hue-change blit draws it
// (DM_CHANGEHUE, FUN_004b21d0 -- the 1998 PutHueChange in graphics.cpp):
// every green-dominant pixel takes `hue` (degrees, 0..359; 360 and up leave
// it alone) at its own saturation and value, other pixels keep their colour,
// and pixel 0 or the bitmap's key is transparent. Writes w*h RGBA8 at `dst`.
bool DecodeBitmapHueChangedToRGBA(const TBitmap* bm, int32_t hue,
                                  uint8_t* dst, int32_t dst_pitch);
