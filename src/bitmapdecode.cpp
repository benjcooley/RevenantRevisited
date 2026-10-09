// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               bitmapdecode.cpp - Bitmap decode helpers                *
// *************************************************************************

#include "bitmapdecode.h"

#include "bitmap.h"

#include <algorithm>

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

// One pixel of retail's hue-change blit (FUN_004b21d0; the 1998 source is
// PutHueChange in graphics.cpp, the same arithmetic). Channels are 5-bit
// values times 8; a 16-bit pixel is read 5:5:5 with green's low bit dropped
// (`>> 6`), as retail reads a 16-bit display. A green-dominant pixel keeps its
// value (green) and saturation and takes `hue`; the HSV terms truncate as
// retail's __ftol does. Other pixels, and hues of 360 and up, are unchanged.
uint16_t HueChangePixel(uint16_t px, int32_t hue, bool rgb565)
{
    int32_t r = ((rgb565 ? px >> 11 : px >> 10) & 0x1f) << 3;
    int32_t g = ((rgb565 ? px >> 6 : px >> 5) & 0x1f) << 3;
    int32_t b = (px & 0x1f) << 3;
    if (g <= r || g <= b)
        return px;

    const double v = double(g) / 255.0;
    const double s = (double(g) - double((std::min)(r, b))) / double(g);
    const int32_t range = hue / 60;
    const double f = double(hue) / 60.0 - double(range);
    const int32_t p  = int32_t((v * (1 - s)) * 255.0);
    const int32_t q  = int32_t((v * (1 - (s * f))) * 255.0);
    const int32_t t  = int32_t((v * (1 - (s * (1 - f)))) * 255.0);
    const int32_t v0 = int32_t(v * 255.0);
    switch (range)
    {
        case 0: r = v0; g = t;  b = p;  break;
        case 1: r = q;  g = v0; b = p;  break;
        case 2: r = p;  g = v0; b = t;  break;
        case 3: r = p;  g = q;  b = v0; break;
        case 4: r = t;  g = p;  b = v0; break;
        case 5: r = v0; g = p;  b = q;  break;
        default: return px;
    }
    r >>= 3;
    g >>= 3;
    b >>= 3;
    return rgb565 ? uint16_t((r << 11) | (g << 6) | b) : uint16_t((r << 10) | (g << 5) | b);
}

}  // namespace

bool DecodeBitmapHueChangedToRGBA(const TBitmap* bm, int32_t hue,
                                  uint8_t* dst, int32_t dst_pitch)
{
    if (!bm || !dst || bm->width <= 0 || bm->height <= 0 || hue < 0)
        return false;
    if (!(bm->flags & (BM_15BIT | BM_16BIT)) || (bm->flags & (BM_COMPRESSED | BM_ARGB4444)))
        return false;

    const bool rgb565 = (bm->flags & BM_16BIT) != 0;
    const uint16_t key = uint16_t(bm->keycolor);
    const uint16_t* src = bm->data16;
    for (int32_t y = 0; y < bm->height; y++)
    {
        uint8_t* row = dst + y * dst_pitch;
        for (int32_t x = 0; x < bm->width; x++, row += 4)
        {
            const uint16_t px = src[y * bm->width + x];
            const uint16_t out = px == key ? 0 : HueChangePixel(px, hue, rgb565);
            if (out == 0)
            {
                row[0] = row[1] = row[2] = row[3] = 0;     // DM_TRANSPARENT skips 0
                continue;
            }
            if (rgb565)
                Decode565(out, row);
            else
                Decode555(out, row);
        }
    }
    return true;
}

