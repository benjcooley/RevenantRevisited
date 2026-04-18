// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 chunkcache.cpp - Chunk Cache Object                   *
// *************************************************************************

#include "revenant.h"
#include "bitmap.h"
#include "decompdata.h"
#include "chunkcache.h"

#include <cstring>

// Chunk compression format (per-chunk):
//   int32   chunk id
//   uint8   dle_rle    // byte value reserved as the RLE escape
//   uint8   dle_lz     // byte value reserved as the LZ escape
//   byte stream organized as CHUNKHEIGHT rows. For every source byte `op`:
//     op == dle_rle → next byte is count:
//        count == 0       → end of row
//        count >= 0x80    → skip (count & 0x7f) output pixels
//        count  < 0x80    → fill `count` pixels with the following data byte
//     op == dle_lz  → next byte is count:
//        count == 0 (16-bit variant only) → next byte sets the high-byte
//                                           state used for 16-bit writes
//        count  > 0 → next two bytes form a little-endian distance; copy
//                     `count` bytes from (post-payload - distance - 4) in
//                     the source stream forward into dest
//     otherwise           → write `op` as a raw pixel
//
// The LZ back-reference walks backwards through the *source* stream (not
// the output), and the -4 subtracted from the distance compensates for
// the 4-byte chunk-id prefix sitting before the DLE header.

TChunkCache::TChunkCache()
{
    numchunks = 0;
    chunks = chunks16 = nullptr;
    id = used = id16 = used16 = nullptr;
    chunkbuffer = chunkbuffer16 = nullptr;
}

void TChunkCache::AllocCache(int32_t megabytes)
{
    numchunks = (megabytes * 1024 * 1024) / (CHUNKWIDTH * CHUNKHEIGHT * 3);

    chunks = (void **)malloc(numchunks * sizeof(void *));
    id = (int32_t *)malloc(numchunks * sizeof(int32_t));
    used = (int32_t *)malloc(numchunks * sizeof(int32_t));

    chunks16 = (void **)malloc(numchunks * sizeof(void *));
    id16 = (int32_t *)malloc(numchunks * sizeof(int32_t));
    used16 = (int32_t *)malloc(numchunks * sizeof(int32_t));

    for (int32_t loop = 0; loop < numchunks; loop++)
    {
        chunks[loop] = nullptr;
        id[loop]     = 0;
        used[loop]   = 0;
    
        chunks16[loop] = nullptr;
        id16[loop]     = 0;
        used16[loop]   = 0;
    }   

    currentcycle   = 0;
    currentcycle16 = 0;
    chunkbuffer    = (char *)malloc(CHUNKWIDTH * CHUNKHEIGHT * numchunks);
//  if (!VirtualLock(chunkbuffer, CHUNKWIDTH * CHUNKHEIGHT * numchunks))
//  {
//      Error("Error");
//  }
    chunkbuffer16  = (char *)malloc(CHUNKWIDTH * CHUNKHEIGHT * numchunks * 2);
//  if (!VirtualLock(chunkbuffer16, CHUNKWIDTH * CHUNKHEIGHT * numchunks * 2))
//  {
//      Error("Error");
//  }
}

uint32_t TChunkCache::MemUsed()
{
    return numchunks * CHUNKWIDTH * CHUNKHEIGHT * 3;
}

void *TChunkCache::AddChunk(void *chunk, uint32_t type)
{
    if (chunk == nullptr || chunks == nullptr)
        return nullptr;

    BEGIN_CRITICAL(); // Don't allow two threads at once

    int32_t oldest    = 0x7fffffff;
    int32_t oldestptr = 0;

    currentcycle++;

    int32_t *value = (int32_t *)chunk;
    int32_t number = *value;

    int32_t *idptr = id;
    int32_t *usedptr = used;
    for (int32_t loop = 0; loop < numchunks; loop++, idptr++, usedptr++)
    {
        if (*idptr == number)
        {
            used[loop] = currentcycle;
            END_CRITICAL();
            return chunks[loop];
        }
            
        if (*usedptr < oldest)
        {
            oldest     = used[loop];
            oldestptr  = loop;
        }
    }

    used[oldestptr]   = currentcycle;                                         
    chunks[oldestptr] = (void *)((char *)chunkbuffer + (oldestptr * CHUNKWIDTH * CHUNKHEIGHT));

    END_CRITICAL(); // 

    number = ChunkDecompress(chunk, chunks[oldestptr], type);
    id[oldestptr] = number;

    return chunks[oldestptr];
}

