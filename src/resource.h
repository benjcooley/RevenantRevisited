// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     resource.h - Bitmap resource                      *
// *************************************************************************

#pragma once

#include "revenant.h"

void *LoadResource(const char *name, int32_t id = -1, uint32_t *ressize = nullptr);
void *LoadResourceHeader(const char *name, int32_t id = -1, uint32_t *ressize = nullptr);
void *LoadResourceBinary(const char *name, int32_t id = -1, uint32_t *ressize = nullptr);
bool SaveResource(const char *name, void *ptr, int32_t id = -1);
bool SaveResourceHeader(const char *name, void *header, int32_t hdrsize, int32_t id = -1);
