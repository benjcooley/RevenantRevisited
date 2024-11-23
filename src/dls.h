// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                dls.h - Dynamic Lighting System Module                 *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "bitmap.h"

#define NUMBASELIGHTS   4

extern RTBitmap UnlitBuffer;
extern RTBitmap NormalBuffer;
extern RTBitmap ZBuffer;

void SetAmbientLight(int32_t ambient);
    // Sets the ambient lighting value and regenerates color tables

int32_t GetAmbientLight();
    // Returns the current ambient lighting value

void SetAmbientColor(RSColor color);
    // Sets ambient color and regenerates color light tables

void GetAmbientColor(RSColor color);
    // Returns ambient color (real ambient is combination of ambient color * ambient value)

int32_t NewLightIndex(RSColor color, int32_t mult = -1);
    // Finds or creates the light index to the baselight which is the closest match for color

void FreeLightIndex(int32_t id);
    // Frees up use of a given index

void ClearLights();
    // Clear out all light usage

void SetLightColor(int32_t id, RSColor color, int32_t mult = -1);
    // Sets light color for dls color id 0-3

void GetLightColor(int32_t id, RSColor color, int32_t &mult);
    // Gets light color for dls color id 0-3

void SetMonoPercent(int32_t pcnt);
    // Sets the 16to32 bit tables, and also implements color desaturation (monopercent)
    // Monochrome drawing allows colored lights to look more dramatic (i.e.
    // green grass will reflect blue light when mono is turned up), but it also
    // literally grays out the scene.

int32_t GetMonoPercent();
    // returns the color/monochrome percentage for the 16to32 bit tables (tile drawing)

//void DrawLight(RS3DPoint pos, RSColor color, int32_t intensity, PTSurface surface);
    // Draw a dynamic light to the screen!!

void DrawLightNoNormals(RS3DPoint pos, RSColor color, int32_t intensity, PTSurface surface);
    // Draw a dynamic light without looking at surface normals

//void DrawStaticLight(RS3DPoint pos, RSColor color, int32_t intensity, PTSurface surface, int32_t id);
    // Draws a static light (matches color val in 'color' to one of four lighting colors)

//void DrawStaticMonoLight(RS3DPoint pos, int32_t intensity, PTSurface surface);
    // Draws a monochrome (color 0) static light (faster than DrawStaticLight)

//void DrawStaticDirLight(RS3DPoint pos, RSColor color, int32_t intensity, PTSurface surface, int32_t id);
    // Draws a static directional light

//void DrawStaticDirMonoLight(RS3DPoint pos, int32_t intensity, PTSurface surface);
    // Draws a monochrome static directional light (color 0) FASTER

void DrawStaticLightNoNormals(RS3DPoint pos, RSColor color, int32_t intensity, PTSurface surface, int32_t id);
    // Draws a static no normal light (matches color val in 'color' to one of four lighting colors)

void DrawStaticMonoLightNoNormals(RS3DPoint pos, int32_t intensity, PTSurface surface);
    // Draws a static monochrome (color 0) static light without normal values (faster!)

void DrawAmbientLight(PTSurface surface, RSRect r);
    // Draws static ambient light

//void DrawAmbientLightNoNormals(PTSurface surface, RSRect r);
    // Draws static ambient light without normals

void TransferAndLight32to16(PTSurface dest, PTSurface source, RSRect r);
    // Transfer 32 bit color/intensity to 16 bit screen

void  MakeColorTables();
    // Sets up the DLS color table entries

