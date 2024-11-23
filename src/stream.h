// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      stream.h - TStream object                        *
// *************************************************************************

#pragma once

#include "revenant.h"

// *******************************************************
// * TInputStream - Stream object for loading and saving *
// *******************************************************

// The TInputStream object represents an INPUT file or buffer
// stream from which a sector or other streamed data is
// loaded.  The stream object is similar to a C++ stream
// object.

_CLASSDEF(TInputStream)
class TInputStream
{
  public:
    TInputStream(uint8_t *buffer, int32_t bufferlen) { buf = ptr = buffer; buflen = bufferlen; }

    RTInputStream operator >> (int32_t &d) { d = *(int32_t *)ptr; ptr += 4;  return *this; }
    RTInputStream operator >> (uint32_t &d) { d = *(uint32_t *)ptr; ptr += 4;  return *this; }
    RTInputStream operator >> (short &d) { d = *(short *)ptr; ptr += 2;  return *this; }
    RTInputStream operator >> (uint16_t &d) { d = *(uint16_t *)ptr; ptr += 2;  return *this; }
    RTInputStream operator >> (char &d) { d = *(char *)ptr; ptr += 1;  return *this; }
    RTInputStream operator >> (uint8_t &d) { d = *(uint8_t *)ptr; ptr += 1;  return *this; }
    RTInputStream operator >> (char *d);

    void *Buffer() { return buf; }
        // Returns pointer to buf
    int32_t BufferSize() { return buflen; }
        // Returns size of buffer
    int32_t DataSize() { return buflen; }
        // Returns size of data
    void Reset()
      { ptr = buf; }
        // Resets read positon
    int32_t GetPos()
      { return (int32_t)(ptr - buf); }
        // Gets read position
    bool SetPos(int32_t newpos)
      { if ((uint32_t)newpos < (uint32_t)buflen) { ptr = buf + newpos; return true; } else return false; }
        // Sets read position (if not past end of buffer)
    bool MovePos(int32_t newpos)
      { return SetPos(GetPos() + newpos); }
        // Sets read position (if not past end of buffer)

  private:
    uint8_t *buf, *ptr;                    // Pointers
    int32_t buflen;                        // Sizes
};

_CLASSDEF(TOutputStream)
class TOutputStream
{
  public:
    TOutputStream(int32_t startsize, int32_t growsize);
    virtual ~TOutputStream();

    RTOutputStream operator << (int32_t d) { *(int32_t *)ptr = d; ptr += 4;  return *this; }
    RTOutputStream operator << (uint32_t d) { *(uint32_t *)ptr = d; ptr += 4;  return *this; }
    RTOutputStream operator << (short d) { *(short *)ptr = d; ptr += 2;  return *this; }
    RTOutputStream operator << (uint16_t d) { *(uint16_t *)ptr = d; ptr += 2;  return *this; }
    RTOutputStream operator << (char d) { *(char *)ptr = d; ptr += 1;  return *this; }
    RTOutputStream operator << (uint8_t d) { *(uint8_t *)ptr = d; ptr += 1;  return *this; }
    RTOutputStream operator << (char *d);

    void *Buffer() { return buf; }
        // Returns pointer to buf
    int32_t BufferSize() { return bufsize; }
        // Returns size of buffer
    int32_t DataSize() { return (int32_t)(ptr - buf); }
        // Returns size of data
    void MakeFreeSpace(int32_t freespace);
        // Makes sure there is at least this amount of free space at end of buffer
    void Reset()
      { ptr = buf; }
        // Resets write positon
    int32_t GetPos()
      { return (int32_t)(ptr - buf); }
        // Gets read position
    bool SetPos(int32_t newpos)
      { if ((uint32_t)newpos < (uint32_t)bufsize) { ptr = buf + newpos; return true; } else return false; }
        // Sets read position (if not past end of buffer)
    bool MovePos(int32_t newpos)
      { return SetPos(GetPos() + newpos); }
        // Sets read position (if not past end of buffer)

  private:
    uint8_t *buf, *ptr;          // Pointers
    int32_t bufsize, growsize;   // Sizes
};


