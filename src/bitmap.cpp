// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     bitmap.cpp - Bitmap objects                       *
// *************************************************************************

#include "bitmap.h"

#include "display.h"
#include "bitmapdata.h"
#include "graphics.h"
#include "resource.h"
#include "font.h"
#include "chunkcache.h"
#include "decompdata.h"

#include <algorithm>
#include <vector>

PTBitmap TBitmap::NewBitmap(int32_t width, int32_t height, int32_t bmflags, 
    int32_t aliasbufsize)
{
    int32_t bytesperpixel;

    switch (bmflags & (BM_8BIT + BM_2BYTEFORMATS + BM_24BIT + BM_32BIT))
    {
        case BM_8BIT:
        bytesperpixel=1;
        break;
    
        case BM_15BIT:
        case BM_16BIT:
        case BM_ARGB4444:
        case BM_ARGB1555:
        bytesperpixel=2;
        break;

        case BM_24BIT:
        bytesperpixel=3;
        break;

        case BM_32BIT:
        bytesperpixel=4;
        break;

        default:
        return nullptr;
        break;
    }   

    int32_t allocval = (bytesperpixel * width * height) + sizeof(TBitmap) - 
        4 + aliasbufsize;
    
    int32_t zbuffersize = 0;
    int32_t normalsize  = 0;
    int32_t alphasize   = 0;
    int32_t palettesize = 0;

    if (bmflags & BM_ZBUFFER) 
    {
        allocval   += (width << 1) * height;
        zbuffersize = (width << 1) * height;
    }
    
    if (bmflags & BM_NORMALS)
    {
        allocval  += (width << 1) * height;
        normalsize = (width << 1) * height;
    }

    if (bmflags & BM_ALPHA)
    {
        allocval += (width << 1) * height;
        alphasize = (width << 1) * height;
    }
    
    if (bmflags & BM_PALETTE) 
    {
        allocval   += sizeof(SPalette);
        palettesize = sizeof(SPalette);
    }
    
    PTBitmap bitmap = (PTBitmap) new uint8_t[allocval];
    if (bitmap == nullptr) return nullptr;
    
    bitmap->width  = width;
    bitmap->height = height;
    bitmap->flags  = bmflags;

    int32_t drawmode = 0;

    if (bmflags & BM_ZBUFFER) 
        drawmode |= DM_ZBUFFER;
    
    if (bmflags & BM_NORMALS) 
        drawmode |= DM_NORMALS;

    if (bmflags & BM_ALIAS) 
        drawmode |= DM_ALIAS;

    if (bmflags & BM_ALPHA) 
        drawmode |= DM_ALPHA;

    bitmap->drawmode    = drawmode;
    bitmap->keycolor    = 0;

    bitmap->aliassize   = aliasbufsize;
    if (aliasbufsize)
      bitmap->alias.set((void *)((uint8_t*)bitmap + bytesperpixel * width * height + sizeof(TBitmap) - 4));
    else
      bitmap->alias.set(0);

    bitmap->alphasize   = alphasize;
    if (alphasize)
      bitmap->alpha.set((void *)((uint8_t*)bitmap + bytesperpixel * width * height + sizeof(TBitmap) 
                        + aliasbufsize - 4));
    else
      bitmap->alpha.set(0);

    bitmap->zbuffersize = zbuffersize;
    if (zbuffersize)
      bitmap->zbuffer.set((void *)((uint8_t*)bitmap + bytesperpixel * width * height 
                        + sizeof(TBitmap) + aliasbufsize + alphasize - 4));
    else
      bitmap->zbuffer.set(0);

    bitmap->normalsize  = normalsize;
    if (normalsize)
      bitmap->normal.set((void *)((uint8_t*)bitmap + bytesperpixel * width * height 
                        + sizeof(TBitmap) + aliasbufsize + alphasize + zbuffersize - 4));
    else
      bitmap->normal.set(0);

    bitmap->palettesize = palettesize;
    if (palettesize)
      bitmap->palette.set((void *)((uint8_t*)bitmap + bytesperpixel * width * height + sizeof(TBitmap) + aliasbufsize
                        + alphasize + zbuffersize + normalsize - 4));
    else
      bitmap->palette.set(0);

    bitmap->datasize    = bytesperpixel * width * height;

    return bitmap;
}

PTBitmap TBitmap::Load(int32_t resource)
{
    PTBitmap bitmap = (PTBitmap)LoadResource("bitmap", resource);
    return bitmap;
}

