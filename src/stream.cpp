// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     stream.cpp - TStream object                       *
// *************************************************************************

#include "stream.h"

#include <algorithm>
#include <cstdlib>

// ******************************
// * Constructor and Destructor *
// ******************************

TOutputStream::TOutputStream(int32_t startsize, int32_t grwsize)
{
    bufsize  = std::max<int32_t>(startsize, 16);
    growsize = std::max<int32_t>(grwsize, 16);
    buf = (uint8_t *)std::malloc(bufsize);
    Reset();
}

TOutputStream::~TOutputStream()
{
    std::free(buf);
}

void TOutputStream::MakeFreeSpace(int32_t freespace)
{
    const int32_t datasize = DataSize();
    if (bufsize - datasize >= freespace)
        return;

    int32_t newsize = bufsize;
    while (newsize - datasize < freespace)
        newsize += growsize;
    buf = (uint8_t *)std::realloc(buf, newsize);
    ptr = buf + datasize;
    bufsize = newsize;
}

// ****************
// * IO Functions *
// ****************

TInputStream& TInputStream::operator >> (char *d)
{
    uint8_t len = 0;
    *this >> len;
    Read(d, len);
    d[len] = 0;
    return *this;
}

std::string TInputStream::ReadString()
{
    uint8_t len = 0;
    *this >> len;
    std::string s(len, '\0');
    Read(s.data(), len);
    return s;
}

TOutputStream& TOutputStream::operator << (const char *d)
{
    const uint8_t len = (uint8_t)std::min<size_t>(std::strlen(d), 255);
    *this << len;
    Write(d, len);
    return *this;
}
