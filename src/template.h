// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               template.h - Terrain template routines                  *
// *************************************************************************

#pragma once

#include "revenant.h"

#define MAXTERRAINNAMES     32
#define MAXOBJECTREFS       64
#define MAXTEMPLATES        256

#define TEMPLATEFILENAME    "template"

#define MAXDENSITY          16

_STRUCTDEF(SObjectRef)
struct SObjectRef
{
    char name[RESNAMELEN];
    S3DPoint offset;
};

_STRUCTDEF(STemplate)
struct STemplate
{
    char names[MAXTERRAINNAMES][RESNAMELEN];
    int32_t numnames;
    SObjectRef objects[MAXOBJECTREFS];
    int32_t numobjects;
};

// Since the terrain template object uses lots of big arrays, it's best to
// only allocate it when you need it and then delete it when you're done, instead of
// just making it a global that always hangs around.
// Creating always loads the template file, and deleting always saves it.

_CLASSDEF(TTerrainTemplate)
class TTerrainTemplate
{
  public:
    TTerrainTemplate();
    ~TTerrainTemplate();

    bool Load();
    bool Save();

    int32_t NewTemplate(char *tilename);
        // Returns the index of the template
    int32_t AddObject(int32_t index, TObjectInstance* inst, S3DPoint offset);
        // Add inst at offset to the indexed template

    void ApplyTemplate(TObjectInstance* inst, TSector* sect, int32_t density);
        // Choose a template for the given object and add the template objects to the sector

  private:
    STemplate templates[MAXTEMPLATES];
    int32_t numtemplates;
};
