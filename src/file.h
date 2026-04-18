// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       file.h - Common File IO                         *
// *************************************************************************

#pragma once

#include "revenant.h"

//==============================================================================
//                           File.CPP Function Prototypes
//==============================================================================

void *LoadFile(const char *filepath, void *filebuf = nullptr, int32_t *filesize = nullptr);
bool FileExists(const char *filepath);
int32_t FileSize(const char *filepath);
FILE *TryOpen(const char *name, const char *mode);
bool TryDelete(const char *name);
bool TryRename(const char *oldname, const char *newname);