bool DecodeBitmapToRGBA(PTBitmap bm, uint8_t* dst, int32_t dst_pitch,
                        int32_t ox, int32_t oy, EBitmapDecode decode)
{
    if (!bm || bm->width <= 0 || bm->height <= 0) return false;
    if (bm->flags & BM_COMPRESSED) return false;  // TODO decompressor
    const int32_t w = bm->width, h = bm->height;

    // Alias path: only when the draw asked for it (a shadow / glow draw).
    // BM_ALIAS alone is NOT sufficient -- the cursor sprite carries both a
    // real data array AND an alias buffer, and must decode from the data
    // array. Retail keyed this off the DM_ALIAS draw mode, not the BM_ALIAS
    // bitmap flag; EBitmapDecode::Alias is our equivalent.
    //
    // The alias buffer stores anti-aliased pixels as an RLE coverage
    // stream: per scanline, alternating (skip-count, run-count,
    // [color16, alpha5]*) groups; AL_EOL ends a line, AL_EOD ends the
    // stream; alpha5 is 0..31 coverage (31 = opaque). C port of the
    // legacy MMX PutAlias* blit (graphics.cpp, now #if 0).
    if (decode == EBitmapDecode::Alias && (bm->flags & BM_ALIAS) && bm->alias.ptr())
    {
        const bool rgb565 = (bm->flags & BM_16BIT) != 0;
        const uint8_t* a   = (const uint8_t*)bm->alias.ptr();
        const uint8_t* aend = a + bm->aliassize;

        // Skipped pixels stay transparent; zero the target region first
        // so the function is self-contained regardless of caller state.
        for (int32_t y = 0; y < h; y++)
            memset(dst + (oy + y) * dst_pitch + ox * 4, 0, (size_t)w * 4);

        int32_t y = 0;
        while (y < h && a < aend)
        {
            uint8_t code = *a++;                 // first code of the line
            if (code == AL_EOD) break;
            if (code == AL_EOL) { y++; continue; } // blank line

            int32_t x = 0;
            for (;;)
            {
                x += code;                       // `code` is a skip count
                if (a >= aend) break;
                const uint8_t run = *a++;
                for (uint8_t i = 0; i < run && a + 3 <= aend; i++)
                {
                    const uint16_t px  = (uint16_t)(a[0] | (a[1] << 8));
                    const uint8_t  cov = a[2];   // 0..31 coverage
                    a += 3;
                    if (x >= 0 && x < w)
                    {
                        uint8_t* row = dst + (oy + y) * dst_pitch + (ox + x) * 4;
                        if (rgb565) Decode565(px, row);           // RGB (sets A=255)
                        else        Decode555(px, row);
                        row[3] = (uint8_t)((cov * 255) / 31);     // override A = coverage
                    }
                    x++;
                }
                if (a >= aend) break;
                code = *a++;                      // next skip count OR AL_EOL
                if (code == AL_EOL) break;
            }
            y++;
        }
        return true;
    }

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
    // BM_ARGB4444: 2 bytes/pixel, bits 15..12 alpha, 11..8 red, 7..4 green,
    // 3..0 blue, no palette and no separate alpha buffer (statusbar.dat,
    // automap.dat, the texture-overlay HUD's art). Decoded the way retail's
    // texture-overlay raster turns a 4444 texel into a pixel (measured in the
    // emulator, docs/ui/HUD_REBUILD.md P1b): colour channels shift up
    // (red 15 -> 0xF0, which a 565 target reads back as 30 of 31, as retail
    // shows it), alpha bit-replicates so 15 stays fully opaque.
    if (bm->flags & BM_ARGB4444)
    {
        const uint16_t* src = bm->data16;
        for (int32_t y = 0; y < h; y++)
        {
            uint8_t* row = dst + (oy + y) * dst_pitch + ox * 4;
            for (int32_t x = 0; x < w; x++)
            {
                const uint16_t px = src[y * w + x];
                const uint8_t a4 = (uint8_t)((px >> 12) & 0x0F);
                row[0] = (uint8_t)(((px >> 8) & 0x0F) << 4);
                row[1] = (uint8_t)(((px >> 4) & 0x0F) << 4);
                row[2] = (uint8_t)((px & 0x0F) << 4);
                row[3] = (uint8_t)((a4 << 4) | a4);
                row += 4;
            }
        }
        return true;
    }
    if (bm->flags & (BM_15BIT | BM_16BIT))
    {
        const bool rgb565 = (bm->flags & BM_16BIT) != 0;
        const bool overlay4444 = decode == EBitmapDecode::Overlay4444;
        const uint16_t key = (uint16_t)bm->keycolor;
        // Magenta (R=max,G=0,B=max) is the implicit transparency key in many
        // Revenant retail sprites. Match the packed value to the pixel format:
        // RGB555 => 0x7c1f, RGB565 => 0xf81f.
        const uint16_t magentaKey = rgb565 ? 0xf81f : 0x7c1f;
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
                if (a == 0 || (!overlay4444 && px == key) || px == magentaKey)
                {
                    row[0]=row[1]=row[2]=row[3]=0;
                }
                else if (overlay4444)
                {
                    // REVSYNC: retail's 16-bit -> ARGB4444 blit truncates
                    // (measured with the emulator's Put oracle).
                    const uint16_t px565 = rgb565 ? px
                                                  : uint16_t((px & 0x7c00) << 1 | (px & 0x03e0) << 1 | (px & 0x1f));
                    row[0] = uint8_t((px565 >> 12) << 4);
                    row[1] = uint8_t(((px565 >> 7) & 0xf) << 4);
                    row[2] = uint8_t(((px565 >> 1) & 0xf) << 4);
                    row[3] = a;
                }
                else
                {
                    if (rgb565) Decode565(px, row);
                    else        Decode555(px, row);
                    row[3] = a;
                }
                row += 4;
            }
        }
        return true;
    }
    return false;
}
