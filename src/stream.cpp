// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     stream.cpp - TStream object                       *
// *************************************************************************

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "revenant.h"
#include "stream.h"
#pragma hdrstop

// ******************************
// * Constructor and Destructor *
// ******************************

TOutputStream::TOutputStream(int32_t startsize, int32_t grwsize)
{
    buf = (uint8_t *)malloc(startsize);
    bufsize = startsize;
    growsize = grwsize;
    Reset();
}

TOutputStream::~TOutputStream()
{
    delete buf;
}

void TOutputStream::MakeFreeSpace(int32_t freespace)
{
    int32_t datasize = DataSize();
    if (bufsize - datasize < freespace)
    {
        buf = (uint8_t *)realloc(buf, bufsize + growsize);
        ptr = buf + datasize;
        bufsize = bufsize + growsize;
    }
}

// ****************
// * IO Functions *
// ****************

RTInputStream TInputStream::operator >> (char *d)
{
    strncpy(d, (char *)(ptr + 1), *ptr);
    d[*ptr] = nullptr;
    ptr += *ptr + 1;
    return *this;
}

RTOutputStream TOutputStream::operator << (char *d)
{
    *ptr = strlen(d);
    strncpy((char *)(ptr + 1), d, *ptr);
    ptr += *ptr + 1;
    return *this;
}
