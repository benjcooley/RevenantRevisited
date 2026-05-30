// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               bitmapdecode.cpp - Bitmap decode helpers                *
// *************************************************************************

#include "bitmapdecode.h"

#include "bitmap.h"

namespace {

inline void Decode555(uint16_t px, uint8_t* rgba_out)
{
    rgba_out[0] = (uint8_t)(((px >> 10) & 0x1F) << 3);
    rgba_out[1] = (uint8_t)(((px >> 5)  & 0x1F) << 3);
    rgba_out[2] = (uint8_t)(( px        & 0x1F) << 3);
    rgba_out[3] = 255;
}

// BM_16BIT bitmaps in the retail data (e.g. SpellIcons.dat RingU/D/G, flags=0x104)
// are genuine 565 (R5 G6 B5). Decoding them as 555 leaks the high green bit into
// red and saturates red one bit short -> visible green tint. Bit-replicate so
// the max 5/6-bit values read as 0xff (not 0xf8 / 0xfc).
inline void Decode565(uint16_t px, uint8_t* rgba_out)
{
    const uint8_t r5 = (uint8_t)((px >> 11) & 0x1F);
    const uint8_t g6 = (uint8_t)((px >> 5)  & 0x3F);
    const uint8_t b5 = (uint8_t)( px        & 0x1F);
    rgba_out[0] = (uint8_t)((r5 << 3) | (r5 >> 2));
    rgba_out[1] = (uint8_t)((g6 << 2) | (g6 >> 4));
    rgba_out[2] = (uint8_t)((b5 << 3) | (b5 >> 2));
    rgba_out[3] = 255;
}

}  // namespace

