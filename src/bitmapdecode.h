// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 bitmapdecode.h - Bitmap decode helpers                *
// *************************************************************************

#pragma once

#include "revenant.h"

bool DecodeBitmapToRGBA(PTBitmap bm, uint8_t* dst, int32_t dst_pitch,
                        int32_t ox, int32_t oy);
