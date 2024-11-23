// *********************************************************************
// *             C I N E M A T I X   G A M E   S Y S T E M             *
// *                          Resource Compiler                        *
// *                             EXILERC.EXE                           *
// *              Copyright (C) 1993 by Cinematix Studios              *
// *********************************************************************

#pramga once

#include "revenant.h"

#define RESVERSION  1   // Increment this when the resource file changes

#define COMP_NONE   0   // No Compression
#define COMP_ZIP    1   // ZIP implode compression

#define RESMAGIC ('C' | ('G' << 8) | ('S' << 16) | ('R' << 24))

struct FileResHdr
{
    uint32_t resmagic;
    uint16_t topbm;
    uint8_t  comptype;
    uint8_t  version;
    uint32_t datasize;
    uint32_t objsize;
    uint32_t hdrsize;
};

