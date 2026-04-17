// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1999 Cinematix                       *
// *                fonttable.cpp - FONT.DEF font registry                 *
// *************************************************************************

#include "fonttable.h"

#include "parse.h"
#include "resource.h"
#include "revutils.h"

#include <stdio.h>
#include <string.h>

TFontTable* FontTable = nullptr;

// ***************
// * TGenericFont *
// ***************

TGenericFont::TGenericFont()
{
    type = FONT_BMFONT;
    name[0] = 0;
    primary = nullptr;
    shadow = glow = select = nullptr;
    color  = SColor{ 255, 255, 255 };
    gcolor = SColor{ 255, 255, 255 };
    scolor = SColor{ 255, 255, 255 };
    lextra = 0;
    cextra = 0;
    drawmode = DM_TRANSPARENT | DM_ALIAS;
    facename[0] = 0;
    height = 0;
    bold = false;
    italic = false;
}

// ***************
// * TFontTable  *
// ***************

TFontTable::TFontTable()
{
}

TFontTable::~TFontTable()
{
    Close();
}

void TFontTable::Close()
{
    // DeleteAll() already calls delete on each entry — don't pre-delete
    // or we double-free.
    fonts.DeleteAll();
    atoms.DeleteAll();
}

TGenericFont* TFontTable::FindFont(const char* nm)
{
    for (int32_t i = 0; i < fonts.NumItems(); i++)
    {
        TGenericFont* f = fonts[i];
        if (f && !stricmp(f->name, nm))
            return f;
    }
    return nullptr;
}

TGenericFont* TFontTable::Font(const char* nm)
{
    TGenericFont* f = FindFont(nm);
    if (!f)
    {
        char buf[128];
        sprintf(buf, "Font '%s' not found in FONT.DEF", nm);
        FatalError(buf);
    }
    return f;
}

TFont* TFontTable::Bitmap(const char* nm)
{
    TGenericFont* f = Font(nm);
    return f ? f->primary : nullptr;
}

TFont* TFontTable::LoadAtom(const char* resname)
{
    if (!resname || !*resname)
        return nullptr;

    for (int32_t i = 0; i < atoms.NumItems(); i++)
    {
        SAtomEntry* a = atoms[i];
        if (a && !stricmp(a->name, resname))
            return a->font;
    }

    TFont* f = (TFont*)LoadResource(resname);
    if (!f)
        return nullptr;

    SAtomEntry* entry = new SAtomEntry;
    strncpyz(entry->name, resname, sizeof(entry->name));
    entry->font = f;
    atoms.Add(entry);

    return f;
}

// ***************
// * Parser      *
// ***************

// BMFONT "name" <NORMAL "n"> <SHADOW "s"> <COLOR r g b>
//               <GLOW "g"> <GCOLOR r g b> <SELECT "s"> <SCOLOR r g b>
//               <LEXTRA n> <CEXTRA n> <DRAWMODE n>

void TFontTable::ParseBMFont(TToken& t)
{
    char nm[32]     = { 0 };
    char normal[32] = { 0 };
    char shadow[32] = { 0 };
    char glow[32]   = { 0 };
    char select[32] = { 0 };
    int32_t cr = 255, cg = 255, cb = 255;
    int32_t gr = 255, gg = 255, gb = 255;
    int32_t sr = 255, sg = 255, sb = 255;
    int32_t lextra   = 0;
    int32_t cextra   = 0;
    int32_t drawmode = (int32_t)(DM_TRANSPARENT | DM_ALIAS);

    if (!Parse(t,
        "%31s <NORMAL %31s> <SHADOW %31s> <COLOR %i %i %i> "
        "<GLOW %31s> <GCOLOR %i %i %i> <SELECT %31s> <SCOLOR %i %i %i> "
        "<LEXTRA %i> <CEXTRA %i> <DRAWMODE %i>",
        nm, normal, shadow,
        &cr, &cg, &cb,
        glow, &gr, &gg, &gb,
        select, &sr, &sg, &sb,
        &lextra, &cextra, &drawmode))
    {
        t.Error("Unable to parse BMFONT tag in FONT.DEF");
    }

    TGenericFont* f = new TGenericFont;
    f->type = FONT_BMFONT;
    strncpyz(f->name, nm, sizeof(f->name));
    f->primary = LoadAtom(normal);
    f->shadow  = LoadAtom(shadow);
    f->glow    = LoadAtom(glow);
    f->select  = LoadAtom(select);
    f->color   = SColor{ (uint8_t)cr, (uint8_t)cg, (uint8_t)cb };
    f->gcolor  = SColor{ (uint8_t)gr, (uint8_t)gg, (uint8_t)gb };
    f->scolor  = SColor{ (uint8_t)sr, (uint8_t)sg, (uint8_t)sb };
    f->lextra  = lextra;
    f->cextra  = cextra;
    f->drawmode = (uint32_t)drawmode;

    fonts.Add(f);
}

