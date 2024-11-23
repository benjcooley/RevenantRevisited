// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   graphics.h - Graphics Routines                      *
// *************************************************************************

#pragma once

#include "revenant.h"

// ************************
// * Draw Block Structure *
// ************************

// Draw Block Structure is passed to each routine with the appropriate
// variables filled in.

_STRUCTDEF(SDrawBlock)
_STRUCTDEF(SDrawParam)

typedef bool (*DRAWFUNCTION)(PSDrawBlock, PSDrawParam);
  // Defines a standard draw function type
typedef void (*DRAWCALLBACK)(PSDrawBlock db, PSDrawParam dp);
  // Defines a callback function which is called for each draw rectangle

struct SDrawBlock
{
    uint32_t srcbitmapflags;            // Bitmap Flags
    uint32_t dstbitmapflags;            // Bitmap Flags
    void  *dest;                        // Pointer to Dest, Dest Z, and Dest Normal buffers
    uint16_t  *dzbuffer;
    uint16_t  *dnormals;
    int32_t   dbufwidth;                // Dest buffer width
    int32_t   dbufheight;               // Dest buffer height
    int32_t   dstride;                  // Dest stride
    int32_t   dzstride;                 // Dest zbuffer stride
//  int32_t   dnstride;                 // Dest normal stride
    void  *source;                      // Pointer to Src, Src Z, and Src Normal buffers
    uint16_t  *szbuffer;
    uint16_t  *snormals;
    int32_t   sbufwidth;                // Src buffer width
    int32_t   sbufheight;               // Src buffer height
    int32_t   sstride;                  // Src stride
    int32_t   szstride;                 // Src zbuffer stride
//  int32_t   snstride;                 // Src normal stride

    uint16_t  *palette;                 // Palette
    uint8_t  *alpha;                    // Alpha buffer pointer
    uint8_t  *alias;                    // Alpha buffer pointer

    uint32_t keycolor;                  // Color to use as transparent
    
    //bool operator= (PSDrawBlock drawblock)
    //    { memcpy(drawblock, this, sizeof(this)); return true; };
      // Redefines '=' to allow assigning strings.
};

struct SDrawParam
{
    uint32_t drawmode;                  // Drawing mode. See Exiledef.h for description
    DRAWFUNCTION func;                  // Drawing function

    DRAWCALLBACK callback;              // Drawing callback function (for adding dirty rectangles, etc.)    

    void *data;                         // Other data (such as text for draw tex function)

    int32_t   originx;                  // Upper left of drawing region
    int32_t   originy;

    int32_t   clipx;                    // Clipping rectangle 
    int32_t   clipy;
    int32_t   clipwidth;
    int32_t   clipheight;
                                    
    int32_t   dx;                       // Rectangle inside dest buffer to use
    int32_t   dy;                       
    int32_t   dwidth;   
    int32_t   dheight;  

    int32_t   sx;                       // Rectangle inside src buffer to use
    int32_t   sy;   
    int32_t   swidth;   
    int32_t   sheight;  
    
    uint32_t color;                    // color used to draw image
    uint32_t intensity;                // intensity of translucent/alpha draw

    uint16_t  zpos;                     // Z position of image
    uint16_t  normal;                   // Normal position for image

    //bool operator= (PSDrawParam drawparam)
    //    { memcpy(drawparam, this, sizeof(this)); return true; };
      // Redefines '=' to allow assigning strings.
};

_STRUCTDEF(SColor)

struct SColor
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// Data pointer points to this when drawing lines
_STRUCTDEF(SLineParam)
struct SLineParam
{
    int32_t x1, y1, x2, y2;
};

// Data pointer points to this when drawing text
_STRUCTDEF(STextParam)
struct STextParam
{
    int32_t startline;
    int32_t numlines;
    char *text;
    TFont *font;
    int32_t wrapwidth;
    int32_t justify;
    bool draw;
    int32_t length;
    bool noclip;
    int32_t linespace;
};
    
// ******************************
// * Low Level Graphic Routines *
// ******************************

// All routines return false if Dest. Buffer is Invalid.

