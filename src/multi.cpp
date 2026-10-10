// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                resource.cpp - Resource object module                  *
// *************************************************************************

#include "multi.h"

#include <stdio.h>
#include <string.h>

void *TMulti::Find(const char *name)
{
    for (int32_t c = 0; c < numoffsets; c++)
    {
        const char *p = (const char *)((void *)names[c]);
        if (p && !stricmp(p, name))
            return (void *)offsets[c];
    }
    return nullptr;
}

void *TMulti::Object(const char *name)
{
    if (void *entry = Find(name))
        return entry;

    char buf[80];
    sprintf(buf, "Unable to find \'%s\' in multiresource", name);
    FatalError(buf);

    return nullptr;
}
