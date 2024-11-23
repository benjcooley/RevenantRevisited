// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    lightdef.h - Light Definition                      *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "graphics.h"

// Light flags
#define LIGHT_DIR       (1 << 0)                // Directional light
#define LIGHT_SUN       (1 << 1)                // Sunlight
#define LIGHT_MOON      (1 << 2)                // Moonlight

_STRUCTDEF(SLightDef)
struct SLightDef
{
    uint8_t flags;         // LIGHT_x
    short multiplier;   // Multiplier
    S3DPoint pos;       // Position of light
    SColor color;       // RGB Color of light 
    uint8_t intensity;     // Intensity of light
    int32_t lightindex;     // Light index for 3d system
    int32_t lightid;        // Light id for dls system
};