bool Draw(PSDrawBlock db, PSDrawParam dp);
  // General drawing function. Clips and calls the 'func' function in the drawparam
  // structure, then calls the 'callback' function in the drawparam structure.  If
  // 'func' is nullptr, looks up the correct draw function by calling GetPutFunction().
 
DRAWFUNCTION GetPutFunction(PSDrawBlock db, PSDrawParam dp);
  // Returns the pointer to the appropriate put function given the current drawmode state

bool ClipRect(RSRect dst, RSRect src, RSRect result);
  // General purpose function to clip src rectangle in dest.
  // Returns false if rectangles don't overlap.  Src and result can be same rect

inline bool ClipRect(int32_t x, int32_t y, int32_t w, int32_t h, RSRect src, RSRect result)
  { return ClipRect(SRect(x, y, x + w - 1, y + h - 1), src, result); }
  // General purpose function to clip src rectangle with dest rect.
  // Returns false if rectangles don't overlap.  Src and result can be same rect

bool SubtractRect(RSRect dst, RSRect src, PSRect rects, int32_t &numrects);
  // Subtracts src rect from dest and returns results in 'rects'.  Returns
  // false if rects don't intersect.  Returns a maximum of 4 rects.
  // The Subtract rect function basically calculates an inverse of the rect intersection

inline bool SubtractRect(int32_t x, int32_t y, int32_t w, int32_t h, RSRect src, PSRect rects, int32_t &numrects)
  { return SubtractRect(SRect(x, y, x + w - 1, y + h - 1), src, rects, numrects); }
  // Subtracts src rect from dest and returns results in 'rects'.  Returns
  // false if rects don't intersect.  Returns a maximum of 4 rects.
  // The Subtract rect function basically calculates an inverse of the rect intersection

bool Clip(PSDrawBlock db, PSDrawParam dp, PSDrawParam dparray, 
    int32_t &numrects);
  // Clipping Routine..  Takes one draw rectangle, clips it, and optionally wraps clips it to 4 other rects.

bool StretchClip(PSDrawBlock db, PSDrawParam dp, PSDrawParam dparray, 
    int32_t &numrects);
  // Stretch Clipping Routine.

