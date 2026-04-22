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
        const uint16_t* src = bm->data16;
        for (int32_t y = 0; y < h; y++)
        {
            uint8_t* row = dst + (oy + y) * dst_pitch + ox * 4;
            for (int32_t x = 0; x < w; x++)
            {
                const uint16_t px = src[y * w + x];
                if (px == key) { row[0]=row[1]=row[2]=row[3]=0; }
                else                           { Decode555(px, row); }
                row += 4;
            }
        }
        return true;
    }
    return false;
}