// ****************************
// * Bitmap Drawing Functions *
// ****************************

void TBitmap::WriteText(char *text, int32_t x, int32_t y, int32_t lines, TFont* font, PSColor color, uint32_t drawmode)
{
    SDrawBlock db;
    SDrawParam dp;
    STextParam tp;

    memset(&db, 0, sizeof(SDrawBlock));
    memset(&dp, 0, sizeof(SDrawParam));
    memset(&tp, 0, sizeof(STextParam));

    db.srcbitmapflags = BM_15BIT | BM_ALIAS;
    db.dstbitmapflags = flags;

    db.dest = &data16;
    if (db.dest == nullptr) 
        return;

    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = width;
    db.keycolor   = 0;

    dp.func = TextDraw;

    if (drawmode == DM_USEDEFAULT)
        dp.drawmode = FONT_DRAWMODE;
    else
        dp.drawmode = drawmode;

    dp.originx    = 0;
    dp.originy    = 0;
                   
    dp.clipx      = 0;
    dp.clipy      = 0;
    dp.clipwidth  = width;
    dp.clipheight = height;

    dp.data = (void *)&tp;

    if (color)
    {
        dp.drawmode |= DM_CHANGECOLOR;
        dp.color = TranslateColor(*color);
    }
    else
        dp.color = 0;

    dp.sx         = 0;
    dp.sy         = 0;

    dp.dx         = x;
    dp.dy         = y;

    dp.dwidth  = width;
    dp.dheight = height;

    tp.text = text;
    tp.numlines = lines;
    tp.font = font;
    tp.wrapwidth = width;
    tp.startline = 0;
    tp.justify = 0;
    tp.length = 0; // Length of text drawn

    Draw(&db, &dp);
};

// **********************************
// * Bitmap Decompressing Functions *
// **********************************

// *********** Chunk Bitmap Transfer Routines *********
bool TBitmap::CacheChunks()
{
    if (!(flags & BM_CHUNKED))
        return false;

    SChunkHeader* hdr  = (SChunkHeader*)(void *)data8;
    SChunkHeader* zhdr = (SChunkHeader*)(void *)zbuffer;
    SChunkHeader* nhdr = (SChunkHeader*)(void *)normal;

    int32_t type   = hdr->type;
    int32_t width  = hdr->width;
    int32_t height = hdr->height;

    for (int32_t outerloop = 0; outerloop < hdr->height; outerloop++)
    {
        for (int32_t innerloop = 0; innerloop < hdr->width; innerloop++)
        {
            ChunkCache.AddChunk(hdr->block[outerloop * width + innerloop].ptr(), 1);
            
            if (zhdr)
                ChunkCache.AddChunkZ(zhdr->block[outerloop * width + innerloop].ptr(), 2);

            if (nhdr)
                ChunkCache.AddChunk16(nhdr->block[outerloop * width + innerloop].ptr(), 1);
        }
    }

    return true;
}

// **************************
// * Misc. Bitmap Functions *
// **************************

bool TBitmap::Clear(SColor &color, uint32_t bmdrawmode, uint16_t zpos)
{
    if (width < 1 || height < 1) return false;

    if (datasize < 1)
        return false;

    if (bmdrawmode == DM_USEDEFAULT)
        bmdrawmode = DM_ZBUFFER | DM_NORMALS;

    SDrawBlock  db;
    memset(&db, 0, sizeof(SDrawBlock));

    SDrawParam  dp;
    MakeDP(dp, 0, 0, 0, 0, width, height, bmdrawmode);

    db.dest       = (uint16_t *)data16;
    db.dzbuffer   = (uint16_t *)zbuffer.ptr();
    db.dzstride   = width;
    db.dnormals   = (uint16_t *)normal.ptr();

    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = width;

    uint32_t bytesperpixel;

    switch (flags & (BM_8BIT | BM_15BIT | BM_16BIT | BM_24BIT | BM_32BIT))
    {
        case BM_8BIT:
            db.dstbitmapflags = BM_8BIT;
            bytesperpixel = 1;
            return false;
        break;
    
        case BM_15BIT:
        case BM_16BIT:
            db.dstbitmapflags = BM_16BIT;
            bytesperpixel = 2;
        break;

        case BM_24BIT:
            db.dstbitmapflags = BM_24BIT;
            bytesperpixel = 3;
        break;

        case BM_32BIT:
            db.dstbitmapflags = BM_32BIT;
            bytesperpixel = 4;
            break;

        default:
            return false;
        break;
    }   

    uint32_t newcolor;

    if (bytesperpixel == 2) 
    {
        newcolor  = TranslateColor(color);
        newcolor  = (newcolor << 16) | newcolor;
    }

    else
        newcolor  = (color.red << 24) | (color.green << 16) | (color.blue << 8);
    
    dp.func       = ::Box;
    dp.intensity  = newcolor;
    dp.zpos       = zpos;

    dp.clipx      = 0;
    dp.clipy      = 0;
    dp.clipwidth  = width;
    dp.clipheight = height;
    dp.color      = newcolor;

    return Draw(&db, &dp);
}

