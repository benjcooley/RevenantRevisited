// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      stream.h - TStream object                        *
// *************************************************************************

#pragma once

#include "revenant.h"

#include <cstring>
#include <string>

// *******************************************************
// * TInputStream - Stream object for loading and saving *
// *******************************************************

// The TInputStream object represents an INPUT file or buffer
// stream from which a sector or other streamed data is
// loaded.  The stream object is similar to a C++ stream
// object.
//
// Values are little-endian and unaligned. Reads past the end of the data
// yield zeros and mark the stream overrun instead of reading past the
// buffer: sector and save files come from disk (and from retail), so a
// damaged file must not take the reader with it.

_CLASSDEF(TInputStream)
class TInputStream
{
  public:
    TInputStream(uint8_t *buffer, int32_t bufferlen) : buf(buffer), ptr(buffer), buflen(bufferlen) {}

    TInputStream& operator >> (int32_t &d)  { Read(&d, sizeof(d)); return *this; }
    TInputStream& operator >> (uint32_t &d) { Read(&d, sizeof(d)); return *this; }
    TInputStream& operator >> (short &d)    { Read(&d, sizeof(d)); return *this; }
    TInputStream& operator >> (uint16_t &d) { Read(&d, sizeof(d)); return *this; }
    TInputStream& operator >> (char &d)     { Read(&d, sizeof(d)); return *this; }
    TInputStream& operator >> (uint8_t &d)  { Read(&d, sizeof(d)); return *this; }
    TInputStream& operator >> (char *d);
        // Stream string (uint8 length, then the bytes) into a buffer of at
        // least 256 bytes.

    std::string ReadString();
        // Stream string (uint8 length, then the bytes; retail 0x0049ce00).
    void ReadBytes(void *dst, int32_t count) { Read(dst, count); }

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
    [[nodiscard]] int32_t Remaining() const
      { return buflen - (int32_t)(ptr - buf); }
        // Bytes left to read
    [[nodiscard]] bool Overrun() const { return overrun; }
        // A read ran past the end of the data
    bool SetPos(int32_t newpos)
      { if ((uint32_t)newpos <= (uint32_t)buflen) { ptr = buf + newpos; return true; } else return false; }
        // Sets read position (the end of the data is a valid position)
    bool MovePos(int32_t newpos)
      { return SetPos(GetPos() + newpos); }

  private:
    void Read(void *dst, int32_t count)
    {
        if (count <= Remaining())
        {
            std::memcpy(dst, ptr, count);
            ptr += count;
        }
        else
        {
            std::memset(dst, 0, count);
            ptr = buf + buflen;
            overrun = true;
        }
    }

    uint8_t *buf     = nullptr;
    uint8_t *ptr     = nullptr;
    int32_t  buflen  = 0;
    bool     overrun = false;
};

// The TOutputStream object represents an OUTPUT buffer a sector or other
// streamed data is written to. It grows as needed.

_CLASSDEF(TOutputStream)
class TOutputStream
{
  public:
    TOutputStream(int32_t startsize, int32_t growsize);
    virtual ~TOutputStream();
    TOutputStream(const TOutputStream&) = delete;
    TOutputStream& operator=(const TOutputStream&) = delete;

    TOutputStream& operator << (int32_t d)  { Write(&d, sizeof(d)); return *this; }
    TOutputStream& operator << (uint32_t d) { Write(&d, sizeof(d)); return *this; }
    TOutputStream& operator << (short d)    { Write(&d, sizeof(d)); return *this; }
    TOutputStream& operator << (uint16_t d) { Write(&d, sizeof(d)); return *this; }
    TOutputStream& operator << (char d)     { Write(&d, sizeof(d)); return *this; }
    TOutputStream& operator << (uint8_t d)  { Write(&d, sizeof(d)); return *this; }
    TOutputStream& operator << (const char *d);
        // Stream string: uint8 length (at most 255), then the bytes
        // (retail 0x0049ccc0).

    void WriteBytes(const void *src, int32_t count) { Write(src, count); }

    void *Buffer() { return buf; }
        // Returns pointer to buf
    int32_t BufferSize() { return bufsize; }
        // Returns size of buffer
    int32_t DataSize() { return (int32_t)(ptr - buf); }
        // Returns size of data (the write position)
    void MakeFreeSpace(int32_t freespace);
        // Makes sure there is at least this amount of free space at end of buffer
    void Reset()
      { ptr = buf; }
        // Resets write positon
    int32_t GetPos()
      { return (int32_t)(ptr - buf); }
        // Gets write position
    bool SetPos(int32_t newpos)
      { if ((uint32_t)newpos <= (uint32_t)bufsize) { ptr = buf + newpos; return true; } else return false; }
        // Sets write position (if not past end of buffer)
    bool MovePos(int32_t newpos)
      { return SetPos(GetPos() + newpos); }

  private:
    void Write(const void *src, int32_t count)
    {
        MakeFreeSpace(count);
        std::memcpy(ptr, src, count);
        ptr += count;
    }

    uint8_t *buf      = nullptr;
    uint8_t *ptr      = nullptr;
    int32_t  bufsize  = 0;
    int32_t  growsize = 0;
};
