// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   revengine.h - the engine's umbrella: base types, defines, logging    *
// *************************************************************************
//
// Everything under src/engine includes this, never game/app/revenant.h.
// The game's umbrella includes this one and adds the game's globals.
// No file under src/engine includes a file under src/game
// (tools/test_layering.py enforces it).

#pragma once

#include "engine/core/platform_compat.h"
#include "engine/core/revdefs.h"
#include "engine/core/revtypes.h"
#include "engine/core/logging.h"
#include "engine/core/file.h"
#include "engine/core/revtime.h"

#include <HandmadeMath.h>

#include <cinttypes>
#include <cstddef>
#include <cstdio>
#include <cstdlib>

#define _strdup(s) strcpy((char*)malloc(strlen(s) + 1), s)