// Copies one bitmap to another 

bool TBitmap::RawPut(int32_t x, int32_t y, 
                  PTBitmap bitmap, int32_t srcx, int32_t srcy, int32_t srcw, int32_t srch, int32_t drawmode,
                  uint32_t color, int32_t intensity, uint16_t zpos, DRAWFUNCTION func, void *data)
{
    if (!bitmap)
        return false;

    SDrawBlock  db;
    SDrawParam  dp;

    memset(&db, 0, sizeof(SDrawBlock));
    memset(&dp, 0, sizeof(SDrawParam));

    db.srcbitmapflags = bitmap->flags;
    db.dstbitmapflags = flags;

    db.dest = &data16;
    if (db.dest == nullptr) 
        return false;

    db.source = &bitmap->data16;
    if (db.source == nullptr) 
        return false;

    db.sbufwidth  = bitmap->width;
    db.sbufheight = bitmap->height;
    db.sstride    = db.sbufwidth;

    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = width;

    db.szbuffer   = (uint16_t *)bitmap->zbuffer.ptr();
    db.szstride   = bitmap->width;
    db.dzbuffer   = (uint16_t *)zbuffer.ptr();
    db.dzstride   = width;

    db.snormals   = (uint16_t *)bitmap->normal.ptr();
    db.dnormals   = (uint16_t *)normal.ptr();

    db.palette    = (uint16_t *)bitmap->palette.ptr();
    db.alpha      = (uint8_t *)bitmap->alpha.ptr();
    db.alias      = (uint8_t *)bitmap->alias.ptr();

    db.keycolor   = bitmap->keycolor;

    if (drawmode == DM_USEDEFAULT)
        drawmode = bitmap->drawmode;

    dp.drawmode   = drawmode | DM_NORESTORE;
    dp.originx    = 0;
    dp.originy    = 0;
                   
    dp.clipx      = 0;
    dp.clipy      = 0;
    dp.clipwidth  = width;
    dp.clipheight = height;

    dp.sx         = srcx;
    dp.sy         = srcy;

    dp.dx         = x;
    dp.dy         = y;

    if (drawmode & DM_USEREG)
    {
        dp.dx -= bitmap->regx;
        dp.dy -= bitmap->regy;
    }

    dp.swidth     = dp.dwidth  = srcw;
    dp.sheight    = dp.dheight = srch;

    dp.func       = func;
    dp.data       = data;
    dp.color      = color;
    dp.intensity  = intensity;
    dp.zpos       = zpos;

    return Draw(&db, &dp);
}

bool TBitmap::Put(int32_t x, int32_t y, TSurface* surface, int32_t srcx, int32_t srcy, int32_t srcw, 
    int32_t srch, int32_t drawmode, int32_t intensity)
{
    SDrawBlock  db;
    SDrawParam  dp;

    memset(&db, 0, sizeof(SDrawBlock));
    memset(&dp, 0, sizeof(SDrawParam));

    db.srcbitmapflags = surface->Flags();
    db.dstbitmapflags = flags;

    db.dest = &data16;
    db.source = surface->Lock();
    surface->Unlock();

    if (!db.source || !db.dest) 
        return false;

    db.sbufwidth  = surface->Width();
    db.sbufheight = surface->Height();
    db.sstride    = surface->Stride();

    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = width;

    db.szbuffer   = (uint16_t *)surface->GetZBuffer();
    db.szstride   = surface->Stride();
    db.dzbuffer   = (uint16_t *)zbuffer.ptr();
    db.dzstride   = width;

    db.snormals   = (uint16_t *)surface->GetNormalBuffer();
    db.dnormals   = (uint16_t *)normal.ptr();

    db.palette    = nullptr;
    db.alpha      = (uint8_t *)nullptr;
    db.alias      = (uint8_t *)nullptr;

    dp.drawmode   = drawmode | DM_NORESTORE;
    dp.drawmode   = dp.drawmode & ~DM_ALIAS;
    dp.drawmode   = dp.drawmode & ~DM_ALPHA;
    dp.originx    = 0;
    dp.originy    = 0;
                   
    dp.clipx      = 0;
    dp.clipy      = 0;
    dp.clipwidth  = width;
    dp.clipheight = height;

    dp.sx         = srcx;
    dp.sy         = srcy;

    dp.dx         = x;
    dp.dy         = y;

    dp.swidth     = dp.dwidth  = srcw;
    dp.sheight    = dp.dheight = srch;

    dp.intensity  = intensity;

    return Draw(&db, &dp);
}