bool DecodeBitmapToRGBA(PTBitmap bm, uint8_t* dst, int32_t dst_pitch,
                        int32_t ox, int32_t oy)
{
    if (!bm || bm->width <= 0 || bm->height <= 0) return false;
    if (bm->flags & BM_COMPRESSED) return false;  // TODO decompressor
    const int32_t w = bm->width, h = bm->height;

    if (bm->flags & BM_8BIT)
    {
        SPalette* pal = (SPalette*)bm->palette.ptr();
        if (!pal) return false;
        const uint8_t key = (uint8_t)bm->keycolor;
        const uint8_t* src = bm->data8;
        for (int32_t y = 0; y < h; y++)
        {
            uint8_t* row = dst + (oy + y) * dst_pitch + ox * 4;
            for (int32_t x = 0; x < w; x++)
            {
                const uint8_t idx = src[y * w + x];
                if (idx == key) { row[0]=row[1]=row[2]=row[3]=0; }
                else
                {
                    // rgbcolors is Windows COLORREF: 0x00BBGGRR.
                    const uint32_t c = pal->rgbcolors[idx];
                    row[0] = (uint8_t)( c        & 0xFF);
                    row[1] = (uint8_t)((c >> 8)  & 0xFF);
                    row[2] = (uint8_t)((c >> 16) & 0xFF);
                    row[3] = 255;
                }
                row += 4;
            }
        }
        return true;
    }
    // BM_UNKNOWN_0x10000 — a 16-bit-per-pixel ARGB4444 variant flag that
    // appears on automap.dat (Marker, Amap, PlusSel, MinusSel) and statusbar.dat.
    // No entry in revdefs.h's BM_* enum. On-disk pixel data is 2 bytes/pixel:
    // header.datasize = w*h*2 exactly (verified on every entry in automap.dat),
    // no palette, no separate alpha buffer. The pixel format is ARGB4444 stored
    // little-endian — bits 15..12 = alpha, 11..8 = red, 7..4 = green, 3..0 = blue
    // (each 4-bit channel bit-replicated 8 = (c4<<4)|c4 for clean white).
    //
    // Determined by inspecting Amap's pixel histogram + spatial layout: the dark
    // interior (body-window area) reads as a perfect ARGB4444 alpha gradient
    // (0xb000 → 0x8000 → ... → 0x0000 = decreasing alpha, RGB=0 transparent
    // black), and the chrome edge reads as the expected warm-brown stone +
    // gold trim ornate carvings (e.g. 0xfb73 = A=15 R=11 G=7 B=3 = opaque
    // warm orange-brown; 0xfda5 = A=15 R=13 G=10 B=5 = opaque cream/gold).
    // Decoding the same data as 555/565 gives all-red garbage (the 0x?000
    // alpha-gradient pixels parse as solid red, the chrome reads as pink).
    //
    // No magenta chroma-key is used here (alpha is encoded directly per-pixel).
    if (bm->flags & 0x10000)
    {
        const uint16_t* src = bm->data16;
        for (int32_t y = 0; y < h; y++)
        {
            uint8_t* row = dst + (oy + y) * dst_pitch + ox * 4;
            for (int32_t x = 0; x < w; x++)
            {
                const uint16_t px = src[y * w + x];
                const uint8_t a4 = (uint8_t)((px >> 12) & 0x0F);
                const uint8_t r4 = (uint8_t)((px >>  8) & 0x0F);
                const uint8_t g4 = (uint8_t)((px >>  4) & 0x0F);
                const uint8_t b4 = (uint8_t)( px        & 0x0F);
                // Bit-replicate 4-bit channels to 8-bit (0xF -> 0xFF, not 0xF0).
                row[0] = (uint8_t)((r4 << 4) | r4);
                row[1] = (uint8_t)((g4 << 4) | g4);
                row[2] = (uint8_t)((b4 << 4) | b4);
                row[3] = (uint8_t)((a4 << 4) | a4);
                row += 4;
            }
        }
        return true;
    }
    if (bm->flags & (BM_15BIT | BM_16BIT))
    {
        const uint16_t key = (uint16_t)bm->keycolor;
        // Magenta (R=31,G=0,B=31 in 555 = 0x7c1f) is the implicit
        // transparency key in many Revenant retail sprites — the
        // bitmap's keycolor field is often 0 even when magenta is the
        // intended transparent background. The runtime engine appears
        // to handle this via DM_TRANSPARENT drawmode + per-blit setup
        // we don't have here. Treat magenta as transparent globally.
        constexpr uint16_t MAGENTA_KEY = 0x7c1f;
        const uint16_t* src = bm->data16;
        // Per-pixel alpha buffer (BM_ALPHA): 1 byte/pixel, 0 = transparent,
        // 0xff = opaque. Used by glass/translucent sprites like the portrait
        // Ring (semi-transparent glass disc + opaque brass border). Without it
        // the glass renders fully opaque and hides what's behind (the portrait).
        // BM_ALPHA carries one 5-bit alpha value per pixel (1 byte each, range
        // 0..31; matches the 15-bit colour depth). Expand 5->8 bit so the brass
        // ring reads opaque and the glass disc its true coverage. Without this
        // the ring rendered at ~12% alpha (effectively invisible).
        const uint8_t* alpha5 = (bm->flags & BM_ALPHA)
                                    ? (const uint8_t*)bm->alpha.ptr() : nullptr;
        for (int32_t y = 0; y < h; y++)
        {
            uint8_t* row = dst + (oy + y) * dst_pitch + ox * 4;
            for (int32_t x = 0; x < w; x++)
            {
                const uint16_t px = src[y * w + x];
                uint8_t a = 0xff;
                if (alpha5)
                {
                    const uint8_t a5 = alpha5[y * w + x] & 0x1f;
                    a = (uint8_t)((a5 << 3) | (a5 >> 2));   // 5-bit -> 8-bit
                }
                if (a == 0 || px == key || px == MAGENTA_KEY)
                {
                    row[0]=row[1]=row[2]=row[3]=0;
                }
                else
                {
                    // Branch ONLY on BM_16BIT — BM_15BIT (e.g. spell icons,
                    // chrome, portraits, every existing 555 asset) keeps the
                    // exact pre-existing Decode555 path so nothing else moves.
                    if (bm->flags & BM_16BIT) Decode565(px, row);
                    else                      Decode555(px, row);
                    row[3] = a;
                }
                row += 4;
            }
        }
        return true;
    }
    return false;
}
