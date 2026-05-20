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

}  // namespace

bool DecodeBitmapToRGBA(PTBitmap bm, uint8_t* dst, int32_t dst_pitch,
                        int32_t ox, int32_t oy)
{
    if (!bm || bm->width <= 0 || bm->height <= 0) return false;
    if (bm->flags & BM_COMPRESSED) return false;  // TODO decompressor
    const int32_t w = bm->width, h = bm->height;

    // BM_ALIAS bitmaps (anti-aliased sprites: cursor/wedge shadows, soft
    // UI text shadows, glows) store their real pixels as an RLE coverage
    // stream in the alias buffer, NOT in the data8/data16 array. Per
    // scanline: alternating (skip-count, run-count, [color16, alpha5]*)
    // groups; AL_EOL ends a line, AL_EOD ends the stream. alpha5 is 0..31
    // coverage (31 = opaque). Legacy blitted this via MMX asm
    // (graphics.cpp PutAlias*, now #if 0); this is the C port of that
    // format. Without it these bitmaps decode from an empty/garbage data
    // array -> invisible (the cursor-shadow bug).
    if ((bm->flags & BM_ALIAS) && bm->alias.ptr())
    {
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
                        Decode555(px, row);                       // RGB (sets A=255)
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
        for (int32_t y = 0; y < h; y++)
        {
            uint8_t* row = dst + (oy + y) * dst_pitch + ox * 4;
            for (int32_t x = 0; x < w; x++)
            {
                const uint16_t px = src[y * w + x];
                if (px == key || px == MAGENTA_KEY)
                {
                    row[0]=row[1]=row[2]=row[3]=0;
                }
                else
                {
                    Decode555(px, row);
                }
                row += 4;
            }
        }
        return true;
    }
    return false;
}