// REVSYNC: TBitmap::SaveBMP @ 0x004a2960. Writes a 24-bit BMP of a 15/16-bit
// bitmap, shrunk by `scale` (1-8): each output pixel is the mean of a
// scale x scale block, rows bottom-up, BGR. The output width is padded to a
// multiple of 4 pixels; its last columns read on into the next source row,
// and each block starts (scale - 1) rows above where an even split would
// put it (retail stepped its row pointer back twice), clamped to the first
// row. The header's image-size field is 0 and the file-size field is
// (width * height + 18) * 3, both as retail wrote them. The save thumbnail
// (docs/gameflow/forensics/SAVE_GAME.md §11.7) is a 640x480 capture at
// scale 3.
bool TBitmap::SaveBMP(const char *filename, int32_t scale)
{
    if (width < 1 || height < 1 || !(flags & (BM_15BIT | BM_16BIT)))
        return false;
    scale = std::clamp<int32_t>(scale, 1, 8);

    const int32_t dstwidth  = (width / scale + 3) & ~3;
    const int32_t dstheight = height / scale;

    FILE *f = fopen(filename, "wb");
    if (!f)
        return false;

    auto writeU16 = [&](uint16_t v) { const uint8_t b[2] = { (uint8_t)v, (uint8_t)(v >> 8) }; return fwrite(b, 2, 1, f) == 1; };
    auto writeU32 = [&](uint32_t v) { const uint8_t b[4] = { (uint8_t)v, (uint8_t)(v >> 8), (uint8_t)(v >> 16), (uint8_t)(v >> 24) }; return fwrite(b, 4, 1, f) == 1; };

    constexpr uint32_t kHeadersSize = 14 + 40;
    bool ok = writeU16(0x4D42)                                              // "BM"
           && writeU32((uint32_t)(dstheight * dstwidth + 18) * 3)           // file size
           && writeU16(0) && writeU16(0)
           && writeU32(kHeadersSize)                                        // pixel data offset
           && writeU32(40)                                                  // BITMAPINFOHEADER
           && writeU32((uint32_t)dstwidth) && writeU32((uint32_t)dstheight)
           && writeU16(1) && writeU16(24)                                   // planes, bits
           && writeU32(0) && writeU32(0)                                    // BI_RGB, image size
           && writeU32(0) && writeU32(0) && writeU32(0) && writeU32(0);     // resolution, colors

    const uint16_t *pixels = (const uint16_t *)data16;
    const int64_t pixelcount = (int64_t)width * height;
    std::vector<uint8_t> line((size_t)dstwidth * 3);
    const int32_t blockarea = scale * scale;
    int64_t rowend = (int64_t)(height - 1) * width;     // start of the source row for output row 0

    for (int32_t row = 0; ok && row < dstheight; row++)
    {
        const int64_t start = std::max<int64_t>(rowend - (int64_t)(scale - 1) * width * 2, 0);
        uint8_t *d = line.data();
        for (int32_t x = 0; x < dstwidth; x++)
        {
            uint32_t red = 0, green = 0, blue = 0;
            for (int32_t bx = 0; bx < scale; bx++)
            {
                for (int32_t by = 0; by < scale; by++)
                {
                    const int64_t at = start + (int64_t)x * scale + bx + (int64_t)by * width;
                    const uint16_t v = at < pixelcount ? pixels[at] : 0;
                    if (flags & BM_16BIT)
                    {
                        red   += (v >> 8) & 0xf8;
                        green += (v & 0x07e0) >> 3;
                    }
                    else
                    {
                        red   += (v >> 7) & 0xf8;
                        green += (v & 0x03e0) >> 2;
                    }
                    blue += (uint8_t)(v << 3);
                }
            }
            *d++ = (uint8_t)(blue / blockarea);
            *d++ = (uint8_t)(green / blockarea);
            *d++ = (uint8_t)(red / blockarea);
        }
        ok = fwrite(line.data(), line.size(), 1, f) == 1;
        rowend -= (int64_t)width * scale;
    }

    return (fclose(f) == 0) && ok;
}