// Put Routines
bool Put(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine.
bool Put8(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine for 8 bit bitmaps. Draws to 32 bit surface
bool Put88(PSDrawBlock db, PSDrawParam dp); 
  // Draws 8 bit bitmap to 8 bit surface.
bool Put816(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine for 8 bit bitmaps. Draws to 16 bit surface.
bool Put32(PSDrawBlock db, PSDrawParam dp);
  // 32 bit Draw Routine.

// Shutter Put Routines
bool ShutterPut(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine.
bool ShutterPut8(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine for 8 bit bitmaps. Draws to 32 bit surface
bool ShutterPut816(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine for 8 bit bitmaps. Draws to 16 bit surface.
bool ShutterPut32(PSDrawBlock db, PSDrawParam dp);
  // 32 bit Draw Routine.

bool PutChunk8(PSDrawBlock db, PSDrawParam dp);
  // Draw Routine for 8 bit bitmaps. Draws to 32 bit surface. Handles Z and Normal Internally
bool TransPut(PSDrawBlock db, PSDrawParam dp);
  // Normal Transparent Draw Routine.
bool TransPut8(PSDrawBlock db, PSDrawParam dp);
  // Normal Transparent Draw Routine for 8 bit bitmap. Draws to 32 bit surface
bool TransPut816(PSDrawBlock db, PSDrawParam dp);   
  // Normal Transparent Draw Routine for 8 bit bitmap. Draws to 16 bit surface
bool TransPut88(PSDrawBlock db, PSDrawParam dp);    
  // Normal Transparent Draw Routine for 8 bit bitmap. Draws to 8 bit surface
bool TransPutKey(PSDrawBlock db, PSDrawParam dp);
  // Normal Transparent Draw Routine with nonzero keycolor.
bool TransPutColor(PSDrawBlock db, PSDrawParam dp);
  // Normal Transparent Draw Routine in given color.
bool PutHueChange(PSDrawBlock db, PSDrawParam dp);
  // Draw routine with hue change to the given color.
bool TransPutSVChange(PSDrawBlock db, PSDrawParam dp);
  // Transparent draw routine with saturation/brightness change to the given color.
bool TransPut32(PSDrawBlock db, PSDrawParam dp);
  // 32 bit Transparent Draw Routine.

bool Stretch(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine.
bool TransStretch(PSDrawBlock db, PSDrawParam dp);
  // Normal Transparent Draw Routine.

bool Mask(PSDrawBlock db, PSDrawParam dp);
  // Normal Masked Draw Routine.
bool MaskStretch(PSDrawBlock db, PSDrawParam dp);
  // Normal Masked Draw Routine.

bool Translucent(PSDrawBlock db, PSDrawParam dp);
  // Draws bitmap translucently to screen
bool TransTranslucent(PSDrawBlock db, PSDrawParam dp);
  // Draws bitmap transparently and translucently to screen
bool TranslucentStretch(PSDrawBlock db, PSDrawParam dp);
  // Draws bitmap translucently stretched to screen

bool AlphaLighten(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info. Lighten only.
bool Alpha(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info.
bool Alpha8(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info. Converts 8 bit bitmap to 16 bit.
bool AlphaZ(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info and ZBuffer.
bool AlphaZ8(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info and ZBuffer. Converts 8 bit bitmap to 16 bit
bool AlphaDim(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info, dimming the alpha by dp->color.
bool AlphaDimZ(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info, dimming the alpha by dp->color.
bool Alpha32(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info for 32 bit buffers.
bool AlphaDimZNoBitmap(PSDrawBlock db, PSDrawParam dp);
  // Draws an alpha dimming z image with no bitmap data (single color)

bool Alias(PSDrawBlock db, PSDrawParam dp);
  // Draws an aliased edge around graphics.
bool AliasColor(PSDrawBlock db, PSDrawParam dp);
  // Draws an aliased edge around graphics with set color.
bool Alias32(PSDrawBlock db, PSDrawParam dp);
  // Draws an aliased edge around graphics for 32 bit buffers.

bool DrawSelected8(PSDrawBlock db, PSDrawParam dp);
  // Draw a selection highlight around bitmap's edges for 8 bit bitmaps.
bool DrawSelected(PSDrawBlock db, PSDrawParam dp);
  // Draw a selection highlight around bitmap's edges.

// Z Buffer Put routines
bool ZPut(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using zbuffer to control drawing
bool ZPut8(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing Draws to 16 bit surface
bool ZPut816(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing. Draws to 16 bit surface.
bool ZPut32(PSDrawBlock db, PSDrawParam dp);
  // Draws to a 32 bit bitmap using zbuffer to control drawing

// Z Buffer Put Shutter routines
bool ShutterZPut(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using zbuffer to control drawing
bool ShutterZPut8(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing Draws to 16 bit surface
bool ShutterZPut816(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing. Draws to 16 bit surface.
bool ShutterZPut32(PSDrawBlock db, PSDrawParam dp);
  // Draws to a 32 bit bitmap using zbuffer to control drawing

// Z Buffer Normal Put routines
bool ZNormalPut(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using zbuffer to control drawing, also transfer normal info.
bool ZNormalPut8(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing, also transfer normal info.
  // Draws to 32 bit surface
bool ZNormalPut816(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing, also transfer normal info.
  // Draws to 16 bit surface
bool ZNormalPut32(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using zbuffer to control drawing, also transfer normal info.

// Z Buffer Normal Put Shutter routines
bool ShutterZNormalPut(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using zbuffer to control drawing, also transfer normal info.
bool ShutterZNormalPut8(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing, also transfer normal info.
  // Draws to 32 bit surface
bool ShutterZNormalPut816(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing, also transfer normal info.
  // Draws to 16 bit surface
bool ShutterZNormalPut32(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using zbuffer to control drawing, also transfer normal info.

bool TransZStaticPut8(PSDrawBlock db, PSDrawParam dp);
  // Draw an 8-bit bitmap to a 32-bit surface at a static z position, also drawing zbuffer info
bool TransZStaticPut816(PSDrawBlock db, PSDrawParam dp);
  // Draw an 8-bit bitmap to a 16-bit surfaceat a static z position, also drawing zbuffer info

bool ZFind(PSDrawBlock db, PSDrawParam dp);
  // Return the first uncliped zbuffer point if it is highest in zbuf

bool ZStretch(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine.
bool ZMaskStretch(PSDrawBlock db, PSDrawParam dp);
  // Normal Masked Draw Routine.

bool Box(PSDrawBlock db, PSDrawParam dp);
  // Fill the area specified by db and dp

bool Convert15to16(PTBitmapData bitmap);
  // Convert 15/24 bit buffer to 16 bit buffer.
bool ConvertPal15to16(PTBitmapData bitmap);
  // Convert a 15 bit palette to 16 bit

bool Convert16to15(PTBitmapData bitmap);
  // Convert 16/24 bit buffer to 15 bit buffer.
bool ConvertPal16to15(PTBitmapData bitmap);
  // Convert a 16 bit palette to 15 bit

bool TextDraw(PSDrawBlock db, PSDrawParam dp);
  // Write text in the given font, transparent and aliased, with wordwrapping if desired.
  // Returns number of lines of text generated in 'length' data parameter.  See SLineParam
  // structure for params to set in the data portion of 'dp'

bool LineDraw(PSDrawBlock db, PSDrawParam dp);
  // Draws line in either 15/16 or 24 bit color.
  // db->dstbitmapflags determine which to draw in.  See SLineParam stucture for
  // parameter data to use for dp->data.

uint16_t ZFindChunk(PSDrawBlock db, PSDrawParam dp);
  // Return the first uncliped zbuffer point in a chunked bitmap

inline void SetupZDraw(int32_t *bmwidth, int32_t *bmheight, int32_t *srcoff, int32_t *srcadd, int32_t *srczoff, int32_t *srczadd,
    int32_t *dstoff, int32_t *dstadd, int32_t *dstzoff, int32_t *dstzadd, PSDrawBlock db, PSDrawParam dp)
{
    int32_t srcbytes = 2;
    if (db->srcbitmapflags & BM_8BIT) srcbytes = 1;
    else if (db->srcbitmapflags & (BM_15BIT | BM_16BIT)) srcbytes = 2;
    else if (db->srcbitmapflags & BM_24BIT) srcbytes = 3;
    else if (db->srcbitmapflags & BM_32BIT) srcbytes = 4;

    int32_t dstbytes = 2;
    if (db->dstbitmapflags & BM_8BIT) dstbytes = 1;
    else if (db->dstbitmapflags & (BM_15BIT | BM_16BIT)) dstbytes = 2;
    else if (db->dstbitmapflags & BM_24BIT) dstbytes = 3;
    else if (db->dstbitmapflags & BM_32BIT) dstbytes = 4;

    // Get offsets/add stuff
    *dstoff  = (dp->dy + dp->originy) * db->dstride + dp->dx + dp->originx;
    *dstadd  = db->dstride - dp->dwidth;
        
    *dstzoff  = (dp->dy + dp->originy) * db->dzstride + dp->dx + dp->originx;
    *dstzadd  = db->dzstride - dp->dwidth;

    if (dp->drawmode & DM_REVERSEVERT)
    {
        *srcoff = (dp->sy + dp->sheight - 1) * db->sstride + dp->sx;
        *srcadd = -db->sstride + dp->swidth;
        *srczoff = (dp->sy + dp->sheight - 1) * db->szstride + dp->sx;
        *srczadd = -db->szstride + dp->swidth;
    }
    else
    {
        *srcoff = dp->sy * db->sstride + dp->sx;
        *srcadd = db->sstride - dp->swidth;
        *srczoff = dp->sy * db->szstride + dp->sx;
        *srczadd = db->szstride - dp->swidth;
    }
    
    *bmwidth  = dp->swidth * srcbytes;
    *bmheight = dp->sheight;

    *dstoff = *dstoff * dstbytes;
    *dstadd = *dstadd * dstbytes;
    *dstzoff = *dstzoff * 2;
    *dstzadd = *dstzadd * 2;

    *srcoff = *srcoff * srcbytes;
    *srcadd = *srcadd * srcbytes;
    *srczoff = *srczoff * 2;
    *srczadd = *srczadd * 2;
}

inline void SetupDraw(int32_t *bmwidth, int32_t *bmheight, int32_t *srcoff, int32_t *srcadd,
    int32_t *dstoff, int32_t *dstadd, PSDrawBlock db, PSDrawParam dp)
{
    int32_t srcbytes = 2;
    if (db->srcbitmapflags & BM_8BIT) srcbytes = 1;
    else if (db->srcbitmapflags & (BM_15BIT | BM_16BIT)) srcbytes = 2;
    else if (db->srcbitmapflags & BM_24BIT) srcbytes = 3;
    else if (db->srcbitmapflags & BM_32BIT) srcbytes = 4;

    int32_t dstbytes = 2;
    if (db->dstbitmapflags & BM_8BIT) dstbytes = 1;
    else if (db->dstbitmapflags & (BM_15BIT | BM_16BIT)) dstbytes = 2;
    else if (db->dstbitmapflags & BM_24BIT) dstbytes = 3;
    else if (db->dstbitmapflags & BM_32BIT) dstbytes = 4;

    // Get offsets/add stuff
    *dstoff  = (dp->dy + dp->originy) * db->dstride + dp->dx + dp->originx;
    *dstadd  = db->dstride - dp->dwidth;
        
    if (dp->drawmode & DM_REVERSEVERT)
    {
        *srcoff = (dp->sy + dp->sheight - 1) * db->sstride + dp->sx;
        *srcadd = -db->sstride + dp->swidth;
    }
    else
    {
        *srcoff = dp->sy * db->sstride + dp->sx;
        *srcadd = db->sstride - dp->swidth;
    }
    
    *bmwidth  = dp->swidth * srcbytes;
    *bmheight = dp->sheight;

    *dstoff = *dstoff * dstbytes;
    *dstadd = *dstadd * dstbytes;

    *srcoff = *srcoff * srcbytes;
    *srcadd = *srcadd * srcbytes;
}

#define SETUP_DRAW\
    int32_t bmheight, bmwidth, srcoff, srcadd, dstoff, dstadd;\
    SetupDraw(&bmwidth, &bmheight, &srcoff, &srcadd, &dstoff, &dstadd, db, dp);

#define SETUP_Z_DRAW\
    int32_t bmheight, bmwidth, srcoff, srcadd, srczoff, srczadd, dstoff, dstadd, dstzoff, dstzadd;\
    SetupZDraw(&bmwidth, &bmheight, &srcoff, &srcadd, &srczoff, &srczadd, &dstoff, &dstadd, &dstzoff, &dstzadd, db, dp);

// Fills out default draw param struct  
inline void MakeDP(SDrawParam &dp, 
    int32_t x, int32_t y, int32_t sx, int32_t sy, int32_t swidth, int32_t sheight, uint32_t drawmode)
{
    dp.drawmode  = drawmode;
    dp.func      = nullptr;
    dp.callback  = nullptr;
    dp.data      = nullptr;
    dp.dx        = x;
    dp.dy        = y;
    dp.dwidth    = swidth;
    dp.dheight   = sheight;
    dp.sx        = sx;
    dp.sy        = sy;
    dp.swidth    = swidth;
    dp.sheight   = sheight;
    dp.zpos      = dp.normal = 0;
    dp.clipx     = dp.clipy = dp.clipwidth = dp.clipheight = dp.originx = dp.originy = 0;
    dp.intensity = 31; dp.color = 0;
}

inline void MakeDPNoSrc(SDrawParam &dp, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t drawmode)
{
    MakeDP(dp, x, y, 0, 0, 0, 0, drawmode);
    dp.dwidth = width;
    dp.dheight = height;
}