void *TChunkCache::AddChunkZ(void *chunk, uint32_t type)
{
    if (chunk == nullptr || chunks == nullptr)
        return nullptr;

    BEGIN_CRITICAL(); // Don't allow two threads at once

    int32_t oldest    = 0x7fffffff;
    int32_t oldestptr = 0;

    currentcycle16++;

    int32_t *value = (int32_t *)chunk;
    int32_t number = *value;

    for (int32_t loop = 0; loop < numchunks; loop++)
    {
        if (id16[loop] == number)
        {
            used16[loop] = currentcycle16;
            END_CRITICAL(); 
            return chunks16[loop];
        }
            
        if (used16[loop] < oldest)
        {
            oldest     = used16[loop];
            oldestptr  = loop;
        }
    }

    used16[oldestptr]   = currentcycle16;
    chunks16[oldestptr] = (void *)((char *)chunkbuffer16 + (oldestptr * CHUNKWIDTH * CHUNKHEIGHT * 2));

    END_CRITICAL(); // 

    number = ChunkDecompressZ(chunk, chunks16[oldestptr], type);
    id16[oldestptr] = number;

    return chunks16[oldestptr];
}

void *TChunkCache::AddChunk16(void *chunk, uint32_t type)
{
    if (chunk == nullptr || chunks == nullptr)
        return nullptr;

    BEGIN_CRITICAL(); // Don't allow two threads at once

    int32_t oldest    = 0x7fffffff;
    int32_t oldestptr = 0;

    currentcycle16++;

    int32_t *value = (int32_t *)chunk;
    int32_t number = *value;

    for (int32_t loop = 0; loop < numchunks; loop++)
    {
        if (id16[loop] == number)
        {
            used16[loop] = currentcycle16;
            END_CRITICAL(); 
            return chunks16[loop];
        }
            
        if (used16[loop] < oldest)
        {
            oldest     = used16[loop];
            oldestptr  = loop;
        }
    }

    used16[oldestptr]   = currentcycle16;
    chunks16[oldestptr] = (void *)((char *)chunkbuffer16 + (oldestptr * CHUNKWIDTH * CHUNKHEIGHT * 2));

    END_CRITICAL(); // 

    number = ChunkDecompress16(chunk, chunks16[oldestptr], type);
    id16[oldestptr] = number;

    return chunks16[oldestptr];
}

bool TChunkCache::RemoveChunk(int32_t number)
{
    if (chunks == nullptr)
        return false;

    BEGIN_CRITICAL(); // Don't allow two threads at once

    int32_t loop = 0;
    for (; loop < numchunks; loop++)
    {
        int32_t cycle = currentcycle - 10;

        if (id[loop] == number)
        {
            used[loop] = cycle;
            id[loop]   = 0;

            break;
        }
    }

    END_CRITICAL(); //

    if (loop == numchunks)
        return false;

    return true;
}

bool TChunkCache::RemoveChunk16(int32_t number)
{
    if (chunks == nullptr)
        return false;

    BEGIN_CRITICAL(); // Don't allow two threads at once

    int32_t loop = 0;
    for (; loop < numchunks; loop++)
    {
        int32_t cycle = currentcycle16 - 10;

        if (id16[loop] == number)
        {
            used16[loop] = cycle;
            id16[loop]   = 0;

            break;
        }
    }

    END_CRITICAL(); //

    if (loop == numchunks)
        return false;

    return true;
}