// WINFONT "name" FONT "face" size <BOLD> <ITALICS>
//                <COLOR r g b> <GCOLOR r g b> <SCOLOR r g b>
//                <LEXTRA n> <CEXTRA n>

void TFontTable::ParseWINFont(TToken& t)
{
    char nm[32]   = { 0 };
    char face[32] = { 0 };
    int32_t size  = 0;
    bool bold = false, italic = false;
    int32_t cr = 255, cg = 255, cb = 255;
    int32_t gr = 255, gg = 255, gb = 255;
    int32_t sr = 255, sg = 255, sb = 255;
    int32_t lextra = 0;
    int32_t cextra = 0;

    if (!Parse(t,
        "%31s FONT %31s %i <BOLD> <ITALICS> "
        "<COLOR %i %i %i> <GCOLOR %i %i %i> <SCOLOR %i %i %i> "
        "<LEXTRA %i> <CEXTRA %i>",
        nm, face, &size, &bold, &italic,
        &cr, &cg, &cb,
        &gr, &gg, &gb,
        &sr, &sg, &sb,
        &lextra, &cextra))
    {
        t.Error("Unable to parse WINFONT tag in FONT.DEF");
    }

    TGenericFont* f = new TGenericFont;
    f->type = FONT_WINFONT;
    strncpyz(f->name, nm, sizeof(f->name));
    strncpyz(f->facename, face, sizeof(f->facename));
    f->height = size;
    f->bold   = bold;
    f->italic = italic;
    f->color  = SColor{ (uint8_t)cr, (uint8_t)cg, (uint8_t)cb };
    f->gcolor = SColor{ (uint8_t)gr, (uint8_t)gg, (uint8_t)gb };
    f->scolor = SColor{ (uint8_t)sr, (uint8_t)sg, (uint8_t)sb };
    f->lextra = lextra;
    f->cextra = cextra;

  // TrueType backend is not hooked up yet - pick the closest bitmap
  // font so WINFONT-based draw calls still produce legible text.

    f->primary = LoadAtom(size <= 14 ? "sysfont" : "scrlfont");

    fonts.Add(f);
}

bool TFontTable::Initialize()
{
    Close();

    char fname[MAXPATHLEN];
    sprintf(fname, "%sfont.def", ResourcePath);

    FILE* fp = rev_fopen(fname, "rb");
    if (!fp)
        return false;

    TFileParseStream s(fp, fname);
    TToken t(s);

    if (!t.DefineGet())
        t.Error("Syntax error in header");

    while (t.Type() != TKN_EOF)
    {
        if (t.Type() != TKN_IDENT)
            t.Error("BMFONT or WINFONT expected");

        if (!stricmp(t.Text(), "BMFONT"))
        {
            t.WhiteGet();
            ParseBMFont(t);
        }
        else if (!stricmp(t.Text(), "WINFONT"))
        {
            t.WhiteGet();
            ParseWINFont(t);
        }
        else
        {
            t.Error("Bad tag in FONT.DEF");
        }

        t.DefineGet();
    }

    fclose(fp);

    return true;
}