bool TBitmap::SaveZBF(char *filename)
{
    if (this->width < 1 || 
        this->height < 1 || 
        !(flags & BM_ZBUFFER) ||
        zbuffer.ptr() == nullptr)
            return false;
    
    FILE *f = fopen(filename, "wb");
    if (!f)
        return false;

    if (fwrite(zbuffer.ptr(), width * height * 2, 1, f) != 1)
    {
        fclose(f);
        return false;
    }

    return true;
}

bool TBitmap::OnPixel(int32_t x, int32_t y)
{
    if (x < 0 || y < 0 || x >= width || y >= height)
        return false;

    if (flags & (BM_15BIT | BM_16BIT))
    {
        uint16_t pixel = *(data16 + (y * width) + x);
        return pixel != keycolor;
    }

    if (flags & BM_8BIT)
    {
        uint8_t pixel = *(data8 + (y * width) + x);
        return pixel != keycolor;
    }

    return true;
}

// *************************
// * Bitmap only functions *
// *************************

// ********** Line Drawing Routines **********
bool TBitmap::Line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, SColor &color)
{
    int32_t width = this->width;
    int32_t height = this->height;

    TBitmap bitmap = *this;
    uint16_t *data=bitmap.data16;

    int32_t bytesperpixel;

    switch (bitmap.flags & (BM_8BIT + BM_15BIT + BM_16BIT + BM_24BIT))
    {
        case BM_8BIT:
            return false;
        break;
    
        case BM_15BIT:
        case BM_16BIT:
            bytesperpixel=2;
        break;

        case BM_24BIT:
            bytesperpixel=3;
        break;

        default:
            return false;
        break;
    }   

    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    
    if (x2 < 0 || x1 >= width || max(y1,y2) < 0 || min(y1,y2) >= height)
        return false;

    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;

    if (x1 < 0)
    {
        y1 += -x1 * dy / dx;
        x1 = 0;
    }
    if (x2 >= width)
    {
        y2 += (width - x2 - 1) * dy / dx;
        x2 = width - 1;
    }

    if (y1 < 0)
    {
        x1 += -y1 * dx / dy;
        y1 = 0;
    }
    
    else if (y1 >= height)
    {
        x1 += (height - y1 - 1) * dx / dy;
        y1 = height - 1;
    }

    if (y2 < 0)
    {
        x2 += -y2 * dx / dy;
        y2 = 0;
    }

    else if (y2 >= height)
    {
        x2 += (height - y2 - 1) * dx / dy;
        y2 = height - 1;
    }

    return true;
}

// ********** Fill Rectangle Routines **********

bool TBitmap::Box(int32_t x1, int32_t y1, int32_t x2, int32_t y2, SColor &color)
{
    int32_t fillwidth = x2 - x1 + 1;
    int32_t fillheight = y2 - y1 + 1;

  // Get offset/add stuff
    int32_t dstoff = (y1 * width) + x1 ;
    int32_t dstadd = width - fillwidth;

    TBitmap bitmap=*this;
    uint16_t *data=bitmap.data16;

    uint16_t c16 = TranslateColor(color);
    uint16_t* row = data + dstoff;
    for (int32_t y = 0; y < fillheight; ++y)
    {
        for (int32_t x = 0; x < fillwidth; ++x)
            row[x] = c16;
        row += width;
    }
    
    return true;
}

uint16_t TranslateColor(SColor &color)
{
    uint16_t red, green, blue, result;

    red = color.red;
    red = red >> 3;
    green = color.green;
    blue = color.blue;
    blue = blue >> 3;
    
    if (Display.BitsPerPixel() == 16)
    {
        green = green >> 2;
        result = (red <<11) | (green << 5) | blue;
    }

    else
    {
        green = green >> 3;
        result = (red <<10) | (green << 5) | blue;
    }
    
    return result;
}