int32_t TChunkCache::ChunkDecompress(void *source, void *dest, uint32_t clear)
{
    if (chunks == nullptr)
        return -1;

    const uint8_t *src = static_cast<const uint8_t *>(source);
    const int32_t number = *reinterpret_cast<const int32_t *>(src);
    src += 4;

    constexpr int32_t total = CHUNKWIDTH * CHUNKHEIGHT;
    uint8_t *dst = static_cast<uint8_t *>(dest);

    std::memset(dst, (clear == 1) ? 0x00 : 0xFF, total);

    const uint8_t dle_rle = src[0];
    const uint8_t dle_lz  = src[1];
    src += 2;

    int32_t row = CHUNKHEIGHT;
    while (row > 0)
    {
        const uint8_t op = *src++;
        if (op == dle_rle)
        {
            const uint8_t count = *src++;
            if (count == 0) { --row; continue; }
            if (count >= 0x80) { dst += (count & 0x7F); continue; }
            const uint8_t data = *src++;
            std::memset(dst, data, count);
            dst += count;
        }
        else if (op == dle_lz)
        {
            const uint8_t count = *src++;
            const uint16_t dist =
                static_cast<uint16_t>(src[0]) |
                (static_cast<uint16_t>(src[1]) << 8);
            src += 2;
            const uint8_t *back = src - dist - 4;
            for (int32_t i = 0; i < count; ++i)
                *dst++ = *back++;
        }
        else
        {
            *dst++ = op;
        }
    }

    return number;
}

int32_t TChunkCache::ChunkDecompressZ(void *source, void *dest, uint32_t /*clear*/)
{
    if (chunks == nullptr)
        return -1;

    const uint8_t *src = static_cast<const uint8_t *>(source);
    const int32_t number = *reinterpret_cast<const int32_t *>(src);
    src += 4;

    constexpr int32_t total = CHUNKWIDTH * CHUNKHEIGHT;
    uint16_t *dst = static_cast<uint16_t *>(dest);

    for (int32_t i = 0; i < total; ++i)
        dst[i] = 0x7F7F;

    const uint8_t dle_rle = src[0];
    const uint8_t dle_lz  = src[1];
    src += 2;

    uint8_t highbyte = 0;
    int32_t row = CHUNKHEIGHT;
    while (row > 0)
    {
        const uint8_t op = *src++;
        if (op == dle_rle)
        {
            const uint8_t count = *src++;
            if (count == 0) { --row; continue; }
            if (count >= 0x80) { dst += (count & 0x7F); continue; }
            const uint8_t data = *src++;
            const uint16_t word = static_cast<uint16_t>(highbyte) << 8 | data;
            for (int32_t i = 0; i < count; ++i)
                *dst++ = word;
        }
        else if (op == dle_lz)
        {
            const uint8_t count = *src++;
            if (count == 0)
            {
                // Set highbyte state (no pixels written).
                highbyte = *src++;
                continue;
            }
            const uint16_t dist =
                static_cast<uint16_t>(src[0]) |
                (static_cast<uint16_t>(src[1]) << 8);
            src += 2;
            const uint8_t *back = src - dist - 4;
            const uint16_t high = static_cast<uint16_t>(highbyte) << 8;
            for (int32_t i = 0; i < count; ++i)
                *dst++ = high | *back++;
        }
        else
        {
            *dst++ = static_cast<uint16_t>(highbyte) << 8 | op;
        }
    }

    return number;
}

int32_t TChunkCache::ChunkDecompress16(void *source, void *dest, uint32_t clear)
{
    if (chunks == nullptr)
        return -1;

    int32_t *buffer = (int32_t *)source;
    int32_t number  = *buffer;
    buffer++;

    memcpy(dest, buffer, CHUNKWIDTH * CHUNKHEIGHT * 2);

    return number;
}

TChunkCache::~TChunkCache()
{
//  VirtualUnlock(chunkbuffer, CHUNKWIDTH * CHUNKHEIGHT * numchunks);
//  VirtualUnlock(chunkbuffer16, CHUNKWIDTH * CHUNKHEIGHT * numchunks * 2);

    if (chunks)
        free(chunks);
    if (id)
        free(id);
    if (used)
        free(used);
    if (chunks16)
        free(chunks16);
    if (id16)
        free(id16);
    if (used16)
        free(used16);
    if (chunkbuffer)
        free(chunkbuffer);
    if (chunkbuffer16)
        free(chunkbuffer16);
}

