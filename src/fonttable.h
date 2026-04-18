// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1999 Cinematix                       *
// *                 fonttable.h - FONT.DEF font registry                  *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "font.h"

// **************************************************************
// * TGenericFont - One named font entry from FONT.DEF         *
// **************************************************************
//
// A generic font bundles together the bitmap font atom used for normal
// drawing and optional shadow/glow/select variants, plus color and
// metric metadata declared in FONT.DEF.  WINFONT entries use the same
// structure but leave the bitmap atom pointers null until the TrueType
// backend is plugged in.

#define FONT_BMFONT     1
#define FONT_WINFONT    2

_CLASSDEF(TGenericFont)
class TGenericFont
{
  public:
    TGenericFont();

    int32_t type;                       // FONT_BMFONT or FONT_WINFONT

    char name[32];                      // Logical name in FONT.DEF ("System", "Gold", ...)

  // Bitmap atoms (shared across generic fonts via TFontTable's atom cache)
    TFont* primary;                     // Normal face - nullptr for pure WINFONT
    TFont* shadow;                      // Shadow variant or nullptr
    TFont* glow;                        // Glow/highlight variant or nullptr
    TFont* select;                      // Selected variant or nullptr

  // Metrics and style
    SColor  color;                      // Default text color
    SColor  gcolor;                     // Glow color
    SColor  scolor;                     // Select color
    int32_t lextra;                     // Extra spacing between word-wrapped lines
    int32_t cextra;                     // Extra spacing between characters
    uint32_t drawmode;                  // Default draw mode flags

  // WINFONT descriptor (unused by BMFONT entries)
    char    facename[32];               // Windows typeface name
    int32_t height;                     // Font pixel height
    bool    bold;
    bool    italic;
};

// **************************************************************
// * TFontTable - Loads FONT.DEF and holds named fonts         *
// **************************************************************

_CLASSDEF(TFontTable)
class TFontTable
{
  public:
    TFontTable();
    ~TFontTable();

    bool Initialize();
      // Parses FONT.DEF from the resources archive.  Returns false on I/O
      // failure; parse errors raise FatalError via TToken::Error.
    void Close();
      // Frees all generic fonts and cached bitmap atoms.

    TGenericFont* Font(const char* name);
      // Look up a generic font by logical name.  FatalErrors if not found.
    TGenericFont* FindFont(const char* name);
      // Like Font() but returns nullptr if not found.

    TFont* Bitmap(const char* name);
      // Convenience: returns Font(name)->primary, the normal bitmap face.

  private:
    TFont* LoadAtom(const char* resname);
      // Loads a bitmap font resource by name (SYSFONT.DAT, SCRLFONT.DAT, ...)
      // and caches it so repeated references share one copy.

    void ParseBMFont(TToken& t);
    void ParseWINFont(TToken& t);

    struct SAtomEntry
    {
        char name[32];
        TFont* font;
        SAtomEntry() : font(nullptr) { name[0] = 0; }
        ~SAtomEntry() { if (font) free(font); }
    };

    TPointerArray<TGenericFont, 16, 16> fonts;
    TPointerArray<SAtomEntry, 16, 16>   atoms;
};

// Global font table - created during game startup, used for all
// font lookups previously keyed off of FONT.NNN numeric ids.

extern TFontTable* FontTable;
