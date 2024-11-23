// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           chunkcache.h - Decompression Cache Include File             *
// *************************************************************************

#ifndef _CHUNKCACHE_H
#define _CHUNKCACHE_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

_CLASSDEF(TChunkCache)

class TChunkCache
{
  protected:
    int32_t numchunks;

  // 8 bit
    void   **chunks;                // Pointer to chunks in buffer
    int32_t    *id;                     // ID number of chunk
    int32_t    *used;                   // Indicates if spot is taken and how many times accessed
    void   *chunkbuffer;            // Pointer to chunk buffer
    int32_t    currentcycle;            // Current cycle count. Used to remove unused chunks

  // 16 bit
    void   **chunks16;              // Pointer to chunks in buffer
    int32_t    *id16;                   // ID number of chunk
    int32_t    *used16;                 // Indicates if spot is taken and how many times accessed
    void   *chunkbuffer16;          // Pointer to chunk buffer
    int32_t    currentcycle16;          // Current cycle count. Used to remove unused chunks

  public:
    TChunkCache();
      // Constructor

    void AllocCache(int32_t megabytes);
        // Allocates space for chunk cache in MB

    uint32_t MemUsed();
        // Returns memory used by cache (for editor)

    void *AddChunk(void *chunk, uint32_t type);
      // Adds chunk to 8 bit list, unless already present

    void *AddChunkZ(void *chunk, uint32_t type);
      // Adds ZBuffer chunk to 16 bit list, unless already present

    void *AddChunk16(void *chunk, uint32_t type);
      // Adds chunk to 16 bit list, unless already present

    bool RemoveChunk(int32_t number);
      // Removes chunk from buffer

    bool RemoveChunk16(int32_t number);
      // Removes chunk from buffer

    ~TChunkCache();
      // Destructor

//  protected:
    int32_t ChunkDecompress(void *source, void *dest, uint32_t type);
      // Decompresses chunks of a bitmap. Adds chunks to cache

    int32_t ChunkDecompressZ(void *source, void *dest, uint32_t type);
      // Decompresses ZBuffer chunks of a bitmap. Adds chunks to 16 bit cache

    int32_t ChunkDecompress16(void *source, void *dest, uint32_t type);
      // Decompresses chunks of a bitmap. Adds chunks to 16 bit cache
};

#endif