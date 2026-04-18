// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               graphics.cpp - Graphics objects module                  *
// *************************************************************************

#include <stdio.h>
#include <string.h>

#include "revenant.h"
#include "bitmap.h"
#include "chunkcache.h"
#include "decompdata.h"
#include "display.h"
#include "font.h"
#include "graphics.h"

extern uint8_t  ColorTable[32 * 256];
extern uint8_t  ColorTable16[32 * 256];
extern uint8_t  ColorTableUpperHi[32 * 256];
extern uint8_t  ColorTableUpperLo[32 * 256];
extern uint8_t  ColorTableLower[32 * 256];
extern uint32_t Conv16to32Upper[256];
extern uint32_t Conv16to32Lower[256];
extern uint8_t  IntensityTableUpper[256];
extern uint8_t  IntensityTableLower[256];

// ***************************************
// * Entry Function to graphics routines *
// ***************************************

bool Draw(PSDrawBlock db, PSDrawParam dp)
{
    int32_t numrects = 0;    
    SDrawParam  dparray[4];

    if (dp->drawmode & DM_NOCLIP)
    {
        dparray[0] = *dp;
        numrects = 1;
    }
    else
    {
        if (dp->drawmode & DM_STRETCH)
        {  
            if (!StretchClip(db, dp, dparray, numrects))
                return false;
        }
        else
        {
            if (!Clip(db, dp, dparray, numrects))
                return false;
        }
    }

    bool drawok = false;

    DRAWFUNCTION func = dp->func;
    if (!func)
        func = GetPutFunction(db, dp);
    if (!func)
        return false; // Unable to find an appropriate draw function

    for (int32_t loop = 0; loop < numrects; loop++)
    {
        PSDrawParam dpa = &dparray[loop];

        drawok |= func(db, dpa);

      // Add update rectangles (or whatever else a callback might be used for)
        if (dpa->callback && !(dpa->drawmode & DM_DOESCALLBACK))
            dpa->callback(db, dpa);
    }

    return drawok;
}

DRAWFUNCTION GetPutFunction(PSDrawBlock db, PSDrawParam dp)
{
    DRAWFUNCTION drawfunc = nullptr;

    if (db->srcbitmapflags & BM_CHUNKED)
        drawfunc = PutChunk8;
    
    else if (dp->drawmode & DM_FILL)
    {
        drawfunc = Box;
    }

    else if (dp->drawmode & DM_SELECTED)
    {
        if (db->srcbitmapflags & BM_8BIT)
            drawfunc = DrawSelected8;
    
        else
            drawfunc = DrawSelected;
    }
    
    else if ((dp->drawmode & DM_ZBUFFER) || (dp->drawmode & DM_NORMALS)) // ZBuffer or Normal Buffer draws
    {
        if (dp->drawmode & DM_STRETCH)
        {
            if (dp->drawmode & DM_ZMASK)
                drawfunc = ZMaskStretch;

            else
                drawfunc = ZStretch;
        }

        else if (dp->drawmode & DM_ALPHA)
        {
            if (db->srcbitmapflags & BM_NOBITMAP && db->dstbitmapflags & (BM_16BIT | BM_15BIT))
                drawfunc = AlphaDimZNoBitmap;
        }
        else
        {
            if (db->srcbitmapflags & BM_8BIT && db->dstbitmapflags & BM_32BIT)
            {
                if (dp->drawmode & DM_SHUTTER)  
                    drawfunc = ShutterZPut8;
                else
                    drawfunc = ZPut8;
            }

            else if (db->srcbitmapflags & BM_8BIT && db->dstbitmapflags & (BM_16BIT | BM_15BIT))
            {
                if (dp->drawmode & DM_SHUTTER)  
                    drawfunc = ShutterZPut816;
                else
                    drawfunc = ZPut816;
            }
            
            else if (db->dstbitmapflags & (BM_32BIT | BM_24BIT))
            {
                if (dp->drawmode & DM_SHUTTER)  
                    drawfunc = ShutterZPut32;
                else
                    drawfunc = ZPut32;
            }

            else
            {
                if (dp->drawmode & DM_SHUTTER)  
                    drawfunc = ShutterZPut;
                else
                    drawfunc = ZPut;
            }
        }
    }

    else    // No ZBuffer or Normal buffer
    {
        if (dp->drawmode & DM_STRETCH)
        {
            if (dp->drawmode & DM_ZMASK)
                drawfunc = MaskStretch;
            
            else if (dp->drawmode & DM_TRANSLUCENT)
                drawfunc = TranslucentStretch;
            
            else if (dp->drawmode & DM_TRANSPARENT)
                drawfunc = TransStretch;
            
            else
                drawfunc = Stretch;
        }

        else if (dp->drawmode & DM_ALPHA && dp->drawmode & DM_ZSTATIC)
        {
            if (dp->intensity < 31)
                drawfunc = AlphaDimZ;
            
            else
            {
                if (db->srcbitmapflags & BM_8BIT)
                    drawfunc = AlphaZ8;

                else
                    drawfunc = AlphaZ;
            }
        }

        else if (dp->drawmode & DM_ALPHA)
        {
            if (db->dstbitmapflags & BM_32BIT)
                drawfunc = Alpha32;

            else
            {
                if (dp->intensity < 31)
                    drawfunc = AlphaDim;
            
                else
                {
                    if (db->srcbitmapflags & BM_8BIT)
                        drawfunc = Alpha8;

                    else
                        drawfunc = Alpha;
                }
            }
        }

        else if (dp->drawmode & DM_ALPHALIGHTEN)
            drawfunc = AlphaLighten;

        else if (dp->drawmode & DM_TRANSLUCENT)
        {
            if (dp->drawmode & DM_TRANSPARENT)
                drawfunc = TransTranslucent;
            
            else
                drawfunc = Translucent;
        }    

        else
        {
            if (dp->drawmode & DM_TRANSPARENT)
            {                
                if (db->srcbitmapflags & BM_8BIT)
                {
                    if (db->dstbitmapflags & BM_32BIT)
                    {
                        if (dp->drawmode & DM_ZSTATIC)
                            drawfunc = TransZStaticPut8;
                        else
                            drawfunc = TransPut8;
                    }
                    else if (db->dstbitmapflags & BM_8BIT)
                        drawfunc = TransPut88;
                    else if (db->dstbitmapflags & (BM_16BIT | BM_15BIT))
                    {
                        if (dp->drawmode & DM_ZSTATIC)
                            drawfunc = TransZStaticPut816;
                        else
                            drawfunc = TransPut816;
                    }
                }
                else if (db->dstbitmapflags & (BM_32BIT | BM_24BIT))
                    drawfunc = TransPut32;
    
                else
                {
                    if (dp->drawmode & DM_ALIAS)
                    {
                        if (dp->drawmode & DM_CHANGECOLOR)
                            drawfunc = AliasColor;
                        else
                            drawfunc = Alias;
                    }
                    else if (dp->drawmode & DM_CHANGECOLOR)
                        drawfunc = TransPutColor;
                
                    else if (dp->drawmode & DM_CHANGEHUE)
                        drawfunc = PutHueChange;        // make a TransPut version someday

                    else if (dp->drawmode & DM_CHANGESV)
                        drawfunc = TransPutSVChange;

                    else if (db->keycolor)
                        drawfunc = TransPutKey;

                    else
                        drawfunc = TransPut;
                }
            }

            else
            {                
                if (db->srcbitmapflags & BM_8BIT && db->dstbitmapflags & BM_32BIT)
                {
                    if (dp->drawmode & DM_SHUTTER)  
                        drawfunc = ShutterPut8;
                        
                    else
                        drawfunc = Put8;
                }

                else if (db->srcbitmapflags & BM_8BIT && db->dstbitmapflags & (BM_16BIT | BM_15BIT))
                {
                    if (dp->drawmode & DM_SHUTTER)  
                        drawfunc = ShutterPut816;
                        
                    else
                        drawfunc = Put816;
                }
                else if (db->srcbitmapflags & BM_8BIT && db->dstbitmapflags & BM_8BIT)
                {
                    drawfunc = Put88;
                }
                else if (db->dstbitmapflags & (BM_32BIT | BM_24BIT))
                {
                    if (dp->drawmode & DM_SHUTTER)  
                        drawfunc = ShutterPut32;
                        
                    else
                        drawfunc = Put32;
                }

                else
                {
                    if (dp->drawmode & DM_SHUTTER)  
                        drawfunc = ShutterPut8;
                    else if (dp->drawmode & DM_CHANGEHUE)
                        drawfunc = PutHueChange;
                    else
                        drawfunc = Put;
                }
            }                   
        }

    } // End of no Zbuffer/Normal Buffer

    return drawfunc;
}

// ****************************************************
// * General purpose bitmap buffer clipping functions *
// ****************************************************

// Returns the intersection of two rectangles (always a single rectangle)
bool ClipRect(const SRect& dst, const SRect& src, RSRect result)
{
    if (src.left > dst.right || src.top > dst.bottom ||
        src.right < dst.left || src.bottom < dst.top)
            return false;

    if (src.left < dst.left)
        result.left = dst.left;
    else
        result.left = src.left;

    if (src.right > dst.right)
        result.right = dst.right; 
    else
        result.right = src.right;

    if (src.top < dst.top)
        result.top = dst.top;
    else
        result.top = src.top;

    if (src.bottom > dst.bottom)
        result.bottom = dst.bottom;
    else
        result.bottom = src.bottom;

    return true;
}

// Returns the inverse of the intersection of two rectangles (subtraction) returns up to 4 rects
bool SubtractRect(const SRect& dst, const SRect& src, PSRect rects, int32_t &numrects)
{
    numrects = 0;

    if (src.left > dst.right || src.top > dst.bottom ||
        src.right < dst.left || src.bottom < dst.top)
            return false;

    PSRect r = rects;

    if (src.left > dst.left)
    {
        r->left = dst.left;
        r->top = dst.top;
        r->right = src.left - 1;
        r->bottom = dst.bottom;
        r++;
        numrects++;
    }
    
    if (src.right < dst.right)
    {
        r->left = src.right + 1;
        r->top = dst.top;
        r->right = dst.right;
        r->bottom = dst.bottom;
        r++;
        numrects++;
    }       

    if (src.top > dst.top)
    {
        r->left = max(dst.left, src.left);
        r->top = dst.top;
        r->right = min(dst.right, src.right);
        r->bottom = src.top - 1;
        r++;
        numrects++;
    }       

    if (src.bottom < dst.bottom)
    {
        r->left = max(dst.left, src.left);
        r->top = src.bottom + 1;
        r->right = min(dst.right, src.right);
        r->bottom = dst.bottom;
        r++;
        numrects++;
    }       

    return numrects > 0;
}

bool Clip(PSDrawBlock db, PSDrawParam dp, PSDrawParam dparray, int32_t &numrects)
{
    numrects = 0;
    dparray[0] = *dp;

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    // ********* Clip source to source buffer!! **********
    // Note: source rect doesn not use origins or clipping rectangle, just clips
    // to source surface size

    // There actually is a source buffer
    if (dbval.sbufwidth != 0 && dbval.sbufheight != 0)
    {

        if (!(dpval.drawmode & (DM_WRAPCLIPSRC | DM_WRAPCLIP)))         // Not wrap clipping
        {
            if (dpval.sx >= dbval.sbufwidth || dpval.sy >= dbval.sbufheight ||
                dpval.sx + dpval.swidth < 0 || dpval.sy + dpval.sheight < 0)
                    return false;

            if (dpval.sx < 0)
            {
                dpval.dwidth -= -dpval.sx;
                dpval.swidth -= -dpval.sx;
                dpval.dx += -dpval.sx;
                dpval.sx = 0;
            }
            if (dpval.sy < 0)
            {
                dpval.dheight -= -dpval.sy;
                dpval.sheight -= -dpval.sy;
                dpval.dy += -dpval.sy;
                dpval.sy = 0;
            }
            if (dpval.sx + dpval.swidth > dbval.sbufwidth)
            {
                dpval.dwidth -= dpval.swidth + dpval.sx - dbval.sbufwidth; 
                dpval.swidth = dbval.sbufwidth - dpval.sx;
            }
            if (dpval.sy + dpval.sheight > dbval.sbufheight)
            {
                dpval.dheight -= dpval.sheight + dpval.sy - dbval.sbufheight; 
                dpval.sheight = dbval.sbufheight - dpval.sy;
            }
        }
        else // Make sure buf width/height not to large for wrapping
        {
            if (dpval.swidth > dbval.sbufwidth)
            {
                dpval.dwidth -= dpval.swidth - dbval.sbufwidth;
                dpval.swidth = dbval.sbufwidth;
            }
            if (dpval.sheight > dbval.sbufheight)
            {
                dpval.dheight -= dpval.sheight - dbval.sbufheight;
                dpval.sheight = dbval.sbufheight;
            }
            if (dpval.swidth <= 0 || dpval.sheight <= 0)
                return false;
        }
    }

    // ********* Clip the dest clipping rectange !! **********
    int32_t cx, cy, cw, ch;
    cx = dpval.clipx;
    cy = dpval.clipy;
    cw = dpval.clipwidth;
    ch = dpval.clipheight;

    if (!(dpval.drawmode & (DM_WRAPCLIP | DM_WRAPCLIPSRC)))
    {
        if (cx >= dbval.dbufwidth || cy >= dbval.dbufheight ||
            cx + cw <= 0 || cy + ch <= 0) 
            return false;

        if (cx < 0)
        {
            cw += cx;
            cx = 0;
        }

        if (cy < 0)
        {
            ch += cy;
            cy = 0;
        }
        
        if (cx + cw > dbval.dbufwidth)
           cw = dbval.dbufwidth - cx;

        if (cy + ch > dbval.dbufheight)
           ch = dbval.dbufheight - cy;

    }

    // ********* Do Edge Clipping **********

    int32_t destx = dpval.dx + dpval.originx;
    int32_t desty = dpval.dy + dpval.originy;

    if ((destx >= cx + cw) || (destx + dpval.dwidth <= cx) ||
        (desty >= cy + ch) || (desty + dpval.dheight <= cy))
       return false;

    if (destx < cx)
    {
        dpval.sx    += cx - destx;
        dpval.dwidth -= cx - destx;
        dpval.swidth = dpval.dwidth;
        dpval.dx     = cx - dpval.originx;
        destx = cx;
    }
        
    if (desty < cy)
    {
        dpval.sy     += cy - desty;
        dpval.dheight -= cy - desty;
        dpval.sheight = dpval.dheight;
        dpval.dy      = cy - dpval.originy;
        desty = cy;
    }

    if ((destx + dpval.dwidth) > (cx + cw))
        dpval.dwidth = dpval.swidth = (cx + cw) - destx;    

    if ((desty + dpval.dheight) > (cy + ch))
        dpval.dheight = dpval.sheight = (cy + ch) - desty;

    PSDrawParam dpa = dparray;

    if (!(dpval.drawmode & DM_WRAPCLIP) && !(dpval.drawmode & DM_WRAPCLIPSRC))
    {
    // ********* No Wrap Clip **********

        dpa->dx      = dpval.dx;
        dpa->dy      = dpval.dy;
        dpa->sx      = dpval.sx;
        dpa->sy      = dpval.sy;
        dpa->dwidth  = dpa->swidth  = dpval.dwidth;
        dpa->dheight = dpa->sheight = dpval.dheight;
        dpa->originx = dpval.originx;
        dpa->originy = dpval.originy;
       
        numrects = 1;
    }

    else if (dpval.drawmode & DM_WRAPCLIP)
    {
    // ********* Do Dest Wrap Clipping **********

        int32_t srcx = dpval.sx;
        int32_t srcy = dpval.sy;

        destx = dpval.dx + dpval.originx;
        desty = dpval.dy + dpval.originy;

        int32_t dstx1, dsty1, srcx1, srcy1, srcwidth1, srcheight1;
        int32_t dstx2, dsty2, srcx2, srcy2, srcwidth2, srcheight2;

        dparray[1] = *dp;
        dparray[2] = *dp;
        dparray[3] = *dp;

        destx = destx % max(dbval.dbufwidth, 1);
        if (destx < 0) 
            destx += dbval.dbufwidth;
        
        desty = desty % max(dbval.dbufheight, 1);
        if (desty < 0) 
            desty += dbval.dbufheight;

        if (dpval.drawmode & DM_WRAPCLIPSRC) // Allow ONLY if buffers same size
        {
            if (dbval.sbufwidth != dbval.dbufwidth || dbval.sbufheight != dbval.dbufheight ||
                dp->sx != dp->dx || dp->sy != dp->dy || dp->swidth != dp->dwidth || dp->sheight != dp->dheight)
                FatalError("Can't use DM_WRAPCLIP and DM_WRAPCLIPSRC together unless buffers and coords match!");

            srcx = srcx % max(dbval.sbufwidth, 1);
            if (srcx < 0) 
                srcx += dbval.sbufwidth;
        
            srcy = srcy % max(dbval.sbufheight, 1);
            if (srcy < 0) 
                srcy += dbval.sbufheight;
        }

      // Do wrapping stuff
        if (destx + dpval.dwidth > dbval.dbufwidth)
        {
            dstx1     = destx - dpval.originx;
            srcx1     = srcx;
            srcwidth1 = dbval.dbufwidth - destx;
            dstx2     = 0 - dpval.originx;
            if (dpval.drawmode & DM_WRAPCLIPSRC)
                srcx2  = 0;
            else 
                srcx2  = srcx1 + srcwidth1;
            srcwidth2 = dpval.dwidth - srcwidth1;
        }

        else
        {
            dstx1     = destx - dpval.originx;
            srcx1     = srcx;
            srcwidth1 = dpval.dwidth;
            dstx2     = 0 - dpval.originx;
            srcx2     = srcwidth2 = 0;
        }

        if (desty + dpval.dheight > dbval.dbufheight)
        {
            dsty1      = desty - dpval.originy;
            srcy1      = srcy;
            srcheight1 = dbval.dbufheight - desty;
            dsty2      = 0 - dpval.originy;
            if (dpval.drawmode & DM_WRAPCLIPSRC)
                srcy2  = 0;
            else 
                srcy2  = srcy1 + srcheight1; 
            srcheight2 = dpval.dheight - srcheight1;
        }

        else
        {
            dsty1      = desty - dpval.originy;
            srcy1      = srcy;
            srcheight1 = dpval.dheight;
            dsty2      = 0 - dpval.originy;
            srcy2      = srcheight2 = 0;
        }
    
      // Upper left corner
        dpa->dx      = dstx1;
        dpa->dy      = dsty1;
        dpa->sx      = srcx1;
        dpa->sy      = srcy1;
        dpa->dwidth  = dpa->swidth = srcwidth1;
        dpa->dheight = dpa->sheight = srcheight1;
       
        numrects = 1;

      // Lower left corner
        if (srcheight2)
        {
            dpa++;
            dpa->dx      = dstx1;
            dpa->dy      = dsty2;
            dpa->sx      = srcx1;
            dpa->sy      = srcy2;
            dpa->dwidth  = dpa->swidth = srcwidth1;
            dpa->dheight = dpa->sheight = srcheight2;
       
            numrects++;
        }
    
      // Upper right corner
        if (srcwidth2)
        {
            dpa++;
            dpa->dx      = dstx2;
            dpa->dy      = dsty1;
            dpa->sx      = srcx2;
            dpa->sy      = srcy1;
            dpa->dwidth  = dpa->swidth = srcwidth2;
            dpa->dheight = dpa->sheight = srcheight1;
       
            numrects++;

          // Lower right corner
            if (srcheight2)
            {
                dpa++;
                dpa->dx      = dstx2;
                dpa->dy      = dsty2;
                dpa->sx      = srcx2;
                dpa->sy      = srcy2;
                dpa->dwidth  = dpa->swidth = srcwidth2;
                dpa->dheight = dpa->sheight = srcheight2;
       
                numrects++;
            }
        }
    }

    else if (dpval.drawmode & DM_WRAPCLIPSRC)
    {
    // ********* Do Source Wrap Clipping **********

        int32_t srcx = dpval.sx;
        int32_t srcy = dpval.sy;

        int32_t dstx1, dsty1, srcx1, srcy1, srcwidth1, srcheight1;
        int32_t dstx2, dsty2, srcx2, srcy2, srcwidth2, srcheight2;

        dparray[1] = *dp;
        dparray[2] = *dp;
        dparray[3] = *dp;

        srcx = srcx % max(dbval.sbufwidth, 1);
        if (srcx < 0) 
            srcx += dbval.sbufwidth;
        
        srcy = srcy % max(dbval.sbufheight, 1);
        if (srcy < 0) 
            srcy += dbval.sbufheight;

      // Do wrapping stuff
        if (srcx + dpval.dwidth > dbval.sbufwidth)
        {
            dstx1     = dpval.dx;
            srcx1     = srcx;
            srcwidth1 = dbval.sbufwidth - srcx;
            dstx2     = dstx1 + srcwidth1;
            srcx2     = 0;
            srcwidth2 = dpval.dwidth - srcwidth1;
        }

        else
        {
            dstx1     = dpval.dx;
            srcx1     = srcx;
            srcwidth1 = dpval.dwidth;
            dstx2     = srcx2 = srcwidth2 = 0;
        }

        if (srcy + dpval.dheight > dbval.sbufheight)
        {
            dsty1      = dpval.dy;
            srcy1      = srcy;
            srcheight1 = dbval.sbufheight - srcy;
            dsty2      = dsty1 + srcheight1;
            srcy2      = 0; 
            srcheight2 = dpval.dheight - srcheight1;
        }

        else
        {
            dsty1      = dpval.dy;
            srcy1      = srcy;
            srcheight1 = dpval.dheight;
            dsty2      = srcy2 = srcheight2 = 0;
        }
    
      // Upper left corner
        dpa->dx      = dstx1;
        dpa->dy      = dsty1;
        dpa->sx      = srcx1;
        dpa->sy      = srcy1;
        dpa->dwidth  = dpa->swidth  = srcwidth1;
        dpa->dheight = dpa->sheight = srcheight1;
       
        numrects = 1;

      // Lower left corner
        if (srcheight2)
        {
            dpa++;
            dpa->dx      = dstx1;
            dpa->dy      = dsty2;
            dpa->sx      = srcx1;
            dpa->sy      = srcy2;
            dpa->dwidth  = dpa->swidth = srcwidth1;
            dpa->dheight = dpa->sheight = srcheight2;
       
            numrects++;
        }
    
      // Upper right corner
        if (srcwidth2)
        {
            dpa++;
            dpa->dx      = dstx2;
            dpa->dy      = dsty1;
            dpa->sx      = srcx2;
            dpa->sy      = srcy1;
            dpa->dwidth  = dpa->swidth = srcwidth2;
            dpa->dheight = dpa->sheight = srcheight1;
       
            numrects++;

          // Lower right corner
            if (srcheight2)
            {
                dpa++;
                dpa->dx      = dstx2;
                dpa->dy      = dsty2;
                dpa->sx      = srcx2;
                dpa->sy      = srcy2;
                dpa->dwidth  = dpa->swidth = srcwidth2;
                dpa->dheight = dpa->sheight = srcheight2;
       
                numrects++;
            }
        }
    }

    return true;
}

bool StretchClip(PSDrawBlock db, PSDrawParam dp, PSDrawParam dparray, 
                 int32_t &numrects)
{
    numrects = 0;
    return false;
}

// ***************************************************
// * General purpose bitmap buffer drawing functions *
// ***************************************************

bool Put(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false; // No longer supported
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return ClipDecompress(db, dp, false);
//      else
//          return Decompress(db, dp, false);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dpval.drawmode]

        and  ebx, DM_REVERSEHORZ
        jne  reverse

    forward:
        mov  ecx, [dpval.swidth]     ; Do two pixels at a time

        test ecx, 1
        je   notodd

        mov  ax, [esi]
        add  esi, 2
        mov  [edi], ax
        add  edi, 2

    notodd:
        shr  ecx, 1
        or   ecx, ecx
        je   newline

    floop:
        mov  eax, [esi]
        add  esi, 4

        mov  [edi], eax
        add  edi, 4

        dec  ecx
        jne  floop

    newline:
        add  esi, srcadd
        add  edi, dstadd
        dec  edx
        jne  forward
        jmp  done

    reverse:
        add  esi, [bmwidth]
        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

    revloop:
        mov  ecx, [dpval.swidth]     ; Do two pixels at a time backwords

        test ecx, 1
        je   revnotodd

        mov  ax, [esi]
        sub  esi, 2
        mov  [edi], ax
        add  edi, 2
        
    revnotodd:
        shr  ecx, 1
        or   ecx, ecx
        je   revnewline

    revmoveloop:
        mov  eax, [esi]
        sub  esi, 4
        rol  eax, 16
        mov  [edi], eax
        add  edi, 4
        dec  ecx
        jne  revmoveloop

    revnewline:
        add  esi, srcadd
        add  edi, dstadd
        dec  edx
        jne  revloop

    done:
    }
    #endif

    return true;
}

bool Put8(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette + sizeof(uint16_t) * 256);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return false; //ClipDecompress(db, dp, false);
//
//      else
//          return false; //Decompress(db, dp, false);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dpval.drawmode]

        and  ebx, DM_REVERSEHORZ
        jne  reverse

    forward:
        mov  ecx, [dpval.swidth]

    floop:
        xor  eax, eax
        mov  al, [esi]
        shl  eax, 2
        add  eax, [palette]
        mov  eax, [eax]

        mov  [edi], eax

        inc  esi
        add  edi, 4

        dec  ecx
        jne  floop

        add  esi, srcadd
        add  edi, dstadd

        dec  edx
        jne  forward
        jmp  done

    reverse:
        add  esi, [bmwidth]
        mov  eax, [bmwidth]
        add  eax, srcadd
        mov  srcadd, eax

    revloop:
        mov  ecx, [dpval.swidth]

    revmoveloop:
        xor  eax, eax
        mov  al, [esi]
        shl  eax, 2
        add  eax, [palette]
        mov  eax, [eax]

        mov  [edi], eax

        dec  esi
        add  edi, 4

        dec  ecx
        jne  revmoveloop

        add  esi, srcadd
        add  edi, dstadd

        dec  edx
        jne  revloop

    done:
    }
    #endif

    return true;
}

bool Put88(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          ClipDecompress(db, dp, true);
//      else
//          Decompress(db, dp, true);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    int32_t width = bmwidth >> 2;
    int32_t odd = bmwidth & 3;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, [srcoff]

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, [dstoff]

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dpval.drawmode]

        and  ebx, DM_REVERSEHORZ
        jne  reverse

    forloop:
        mov  ecx, [odd]
        or   ecx, ecx
        jz   notodd
    oddloop:
        mov  al, [esi]
        mov  [edi], al
        inc  esi
        inc  edi
        dec  ecx
        jne  oddloop

    notodd:
        mov  ecx, [width]
        or   ecx, ecx
        jz   nextline

    floop:
        mov  eax, [esi]
        mov  [edi], eax
        add  esi, 4
        add  edi, 4

        dec  ecx
        jne  floop

    nextline:
        add  edi, [dstadd]
        add  esi, [srcadd]

        dec  edx
        jne  forloop

        jmp  done

    reverse:
        mov  eax, [width]
        add  esi, eax
             
        shl  eax, 1
        add  eax, [srcadd]
        mov  [srcadd], eax

    revloop:
        mov  ecx, [width]

    rloop:
        mov  eax, [esi]
        mov  [edi], eax
        sub  esi, 4
        add  edi, 4

        dec  ecx
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd

        dec  edx
        jne  revloop

    done:
    }
    #endif
    return true;
}

bool Put816(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    uint32_t *palette = (uint32_t *)(db->palette);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          ClipDecompress(db, dp, false);
//      else
//          Decompress(db, dp, false);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dpval.drawmode]

        and  ebx, DM_REVERSEHORZ
        jne  reverse

    forward:
        mov  ecx, [dpval.swidth]

    floop:
        xor  eax, eax
        mov  al, [esi]
        shl  eax, 1
        add  eax, [palette]
        mov  ax, [eax]

        mov  [edi], ax

        inc  esi
        add  edi, 2

        dec  ecx
        jne  floop

        add  esi, srcadd
        add  edi, dstadd

        dec  edx
        jne  forward
        jmp  done

    reverse:
        mov  eax, [dbval.sstride]
        add  esi, eax
        shl  eax, 1
        add  srcadd, eax

    revloop:
        mov  ecx, [dpval.swidth]

    revmoveloop:
        xor  eax, eax
        mov  al, [esi]
        shl  eax, 1
        add  eax, [palette]
        mov  ax, [eax]

        mov  [edi], ax

        dec  esi
        add  edi, 2

        dec  ecx
        jne  revmoveloop

        add  esi, srcadd
        add  edi, dstadd

        dec  edx
        jne  revloop

    done:
    }
    #endif

    return true;
}

// *********** 32 bit Bitmap Transfer Routine *********

bool Put32(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
            db->sbufheight != dp->sheight)
            return false;

        else 
            return false;
    }

    if (db->dstbitmapflags & BM_32BIT)
    {
        SDrawBlock dbval = *db;
        SDrawParam dpval = *dp;

        SETUP_DRAW

        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            mov  esi, [dbval.source]    ; Point ESI to source
            add  esi, [srcoff]          ; Adjusts ESI for offset

            mov  edi, [dbval.dest]      ; Point EDI to destination
            add  edi, [dstoff]          ; Adjusts EDI for offset
 
            mov  edx, [dpval.sheight]
            mov  ebx, [dpval.drawmode]

            and  ebx, DM_REVERSEHORZ
            jne  Reverse32

            xor  ebx, ebx

        OuterLoop32:
            mov  ecx, [dpval.swidth]

        InnerLoop32:
            mov  bl, [esi]
            mov  eax, [Conv16to32Lower + ebx * 4]
            mov  bl, [esi + 1]
            add  eax, [Conv16to32Upper + ebx * 4]

            mov  [edi], eax

            add  esi, 2
            add  edi, 4
            
            dec  ecx
            jne  InnerLoop32
            
            add  esi, srcadd
            add  edi, dstadd

            dec  edx
            jne  OuterLoop32
            
            jmp  Done32

        Reverse32:
            xor  ebx, ebx

            add  esi, [bmwidth]
            mov  eax, [bmwidth]
            shl  eax, 1
            add  eax, srcadd
            mov  srcadd, eax
    
        RevOuterLoop32:
            mov     ecx, [dpval.swidth]     ; Do two pixels at a time backwords

        RevInnerLoop32:
            mov  bl, [esi]
            mov  eax, [Conv16to32Lower + ebx * 4]
            mov  bl, [esi + 1]
            add  eax, [Conv16to32Upper + ebx * 4]

            mov  [edi], eax

            sub  esi, 2
            add  edi, 4
            
            dec  ecx
            jne  RevInnerLoop32
            
            add  esi, srcadd
            add  edi, dstadd
            dec  edx
            jne  RevOuterLoop32

        Done32:
        }
        #endif
    }

    if (db->dstbitmapflags & BM_24BIT)
    {
        SDrawBlock dbval = *db;
        SDrawParam dpval = *dp;

        SETUP_DRAW

        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            mov  esi, [dbval.source]    ; Point ESI to source
            add  esi, [srcoff]          ; Adjusts ESI for offset

            mov  edi, [dbval.dest]      ; Point EDI to destination
            add  edi, [dstoff]          ; Adjusts EDI for offset
 
            mov  edx, [dpval.sheight]
            mov  ebx, [dpval.drawmode]

            and  ebx, DM_REVERSEHORZ
            jne  Reverse24

            xor  ebx, ebx

        OuterLoop24:
            mov  ecx, [dpval.swidth]
            shr  ecx, 1
        
        InnerLoop24:
            mov  bl, [esi]
            mov  eax, [Conv16to32Lower + ebx * 4]
            mov  bl, [esi + 1]
            add  eax, [Conv16to32Upper + ebx * 4]

            add  esi, 2
            add  edi, 3
            
            dec  ecx
            jne  InnerLoop24
            
            add  esi, srcadd
            add  edi, dstadd

            dec  edx
            jne  OuterLoop24
            
            jmp  Done24

        Reverse24:
            xor  ebx, ebx

            add  esi, [bmwidth]
            mov  eax, [bmwidth]
            shl  eax, 1
            add  eax, srcadd
            mov  srcadd, eax
    
        RevOuterLoop24:
            mov  ecx, [dpval.swidth]     ; Do two pixels at a time backwords

        RevInnerLoop24:
            mov  bl, [esi]
            mov  eax, [Conv16to32Lower + ebx * 4]
            mov  bl, [esi + 1]
            add  eax, [Conv16to32Upper + ebx * 4]

            sub  esi, 2
            add  edi, 3
            
            dec  ecx
            jne  RevInnerLoop24
            
            add  esi, srcadd
            add  edi, dstadd

            dec  edx
            jne  RevOuterLoop24

        Done24:
        }
        #endif
    }

    return true;
}

bool ShutterPut(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return ClipDecompress(db, dp, false);
//      else
//          return Decompress(db, dp, false);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    uint32_t pixelx;   // Current pixel screen pos
    uint32_t pixely;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dpval.drawmode]
        
        mov  eax, [dpval.dy]
        mov  [pixely], eax

        and  ebx, DM_REVERSEHORZ
        jne  reverse

    forward:
        mov  eax, [dpval.dx]
        mov  [pixelx], eax

        mov  ecx, [dpval.swidth]     ; Do two pixels at a time

    floop:
        test [pixely], 1
        jne  line0
        
        test [pixelx], 1
        je   skipraw
    
        jmp  draw   

    line0:
        test [pixelx], 1
        jne  skipraw
    
    draw:
        mov  ax, [esi]
        add  esi, 2

        mov  [edi], ax
        add  edi, 2

        inc  [pixelx]
        dec  ecx

        jne  floop

    skipraw:
        add  esi, srcadd
        add  edi, dstadd

        inc  [pixely]
        dec  edx

        jne  forward

        jmp  done

    reverse:
        add  esi, [bmwidth]
        mov  eax, [bmwidth]

        shl  eax, 1
        add  eax, srcadd

        mov  srcadd, eax

    revloop:
        mov  eax, [dpval.dx]
        mov  [pixelx], eax

        mov  ecx, [dpval.swidth]     ; Do two pixels at a time backwords

    revmoveloop:
        test [pixely], 1
        jne  revline0
        
        test [pixelx], 1
        je   revskipraw
    
        jmp  revdraw    

    revline0:
        test [pixelx], 1
        jne  revskipraw
    
    revdraw:
        mov  ax, [esi]
        sub  esi, 2
     
        mov  [edi], ax
        add  edi, 2

        inc  [pixelx]
        dec  ecx

        jne  revmoveloop

    revskipraw:
        add  esi, srcadd
        add  edi, dstadd

        inc  [pixely]
        dec  edx

        jne  revloop

    done:
    }
    #endif

    return true;
}

bool ShutterPut8(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette + sizeof(uint16_t) * 256);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return false; //ClipDecompress(db, dp, false);
//
//      else
//          return false; //Decompress(db, dp, false);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    uint32_t pixelx;   // Screen pos of current pixel
    uint32_t pixely;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dpval.drawmode]
        
        mov  eax, [dpval.dy]
        mov  [pixely], eax

        and  ebx, DM_REVERSEHORZ
        jne  reverse

    forward:
        mov  eax, [dpval.dx]
        mov  [pixelx], eax

        mov  ecx, [dpval.swidth]

    floop:
        test [pixely], 1
        jne  line0
        
        test [pixelx], 1
        je   SkipRaw
    
        jmp  draw   

    line0:
        test [pixelx], 1
        jne  SkipRaw
    
    draw:
        xor  eax, eax
        mov  al, [esi]
        shl  eax, 2
        add  eax, [palette]
        mov  eax, [eax]

        mov  [edi], eax

    SkipRaw:
        inc  esi
        add  edi, 4

        inc  [pixelx]
        dec  ecx

        jne  floop

        add  esi, srcadd
        add  edi, dstadd

        inc  [pixely]
        dec  edx

        jne  forward
        jmp  done

    reverse:
        add  esi, [bmwidth]
        mov  eax, [bmwidth]

        add  eax, srcadd
        mov  srcadd, eax

    revloop:
        mov  eax, [dpval.dx]
        mov  [pixelx], eax

        mov  ecx, [dpval.swidth]

    revmoveloop:
        test [pixely], 1
        jne  Revline0
        
        test [pixelx], 1
        je   revskipraw
    
        jmp  Revdraw    

    Revline0:
        test [pixelx], 1
        jne  revskipraw
    
    Revdraw:
        xor  eax, eax
        mov  al, [esi]
    
        shl  eax, 2
        add  eax, [palette]
    
        mov  eax, [eax]
        mov  [edi], eax

    revskipraw:
        dec  esi
        add  edi, 4

        inc  [pixelx]
        dec  ecx

        jne  revmoveloop

        add  esi, srcadd
        add  edi, dstadd

        inc  [pixely]
        dec  edx

        jne  revloop

    done:
    }
    #endif

    return true;
}

bool ShutterPut816(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return false; //ClipDecompress(db, dp, false);
//      else
//          return false; //Decompress(db, dp, false);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    uint32_t pixelx;   // Screen pos of current pixel
    uint32_t pixely;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dpval.drawmode]

        mov  eax, [dpval.dy]
        mov  [pixely], eax

        and  ebx, DM_REVERSEHORZ
        jne  reverse

    forward:
        mov  eax, [dpval.dx]
        mov  [pixelx], eax

        mov  ecx, [dpval.swidth]

    floop:
        test [pixely], 1
        jne  line0
        
        test [pixelx], 1
        je   SkipRaw
    
        jmp  draw   

    line0:
        test [pixelx], 1
        jne  SkipRaw
    
    draw:
        xor  eax, eax
        mov  al, [esi]
        shl  eax, 1
        add  eax, [palette]
        mov  ax, [eax]

        mov  [edi], ax

   SkipRaw:
        inc  esi
        add  edi, 2

        inc  [pixelx]
        dec  ecx

        jne  floop

        add  esi, srcadd
        add  edi, dstadd

        inc  [pixely]
        dec  edx

        jne  forward

        jmp  done

    reverse:
        add  esi, [dbval.sstride]
        mov  eax, [dbval.sstride]

        shl  eax, 1
        add  srcadd, eax

    revloop:
        mov  eax, [dpval.dx]
        mov  [pixelx], eax

        mov  ecx, [dpval.swidth]

    revmoveloop:
        test [pixely], 1
        jne  Revline0
        
        test [pixelx], 1
        je   RevSkipRaw
    
        jmp  Revdraw    

    Revline0:
        test [pixelx], 1
        jne  RevSkipRaw
    
    Revdraw:
        xor  eax, eax
        mov  al, [esi]

        shl  eax, 1
        add  eax, [palette]

        mov  ax, [eax]
        mov  [edi], ax

    RevSkipRaw:
        dec  esi
        add  edi, 2

        inc  [pixelx]
        dec  ecx

        jne  revmoveloop

        add  esi, srcadd
        add  edi, dstadd

        inc  [pixely]
        dec  edx

        jne  revloop

    done:
    }
    #endif

    return true;
}

// *********** 32 bit Bitmap Transfer Routine *********

bool ShutterPut32(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
            db->sbufheight != dp->sheight)
            return false;

        else 
            return false;
    }

    if (db->dstbitmapflags & BM_32BIT)
    {
        SDrawBlock dbval = *db;
        SDrawParam dpval = *dp;

        SETUP_DRAW

        uint32_t pixelx;   // Screen pos of current pixel
        uint32_t pixely;

        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            mov  esi, [dbval.source]    ; Point ESI to source
            add  esi, [srcoff]          ; Adjusts ESI for offset

            mov  edi, [dbval.dest]      ; Point EDI to destination
            add  edi, [dstoff]          ; Adjusts EDI for offset
 
            mov  eax, [dpval.dy]
            mov  [pixely], eax

            mov  edx, [dpval.sheight]
            mov  ebx, [dpval.drawmode]

            and  ebx, DM_REVERSEHORZ
            jne  Reverse32

            xor  ebx, ebx

        OuterLoop32:
            mov  eax, [dpval.dx]
            mov  [pixelx], eax

            mov  ecx, [dpval.swidth]

        InnerLoop32:
            test [pixely], 1
            jne  line32
        
            test [pixelx], 1
            je   SkipRaw32
    
            jmp  draw32 

        line32:
            test [pixelx], 1
            jne  SkipRaw32
    
        draw32:
            mov  bl, [esi]
            mov  eax, [Conv16to32Lower + ebx * 4]
    
            mov  bl, [esi + 1]
            add  eax, [Conv16to32Upper + ebx * 4]

            mov  [edi], eax

        SkipRaw32:
            add  esi, 2
            add  edi, 4
            
            inc  [pixelx]
            dec  ecx

            jne  InnerLoop32
            
            add  esi, srcadd
            add  edi, dstadd

            inc  [pixely]
            dec  edx

            jne  OuterLoop32
            
            jmp  Done32

        Reverse32:
            xor  ebx, ebx
            add  esi, [bmwidth]
            
            mov  eax, [bmwidth]
            shl  eax, 1

            add  eax, srcadd
            mov  srcadd, eax
    
        RevOuterLoop32:
            mov  eax, [dpval.dx]
            mov  [pixelx], eax

            mov  ecx, [dpval.swidth]     ; Do two pixels at a time backwords

        RevInnerLoop32:
            test [pixely], 1
            jne  Revline32
        
            test [pixelx], 1
            je   RevSkipRaw32
    
            jmp  Revdraw32

        Revline32:
            test [pixelx], 1
            jne  SkipRaw32
    
        Revdraw32:
            mov  bl, [esi]
            mov  eax, [Conv16to32Lower + ebx * 4]

            mov  bl, [esi + 1]
            add  eax, [Conv16to32Upper + ebx * 4]

            mov  [edi], eax

        RevSkipRaw32:
            sub  esi, 2
            add  edi, 4
            
            inc  [pixelx]
            dec  ecx

            jne  RevInnerLoop32
            
            add  esi, srcadd
            add  edi, dstadd

            inc  [pixely]
            dec  edx

            jne  RevOuterLoop32

        Done32:
        }
        #endif
    }

    if (db->dstbitmapflags & BM_24BIT)
    {
        SDrawBlock dbval = *db;
        SDrawParam dpval = *dp;

        SETUP_DRAW

        uint32_t pixelx;   // Screen pos of current pixel
        uint32_t pixely;

        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            mov  esi, [dbval.source]    ; Point ESI to source
            add  esi, [srcoff]          ; Adjusts ESI for offset

            mov  edi, [dbval.dest]      ; Point EDI to destination
            add  edi, [dstoff]          ; Adjusts EDI for offset
 
            mov  edx, [dpval.sheight]
            mov  ebx, [dpval.drawmode]

            mov  eax, [dpval.dy]
            mov  [pixely], eax

            and  ebx, DM_REVERSEHORZ
            jne  Reverse24

            xor  ebx, ebx

        OuterLoop24:
            mov  eax, [dpval.dx]
            mov  [pixelx], eax

            mov  ecx, [dpval.swidth]
        
        InnerLoop24:
            test [pixely], 1
            jne  line24
        
            test [pixelx], 1
            je   SkipRaw24
    
            jmp  draw24

        line24:
            test [pixelx], 1
            jne  SkipRaw24
    
        draw24:
            mov  bl, [esi]
            mov  eax, [Conv16to32Lower + ebx * 4]
            mov  bl, [esi + 1]
            add  eax, [Conv16to32Upper + ebx * 4]

        SkipRaw24:
            add  esi, 2
            add  edi, 3
            
            inc  [pixelx]
            dec  ecx

            jne  InnerLoop24
            
            add  esi, srcadd
            add  edi, dstadd

            inc  [pixely]
            dec  edx

            jne  OuterLoop24
            
            jmp  Done24

        Reverse24:
            xor  ebx, ebx
            add  esi, [bmwidth]

            mov  eax, [bmwidth]
            shl  eax, 1

            add  eax, srcadd
            mov  srcadd, eax
    
        RevOuterLoop24:
            mov  eax, [dpval.dx]
            mov  [pixelx], eax

            mov  ecx, [dpval.swidth]     ; Do two pixels at a time backwords

        RevInnerLoop24:
            test [pixely], 1
            jne  Revline24
        
            test [pixelx], 1
            je   RevSkipRaw24
    
            jmp  Revdraw24

        Revline24:
            test [pixelx], 1
            jne  RevSkipRaw24
    
        Revdraw24:
            mov  bl, [esi]
            mov  eax, [Conv16to32Lower + ebx * 4]
        
            mov  bl, [esi + 1]
            add  eax, [Conv16to32Upper + ebx * 4]

        RevSkipRaw24:
            sub  esi, 2
            add  edi, 3
            
            inc  [pixelx]
            dec  ecx

            jne  RevInnerLoop24
            
            add  esi, srcadd
            add  edi, dstadd

            inc  [pixelx]
            dec  edx

            jne  RevOuterLoop24

        Done24:
        }
        #endif
    }

    return true;
}

// *********** Chunk Bitmap Transfer Routines *********
bool PutChunk8(PSDrawBlock db, PSDrawParam dp)
{
    SDrawBlock dbval = *db; 
    SDrawParam dpval = *dp; 

    SDrawBlock dbval2 = *db;    
    SDrawParam dpval2 = *dp;    
    dpval2.func = nullptr;

    SETUP_DRAW

    SChunkHeader* hdr  = (SChunkHeader*)dbval.source;
    SChunkHeader* zhdr = (SChunkHeader*)dbval.szbuffer;
    SChunkHeader* nhdr = (SChunkHeader*)dbval.snormals;

    int32_t type   = hdr->type;
    int32_t width  = hdr->width;
    int32_t height = hdr->height;

    int32_t ULx = dpval.sx / CHUNKWIDTH;
    int32_t ULy = dpval.sy / CHUNKHEIGHT;
    int32_t LRx = (dpval.sx + dpval.swidth) / CHUNKWIDTH;
    int32_t LRy = (dpval.sy + dpval.sheight) / CHUNKHEIGHT;
    
    if ((dpval.sx + dpval.swidth) % CHUNKWIDTH)
        LRx++;
    
    if ((dpval.sy + dpval.sheight) % CHUNKHEIGHT)
        LRy++;

    dbval2.srcbitmapflags = dbval.srcbitmapflags & ~BM_COMPRESSED;
    dbval2.srcbitmapflags &= ~BM_CHUNKED;
    dbval2.dstbitmapflags = dbval.dstbitmapflags & ~BM_COMPRESSED;
    dbval2.dstbitmapflags &= ~BM_CHUNKED;
    dbval2.sbufwidth      = CHUNKWIDTH;
    dbval2.sbufheight     = CHUNKHEIGHT;
    dbval2.sstride        = CHUNKWIDTH;
    dbval2.szstride       = CHUNKWIDTH;

    dpval.sx %= CHUNKWIDTH;
    dpval.sy %= CHUNKHEIGHT;

    for (int32_t outerloop = ULy; outerloop < LRy; outerloop++)
    {
        for (int32_t innerloop = ULx; innerloop < LRx; innerloop++)
        {
            dpval2.sx = 0;
            dpval2.swidth = CHUNKWIDTH;

            dpval2.sy = 0;
            dpval2.sheight = CHUNKHEIGHT;

            if (innerloop == ULx && (dpval.sx % CHUNKWIDTH))
            {
                dpval2.sx = dpval.sx;
                dpval2.swidth -= dpval.sx;
            }

            if (innerloop == (LRx - 1) && ((dpval.sx + dpval.swidth) % CHUNKWIDTH))
                dpval2.swidth = ((dpval.sx + dpval.swidth) % CHUNKWIDTH) - dpval2.sx;

            if (outerloop == ULy && (dpval.sy % CHUNKHEIGHT))
            {
                dpval2.sy = dpval.sy;
                dpval2.sheight -= dpval.sy;
            }

            if (outerloop == (LRy - 1) && ((dpval.sy + dpval.sheight) % CHUNKHEIGHT))
                dpval2.sheight = ((dpval.sy + dpval.sheight) % CHUNKHEIGHT) - dpval2.sy;

            dpval2.dwidth  = dpval2.swidth;
            dpval2.dheight = dpval2.sheight;

            dbval2.source = ChunkCache.AddChunk(hdr->block[outerloop * width + innerloop].ptr(), 1);
            
            if (dbval2.source == nullptr)
                goto endloop;

            if (dpval2.drawmode & DM_ZBUFFER)
            {
                if (dbval.szbuffer && dbval.dzbuffer)
                    dbval2.szbuffer = (uint16_t *)ChunkCache.AddChunkZ(zhdr->block[outerloop * width + innerloop].ptr(), 2);

                if (dbval2.szbuffer == nullptr) 
                    goto endloop;

                if (dbval2.szbuffer == nullptr || dbval2.dzbuffer == nullptr)
                    dpval2.drawmode &= ~DM_ZBUFFER;
            }

            if (dpval2.drawmode & DM_NORMALS)
            {
                if (dbval.snormals && dbval.dnormals)
                    dbval2.snormals = (uint16_t *)ChunkCache.AddChunk16(nhdr->block[outerloop * width + innerloop].ptr(), 1);

                if (dbval2.snormals == nullptr || dbval2.dnormals == nullptr)
                    dpval2.drawmode &= ~DM_NORMALS;
            }

            dpval2.drawmode |= DM_NOCLIP;

            Draw(&dbval2, &dpval2);

        endloop:
            dpval2.dx += dpval2.dwidth;
        }

        dpval2.dx = dpval.dx;
        dpval2.dy += dpval2.dheight;
    }

    return true;
}

// *********** Normal Transparent Bitmap Transfer Routine *********
bool TransPut(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return ClipDecompress(db, dp, true);
//
//      else
//          return Decompress(db, dp, true);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dpval.drawmode]

        and  ebx, DM_REVERSEHORZ
        jne  reverse

    forloop:
        mov  ecx, [dpval.swidth]

        test ecx, 1
        je   notodd

        mov  ax, [esi]
        add  esi, 2
        or   ax, ax
        je   nopixel
        mov  [edi], ax
             
    nopixel:
        add  edi, 2

    notodd:
        shr  ecx, 1
        or   ecx, ecx
        je   newline

    floop:
        mov  eax, [esi]      
        add  esi, 4
        or   eax, eax                ; Skips both pixels if both transparent
        je   skip

        cmp  eax, 0000ffffh          ; Checks to see if upper pixel
        jbe  drawbottom              ; transparent

        or   ax, ax                  ; Check if lower pixel needs to be
        jne  drawboth                ; retrieved from screen

        mov  ax, [edi]

        mov  [edi], eax              ; Moves both pixels
        add  edi, 4
        dec  ecx
        jne  floop
        jmp  newline

    drawbottom:
        __emit 66h
    drawboth:
        mov  [edi], eax              ; Moves both pixels
    skip:
        add  edi, 4
        dec  ecx
        jne  floop

    newline:
        add  esi, srcadd
        add  edi, dstadd
        dec  edx
        jne  forloop
        jmp  done

    reverse:
        add  esi, [bmwidth]
             
        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

    revloop:
        mov  ecx, [dpval.swidth]

        test ecx, 1
        je   revnotodd

        mov  ax,[esi]
        sub  esi, 2
        or   ax, ax
        je   revnopixel

        mov  [edi], ax

    revnopixel:
        add  edi, 2

    revnotodd:
        shr  ecx, 1

    rloop:
        mov  eax, [esi]      
        sub  esi, 4
        rol  eax,16
        or   eax, eax                ; Skips both pixels if both transparent
        je   revskip
        
        cmp  eax, 0000ffffh          ; Checks to see if upper pixel
        jbe  revdrawbottom           ; transparent
        or   ax, ax                  ; Check if lower pixel needs to be
        jne  revdrawboth             ; retrieved from screen

        mov  ax, [edi]

        mov  [edi], eax              ; Moves both pixels
        add  edi, 4
        dec  ecx
        jne  rloop

        jmp  revnewline

    revdrawbottom:
        _emit 66h                    ; Cause instruction to be changed to
    revdrawboth:                     ; mov [edi], ax
        mov  [edi], eax              ; Moves both pixels
    revskip:
        add  edi, 4

        dec  ecx
        jne  rloop

    revnewline:
        add  esi, srcadd
        add  edi, dstadd
        dec  edx
        jne  revloop
     
    done:
    }
    #endif
    return true;
}

bool TransPut8(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette + sizeof(uint16_t) * 256);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return false; //ClipDecompress(db, dp, true);
//
//      else
//          return false; //Decompress(db, dp, true);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, [srcoff]

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, [dstoff]

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dpval.drawmode]

        and  ebx, DM_REVERSEHORZ
        jne  reverse

    forloop:
        mov  ecx, [dpval.swidth]

    floop:
        mov  al, [esi]
        and  eax, 0ffh

        or   al, al
        je   skip

        shl  eax, 2
        add  eax, [palette]
        mov  eax, [eax]

        mov  [edi], eax

    skip:
        inc  esi
        add  edi, 4

        dec  ecx
        jne  floop

        add  edi, [dstadd]
        add  esi, [srcadd]

        dec  edx
        jne  forloop

        jmp  done

    reverse:
        mov  eax, [bmwidth]
        add  esi, eax
             
        shl  eax, 1
        add  eax, [srcadd]
        mov  [srcadd], eax

    revloop:
        mov  ecx, [dpval.swidth]

    rloop:
        mov  al, [esi]
        and  eax, 0ffh

        or   al, al
        je   revskip

        shl  eax, 2
        add  eax, [palette]
        mov  eax, [eax]

        mov  [edi], eax

    revskip:
        dec  esi
        add  edi, 4

        dec  ecx
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd

        dec  edx
        jne  revloop

    done:
    }
    #endif
    return true;
}

bool TransZStaticPut8(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->dzbuffer == nullptr)
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette + sizeof(uint16_t) * 256);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
//          db->sbufheight != dp->sheight)
//          return false;//ClipZBufferDecompress832(db, dp, palette);
//      
//      else
//          return false;//ZBufferDecompress832(db, dp, palette);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW

    uint32_t tmpesp;   // Stores Temp ESP

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov  [tmpesp], esp

        mov  esi, [dbval.source]    ; Point ESI to source
        add  esi, [srcoff]          ; Adjusts ESI for offset

        mov  edi, [dbval.dest]      ; Point EDI to destination
        add  edi, [dstoff]          ; Adjusts EDI for offset
 
        mov  edx, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
        add  edx, [dstzoff]         ; Adjusts EDX for offset

        cld                         ; Forward direction
        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  Reverse

        mov  ebx, [bmheight]

    OuterLoop:
        mov  ecx, [dpval.swidth]    ; ECX contains number of rows to do.

    InnerLoop:
        mov  ax, [dpval.zpos]

        cmp  [edx], ax
        jb   SkipRaw

        xor  eax, eax
        mov  al, [esi]

        or   eax, eax
        jz   SkipRaw

        shl  eax, 2
        add  eax, [palette]
        
        mov  eax, [eax]
        shr  eax, 3
        mov  [edi], eax

        mov  ax, [dpval.zpos]
        mov  [edx], ax

    SkipRaw:
        inc  esi
        add  edi, 4
        add  edx, 2

        dec  ecx
        jne  InnerLoop

        add  esi, srcadd
        add  edi, dstadd
        add  edx, dstzadd

        dec  ebx

        jne  OuterLoop
        jmp  Done

    Reverse:
        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax
        mov  ebx, [bmheight]

    RevOuterLoop:
        mov  ecx, [dpval.swidth]    ; ECX contains number of rows to do.

    RevInnerLoop:
        mov  ax, [dpval.zpos]

        cmp  [edx], ax
        jb   RevSkipRaw

        xor  eax, eax
        mov  al, [esi]

        or   eax, eax
        jz   RevSkipRaw

        shl  eax, 2
        add  eax, [palette]

        mov  eax, [eax]
        mov  [edi], eax

        mov  ax, [dpval.zpos]
        mov  [edx], ax

    RevSkipRaw:
        dec  esi
        add  edi, 4
        dec  ebx
        add  edx, 2

        dec  ecx
        jne  RevInnerLoop

        add  esi, srcadd
        add  edi, dstadd
        add  edx, dstzadd

        dec  ebx
        jne  RevOuterLoop

    Done:
        mov esp, [tmpesp]
    }
    #endif

    return true;
}

bool TransZStaticPut816(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->dzbuffer == nullptr)
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
//          db->sbufheight != dp->sheight)
//          return false;//ClipZBufferDecompress832(db, dp, palette);
//      
//      else
//          return false;//ZBufferDecompress832(db, dp, palette);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW

    uint32_t tmpesp;   // Stores Temp ESP

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov  [tmpesp], esp

        mov  esi, [dbval.source]    ; Point ESI to source
        add  esi, [srcoff]          ; Adjusts ESI for offset

        mov  edi, [dbval.dest]      ; Point EDI to destination
        add  edi, [dstoff]          ; Adjusts EDI for offset
 
        mov  edx, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
        add  edx, [dstzoff]     ; Adjusts EDX for offset

        cld                         ; Forward direction
        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  Reverse

        mov  ebx, [bmheight]

    OuterLoop:
        mov  ecx, [dpval.swidth]    ; ECX contains number of rows to do.

    InnerLoop:
        mov  ax, [dpval.zpos]

        cmp  [edx], ax
        jb   SkipRaw

        xor  eax, eax
        mov  al, [esi]

        or   eax, eax
        jz   SkipRaw

        shl  eax, 1
        add  eax, [palette]

        mov  ax, [eax]
        mov  [edi], ax

        mov  ax, [dpval.zpos]
        mov  [edx], ax

    SkipRaw:
        inc  esi
        add  edi, 2
        add  edx, 2

        dec  ecx
        jne  InnerLoop

        add  esi, srcadd
        add  edi, dstadd
        add  edx, dstzadd

        dec  ebx

        jne  OuterLoop
        jmp  Done

    Reverse:
        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax
        mov  ebx, [bmheight]

    RevOuterLoop:
        mov  ecx, [dpval.swidth]    ; ECX contains number of rows to do.

    RevInnerLoop:
        mov  ax, [dpval.zpos]

        cmp  [edx], ax
        jb   RevSkipRaw

        xor  eax, eax
        mov  al, [esi]

        or   eax, eax
        jz   RevSkipRaw

        shl  eax, 1
        add  eax, [palette]

        mov  ax, [eax]
        mov  [edi], ax

        mov  ax, [dpval.zpos]
        mov  [edx], ax

    RevSkipRaw:
        dec  esi
        add  edi, 2
        dec  ebx
        add  edx, 2

        dec  ecx
        jne  RevInnerLoop

        add  esi, srcadd
        add  edi, dstadd
        add  edx, dstzadd

        dec  ebx
        jne  RevOuterLoop

    Done:
        mov esp, [tmpesp]
    }
    #endif

    return true;
}

bool TransPut816(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return false; //ClipDecompress(db, dp, true);
//
//      else
//          return false; //Decompress(db, dp, true);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dpval.drawmode]

        and  ebx, DM_REVERSEHORZ
        jne  reverse

    forloop:
        mov  ecx, [dpval.swidth]

    floop:
        xor  eax, eax
        mov  al, [esi]
        or   al, al
        je   skip

        shl  eax, 1
        add  eax, [palette]
        mov  ax, [eax]

        mov  [edi], ax

    skip:
        inc  esi
        add  edi, 2

        dec  ecx
        jne  floop

        add  esi, srcadd
        add  edi, dstadd
        dec  edx
        jne  forloop
        jmp  done

    reverse:
        add  esi, [bmwidth]
             
        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

    revloop:
        mov  ecx, [dpval.swidth]

    rloop:
        xor  eax, eax
        mov  al, [esi]
        shl  eax, 1
        add  eax, [palette]
        mov  ax, [eax]

        or   ax, ax
        je   revskip

        mov  [edi], ax

    revskip:
        dec  esi
        add  edi, 2

        dec  ecx
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd
        dec  edx
        jne  revloop

    done:
    }
    #endif
    return true;
}

bool TransPut88(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return false; //ClipDecompress(db, dp, true);
//
//      else
//          return false; //Decompress(db, dp, true);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dpval.drawmode]

        and  ebx, DM_REVERSEHORZ
        jne  reverse

    forloop:
        mov  ecx, [dpval.swidth]

    floop:
        mov  al, [esi]
        or   al, al
        je   skip

        mov  [edi], al

    skip:
        inc  esi
        inc  edi

        dec  ecx
        jne  floop

        add  esi, srcadd
        add  edi, dstadd
        dec  edx
        jne  forloop
        jmp  done

    reverse:
        add  esi, [bmwidth]
             
        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

    revloop:
        mov  ecx, [dpval.swidth]

    rloop:
        mov  al, [esi]
        or   al, al
        je   revskip

        mov  [edi], al

    revskip:
        dec  esi
        dec  edi

        dec  ecx
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd
        dec  edx
        jne  revloop

    done:
    }
    #endif
    return true;
}

bool TransPutColor(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return ClipDecompress(db, dp, true);
//
//      else
//          return Decompress(db, dp, true);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    uint16_t color = (uint16_t)dp->color;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dpval.drawmode]

        and  ebx, DM_REVERSEHORZ
        jne  reverse

    forloop:
        mov  ecx, [dpval.swidth]

    floop:
        mov  ax, [esi]
        add  esi, 2
        or   ax, ax
        je   skip

        mov  ax, color
        mov  [edi], ax

    skip:
        add  edi, 2
        dec  ecx
        jne  floop

        add  esi, srcadd
        add  edi, dstadd
        dec  edx
        jne  forloop
        jmp  done

    reverse:
        add  esi, [bmwidth]
             
        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

    revloop:
        mov  ecx, [dpval.swidth]

    rloop:
        mov  ax, [esi]
        sub  esi, 2
        or   ax, ax
        je   revskip
        
        mov  ax, color
        mov  [edi], ax

    revskip:
        add  edi, 2
        dec  ecx
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd
        dec  edx
        jne  revloop
     
    done:
    }
    #endif
    return true;
}

bool TransPutSVChange(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return ClipDecompress(db, dp, true);
//
//      else
//          return Decompress(db, dp, true);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    int32_t saturation = (dp->color >> 8) & 0xFF;
    int32_t brightness = dp->color & 0xFF;
    uint16_t pixel;

    srcoff >>= 1;
    dstoff >>= 1;
    srcadd >>= 1;
    dstadd >>= 1;
    uint16_t *src = (uint16_t *)dbval.source + srcoff;
    uint16_t *dst = (uint16_t *)dbval.dest + dstoff;
    for (int32_t h = 0; h < dpval.sheight; h++)
    {
        for (int32_t w = 0; w < dpval.swidth; w++)
        {
            pixel = *src++;

            if (pixel == db->keycolor)
            {
                pixel = *dst;
                *dst++ = pixel;
            }
            else if (pixel == 0)
                *dst++ = 0;
            else
            {
                int32_t r, g, b;
                if (Display->BitsPerPixel() == 15)
                {
                    r = (pixel >> 10) & 0x1F;
                    g = (pixel >> 5) & 0x1F;
                    b = pixel & 0x1F;
                }
                else
                {
                    r = (pixel >> 11) & 0x1F;
                    g = (pixel >> 6) & 0x1F;
                    b = pixel & 0x1F;
                }

                int32_t upper = 255 - (brightness / 8);
                int32_t lower = brightness / 8;
                if (upper > lower)
                {
                    r = (r * (upper - lower)) / 255 + lower;
                    b = (b * (upper - lower)) / 255 + lower;
                    g = (g * (upper - lower)) / 255 + lower;
                }

                r <<= 3;
                g <<= 3;
                b <<= 3;

                int32_t minval = min(r, min(g, b));
                int32_t maxval = max(r, max(g, b));
                double hue;

                int32_t delta = maxval - minval;
                if (delta <= 0)
                {
                    // for right now, ignore any grey pixels
                /*
                    // all three are equal - no saturation or hue
                    r -= brightness;
                    if (r < 0)
                        r = 0;
                    g = b = r;*/
                }
                else
                {
                    if (r == maxval)
                        hue = ((double)g - (double)b) / (double)delta;
                    else if (g == maxval)
                        hue = 2.0 + (((double)b - (double)r) / (double)delta);
                    else
                        hue = 4.0 + (((double)r - (double)g) / (double)delta);

                    hue *= 60.0;
                    if (hue < 0)
                        hue = hue + 360.0;

                    double s = (double)((((double)maxval - (double)minval) / (double)maxval) - ((double)saturation / 255.0));
                    if (s < 0.0)
                        s = 0.0;
                    double v = (double)(((double)maxval / 255.0) - ((double)brightness / 255.0));
                    if (v < 0.0)
                        v = 0.0;

                    int32_t range = (int32_t)(hue / 60.0);
                    double h = hue / 60.0;
                    double f = h - (double)range;
                    int32_t p = (int32_t)((v * (1 - s)) * 255.0);
                    int32_t q = (int32_t)((v * (1 - (s * f))) * 255.0);
                    int32_t t = (int32_t)((v * (1 - (s * (1 - f)))) * 255.0);
                    int32_t v0 = (int32_t)(v * 255.0);

                    switch (range)
                    {
                        case 0:
                            r = v0;
                            g = t;
                            b = p;
                            break;
                        case 1:
                            r = q;
                            g = v0;
                            b = p;
                            break;
                        case 2:
                            r = p;
                            g = v0;
                            b = t;
                            break;
                        case 3:
                            r = p;
                            g = q;
                            b = v0;
                            break;
                        case 4:
                            r = t;
                            g = p;
                            b = v0;
                            break;
                        case 5:
                            r = v0;
                            g = p;
                            b = q;
                            break;
                    }
                }

                r >>= 3;
                g >>= 3;
                b >>= 3;

                if (Display->BitsPerPixel() == 15)
                    pixel = (r << 10) | (g << 5) | b;
                else
                    pixel = (r << 11) | (g << 6) | b;

                *dst++ = pixel;
            }
        }

        src += srcadd;
        dst += dstadd;
    }

    return true;
}

bool PutHueChange(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return ClipDecompress(db, dp, true);
//
//      else
//          return Decompress(db, dp, true);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    uint16_t hue = (uint16_t)dp->color;
    uint16_t pixel;

    srcoff >>= 1;
    dstoff >>= 1;
    srcadd >>= 1;
    dstadd >>= 1;
    uint16_t *src = (uint16_t *)dbval.source + srcoff;
    uint16_t *dst = (uint16_t *)dbval.dest + dstoff;
    for (int32_t h = 0; h < dpval.sheight; h++)
    {
    for (int32_t w = 0; w < dpval.swidth; w++)
    {
    pixel = *src++;

    if (pixel != 0)
    {
        int32_t r, g, b;
        if (Display->BitsPerPixel() == 15)
        {
            r = (pixel >> 10) & 0x1F;
            g = (pixel >> 5) & 0x1F;
            b = pixel & 0x1F;
        }
        else
        {
            r = (pixel >> 11) & 0x1F;
            g = (pixel >> 6) & 0x1F;
            b = pixel & 0x1F;
        }

        r <<= 3;
        g <<= 3;
        b <<= 3;

        if (g > r && g > b)
        {
            double v = (double)g / 255.0;
            double s = ((double)g - (double)(min(r, b))) / (double)g;

            int32_t range = hue / 60;
            double h = (double)hue / 60.0;
            double f = h - (double)range;
            int32_t p = (int32_t)((v * (1 - s)) * 255.0);
            int32_t q = (int32_t)((v * (1 - (s * f))) * 255.0);
            int32_t t = (int32_t)((v * (1 - (s * (1 - f)))) * 255.0);
            int32_t v0 = (int32_t)(v * 255.0);

            switch (range)
            {
                case 0:
                    r = v0;
                    g = t;
                    b = p;
                    break;
                case 1:
                    r = q;
                    g = v0;
                    b = p;
                    break;
                case 2:
                    r = p;
                    g = v0;
                    b = t;
                    break;
                case 3:
                    r = p;
                    g = q;
                    b = v0;
                    break;
                case 4:
                    r = t;
                    g = p;
                    b = v0;
                    break;
                case 5:
                    r = v0;
                    g = p;
                    b = q;
                    break;
            }

            r >>= 3;
            g >>= 3;
            b >>= 3;

            if (Display->BitsPerPixel() == 15)
                pixel = (r << 10) | (g << 5) | b;
            else
                pixel = (r << 11) | (g << 6) | b;
        }
    }

    if (pixel == 0 && dpval.drawmode & DM_TRANSPARENT)
        dst++;
    else
        *dst++ = pixel;
    }

    src += srcadd;
    dst += dstadd;
    }

    return true;
}

// Non-zero keycolor transparent put
bool TransPutKey(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return ClipDecompress(db, dp, true);
//
//      else
//          return Decompress(db, dp, true);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    uint16_t keycolor = (uint16_t) dbval.keycolor;
    uint32_t dblkeycolor = (keycolor << 16) | keycolor;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dpval.drawmode]

        and  ebx, DM_REVERSEHORZ
        jne  reverse

    forloop:
        mov  ecx, [dpval.swidth]

        test ecx, 1
        je   notodd

        mov  ax, [esi]
        add  esi, 2
        cmp  ax, keycolor
        je   nopixel
        mov  [edi], ax
             
    nopixel:
        add  edi, 2

    notodd:
        shr  ecx, 1
        or   ecx, ecx
        je   newline

    floop:
        mov  eax, [esi]      
        add  esi, 4
        cmp  eax, dblkeycolor        ; Skips both pixels if both transparent
        je   skip

        mov  ebx, eax
        shr  ebx, 16
        cmp  bx, keycolor            ; Checks to see if upper pixel
        je   drawbottom              ; transparent

        cmp  ax, keycolor            ; Check if lower pixel needs to be
        jne  drawboth                ; retrieved from screen

        mov  ax, [edi]

        mov  [edi], eax              ; Moves both pixels
        add  edi, 4
        dec  ecx
        jne  floop
        jmp  newline

    drawbottom:
        mov  bx, [edi+2]
        shl  ebx, 16
        and  eax, 0000ffffh
        add  eax, ebx
        mov  [edi], eax              ; Moves both pixels

        add  edi, 4
        dec  ecx
        jne  floop
        jmp  newline

    drawboth:
        mov  [edi], eax              ; Moves both pixels
    skip:
        add  edi, 4
        dec  ecx
        jne  floop

    newline:
        add  esi, srcadd
        add  edi, dstadd
        dec  edx
        jne  forloop
        jmp  done

    reverse:
        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

    revloop:
        mov  ecx, [dpval.swidth]

        test ecx, 1
        je   revnotodd

        mov  ax,[esi]
        sub  esi, 2
        cmp  ax, keycolor
        je   revnopixel

        mov  [edi], ax

    revnopixel:
        add  edi, 2

    revnotodd:
        shr  ecx, 1

    rloop:
        mov  eax, [esi]      
        sub  esi, 4
        rol  eax,16
        cmp  eax, dblkeycolor        ; Skips both pixels if both transparent
        je   revskip
        
        mov  ebx, eax
        shr  ebx, 16
        cmp  bx, keycolor            ; Checks to see if upper pixel
        je   revdrawbottom           ; transparent
        cmp  ax, keycolor            ; Check if lower pixel needs to be
        jne  revdrawboth             ; retrieved from screen

        mov  ax, [edi]

        mov  [edi], eax              ; Moves both pixels
        add  edi, 4
        dec  ecx
        jne  rloop

        jmp  revnewline

    revdrawbottom:
        mov  bx, [edi+2]
        shl  ebx, 16
        add  eax, ebx
        mov  [edi], eax

        add  edi, 4
        dec  ecx
        jne  rloop

        jmp  revnewline

    revdrawboth:
        mov  [edi], eax              ; Moves both pixels
    revskip:
        add  edi, 4

        dec  ecx
        jne  rloop

    revnewline:
        add  esi, srcadd
        add  edi, dstadd
        dec  edx
        jne  revloop
     
    done:
    }
    #endif
    return true;
}

// *********** 32-bit Transparent Bitmap Transfer Routine *********
bool TransPut32(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
            db->sbufheight != dp->sheight)
            return false;

        else 
            return false;
    }

    if (db->dstbitmapflags & BM_32BIT)
    {
        SDrawBlock dbval = *db;
        SDrawParam dpval = *dp;

        SETUP_DRAW
    
        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            mov  esi, [dbval.source]    ; Point ESI to source
            add  esi, [srcoff]          ; Adjusts ESI for offset

            mov  edi, [dbval.dest]      ; Point EDI to destination
            add  edi, [dstoff]          ; Adjusts EDI for offset
 
            mov  edx, [dpval.sheight]
            mov  ebx, [dpval.drawmode]

            and  ebx, DM_REVERSEHORZ
            jne  Reverse32

            xor  ebx, ebx

        OuterLoop32:
            mov  ecx, [dpval.swidth]
        
        InnerLoop32:
            cmp  uint16_t ptr [esi], 0 
            je   SkipPixel32

            mov  bl, [esi]
            mov  eax, [Conv16to32Lower + ebx * 4]
            mov  bl, [esi + 1]
            add  eax, [Conv16to32Upper + ebx * 4]

            mov  [edi], eax

        SkipPixel32:
            add  esi, 2
            add  edi, 4
            
            dec  ecx
            jne  InnerLoop32
            
            add  esi, srcadd
            add  edi, dstadd

            dec  edx
            jne  OuterLoop32
            
            jmp  Done32

        Reverse32:
            xor  ebx, ebx

            add  esi, [bmwidth]
            mov  eax, [bmwidth]
            shl  eax, 1
            add  eax, srcadd
            mov  srcadd, eax
    
        RevOuterLoop32:
            mov     ecx, [dpval.swidth]     ; Do two pixels at a time backwords

        RevInnerLoop32:
            cmp  uint16_t ptr [esi], 0
            je   RevSkipPixel32

            mov  bl, [esi]
            mov  eax, [Conv16to32Lower + ebx * 4]
            mov  bl, [esi]
            add  eax, [Conv16to32Upper + ebx * 4]

            mov  [edi], eax

        RevSkipPixel32:
            sub  esi, 2
            add  edi, 4
            
            dec  ecx
            jne  RevInnerLoop32
            
            add  esi, srcadd
            add  edi, dstadd
            dec  edx
            jne  RevOuterLoop32

        Done32:
        }
        #endif
    }

    if (db->dstbitmapflags & BM_24BIT)
    {
        SDrawBlock dbval = *db;
        SDrawParam dpval = *dp;

        SETUP_DRAW
    
        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            mov  esi, [dbval.source]    ; Point ESI to source
            add  esi, [srcoff]          ; Adjusts ESI for offset

            mov  edi, [dbval.dest]      ; Point EDI to destination
            add  edi, [dstoff]          ; Adjusts EDI for offset
 
            mov  edx, [dpval.sheight]
            mov  ebx, [dpval.drawmode]

            and  ebx, DM_REVERSEHORZ
            jne  Reverse24

            xor  ebx, ebx

        OuterLoop24:
            mov  ecx, [dpval.swidth]
        
        InnerLoop24:
            cmp  uint16_t ptr [esi], 0
            je   SkipPixel24

            mov  bl, [esi]
            mov  eax, [Conv16to32Lower + ebx * 4]
            mov  bl, [esi + 1]
            add  eax, [Conv16to32Upper + ebx * 4]

        SkipPixel24:            
            add  esi, 2
            add  edi, 3
            
            dec  ecx
            jne  InnerLoop24
            
            add  esi, srcadd
            add  edi, dstadd

            dec  edx
            jne  OuterLoop24
            
            jmp  Done24

        Reverse24:
            xor  ebx, ebx

            add  esi, [bmwidth]
            mov  eax, [bmwidth]
            shl  eax, 1
            add  eax, srcadd
            mov  srcadd, eax
    
        RevOuterLoop24:
            mov     ecx, [dpval.swidth]     ; Do two pixels at a time backwords

        RevInnerLoop24:
            cmp  uint16_t ptr [esi], 0
            je   RevSkipPixel24

            mov  bl, [esi]
            mov  eax, [Conv16to32Lower + ebx * 4]
            mov  bl, [esi + 1]
            add  eax, [Conv16to32Upper + ebx * 4]

        RevSkipPixel24:
            sub  esi, 2
            add  edi, 3
            
            dec  ecx
            jne  RevInnerLoop24
            
            add  esi, srcadd
            add  edi, dstadd
            dec  edx
            jne  RevOuterLoop24

        Done24:
        }
        #endif
    }

    return true;
}

bool Stretch(PSDrawBlock db, PSDrawParam dp)
{
    return false;
}

bool TransStretch(PSDrawBlock db, PSDrawParam dp)
{
    return false;
}

// ********** Masked Bitmap Transfer Routines ***********

bool Mask(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    uint32_t tmpecx;
    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines
        mov  edx, [dpval.sheight]
        mov  ebx, [dbval.szbuffer]   ; Set ebx to point to Source Z-Buffer

        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  reverse

    forloop:
        mov  ecx, [dpval.swidth]
        shr  ecx, 1
    floop:
        mov  [tmpecx], ecx

        mov  ecx, [tmpecx]
        dec  ecx
        jne  floop

        jne  forloop

        dec  edx
        jne  forloop

        jmp  done

    reverse:
        add  esi, bmwidth            ; since were now going backwards
        dec  esi                     ; Start 1 pixel to left
        mov  eax, [dpval.swidth]     ; Add two srcwidths from srcadd
        add  eax, eax
        add  srcadd, eax

    revloop:
        mov  edx, [dpval.swidth]
        mov  [tmpecx], ecx
    rloop:
        mov  ecx, [tmpecx]
        dec  ecx
        jne  rloop

        dec  edx
        jne  revloop

    done:
    }
    #endif
    return true;
}

bool MaskStretch(PSDrawBlock db, PSDrawParam dp)
{
    return false;
}

bool Translucent(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    uint32_t tmpebx;
    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines

        xor  edx, edx
        xor  ecx, ecx

        mov  ch, uint8_t PTR [dpval.intensity]
        mov  dh, 31
        sub  dh, uint8_t PTR [dpval.intensity]
        mov  ebx, [dpval.drawmode]

        and  ebx, DM_REVERSEHORZ
        jne  reverse

        mov  ebx, [dpval.sheight]

    forloop:
        mov  [tmpebx], ebx
        mov  ebx, [dpval.swidth]
    
    floop:
        or   dh, dh
        jne  NotZero
        mov  uint16_t ptr [edi], 0
        jmp  ZeroIntensity
    
    NotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax
        
    ZeroIntensity:
        or   ch, ch
        je   ZeroIntensity2
        mov  cl, uint8_t PTR [esi + 1]
        mov  ah, [ColorTableUpperHi + ecx]
        mov  al, [ColorTableUpperLo + ecx]
        mov  cl, uint8_t PTR [esi]
        add  al, [ColorTableLower + ecx]
        adc  ah, 0
        add  [edi], ax

    ZeroIntensity2:
        add  edi, 2
        add  esi, 2
                            
        dec  ebx
        jne  floop

        add  esi, srcadd
        add  edi, dstadd

        mov  ebx, [tmpebx]
        dec  ebx
        jne  forloop
        jmp  done   

    reverse:
        mov  ebx, [dpval.sheight]
        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

    revloop:
        mov  [tmpebx], ebx
        mov  ebx, [dpval.swidth]

    rloop:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax
        
        mov  cl, uint8_t PTR [esi + 1]
        mov  ah, [ColorTableUpperHi + ecx]
        mov  al, [ColorTableUpperLo + ecx]
        mov  cl, uint8_t PTR [esi]
        add  al, [ColorTableLower + ecx]
        adc  ah, 0
        add  [edi], ax

        add  edi, 2
        sub  esi, 2
                            
        dec  ebx
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd
        mov  ebx, [tmpebx]
        dec  ebx
        jne  revloop

    done:
    }
    #endif
    return true;
}

bool TransTranslucent(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    uint32_t tmpebx;
    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines

        xor  edx, edx
        xor  ecx, ecx

        mov  ch, uint8_t PTR [dpval.intensity]
        mov  dh, 31
        sub  dh, uint8_t PTR [dpval.intensity]

        mov  ebx, [dpval.drawmode]

        and  ebx, DM_REVERSEHORZ
        jne  reverse

        mov  ebx, [dpval.sheight]

    forloop:
        mov  [tmpebx], ebx
        mov  ebx, [dpval.swidth]
    
    floop:
        cmp  uint16_t ptr [esi], 0
        je   ZeroIntensity2

        or   dh, dh
        jne  NotZero
        mov  uint16_t ptr [edi], 0
        jmp  ZeroIntensity
    
    NotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax
        
    ZeroIntensity:
        or   ch, ch
        je   ZeroIntensity2
        mov  cl, uint8_t PTR [esi + 1]
        mov  ah, [ColorTableUpperHi + ecx]
        mov  al, [ColorTableUpperLo + ecx]
        mov  cl, uint8_t PTR [esi]
        add  al, [ColorTableLower + ecx]
        adc  ah, 0
        add  [edi], ax

    ZeroIntensity2:
        add  edi, 2
        add  esi, 2
                            
        dec  ebx
        jne  floop

        add  esi, srcadd
        add  edi, dstadd

        mov  ebx, [tmpebx]
        dec  ebx
        jne  forloop
        jmp  done   

    reverse:
        mov  ebx, [dpval.sheight]
        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

    revloop:
        mov  [tmpebx], ebx
        mov  ebx, [dpval.swidth]
    rloop:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax
        
        mov  cl, uint8_t PTR [esi + 1]
        mov  ah, [ColorTableUpperHi + ecx]
        mov  al, [ColorTableUpperLo + ecx]
        mov  cl, uint8_t PTR [esi]
        add  al, [ColorTableLower + ecx]
        adc  ah, 0
        add  [edi], ax

        sub  esi, 2
        add  edi, 2
                            
        dec  ebx
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd

        mov  ebx, [tmpebx]
        dec  ebx
        jne  revloop

    done:
    }
    #endif
    return true;
}

bool TranslucentStretch(PSDrawBlock db, PSDrawParam dp)
{
    return false;
}

bool AlphaLighten(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 4 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return false;
//  
//      else
//          return AlphaDecompress(db, dp);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW
    
    int32_t alphasrcoff = srcoff >> 1;
    int32_t alphasrcadd = srcadd >> 1;

    uint32_t tmpecx;
    uint32_t tmpesp;
    uint32_t outercounter;
    uint32_t innercounter;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov [tmpesp], esp
        cld
        xor  edx, edx

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines

        mov  esp, [dbval.alpha]
        add  esp, alphasrcoff

        mov  ecx, [dpval.drawmode]

        and  ecx, DM_REVERSEHORZ
        jne  reverse

        xor  ecx, ecx
        mov  eax, [dpval.sheight]
        mov  [outercounter], eax

    forloop:
        mov  eax, [dpval.swidth]
        mov  [innercounter], eax

    floop:
        xor  eax, eax
        mov  dh, [esp]
        or   dh, dh
        je   NoDraw

        mov  dl, uint8_t PTR [esi + 1]
        mov  al, [ColorTableUpperLo + edx]
        mov  ah, [ColorTableUpperHi + edx]
        mov  dl, uint8_t PTR [esi]
        add  al, [ColorTableLower + edx]
        adc  ah, 0

        neg  dh
        add  dh, 31
        je   PutIt

        mov  cl, uint8_t PTR [edi + 1] 
        mov  bl, [IntensityTableUpper + ecx]
        mov  dl, cl
        add  al, [ColorTableUpperLo + edx]
        adc  ah, [ColorTableUpperHi + edx]
        mov  cl, uint8_t PTR [edi]     
        add  bl, [IntensityTableLower + ecx]
        mov  dl, cl
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  cl, ah
        mov  dh, [IntensityTableUpper + ecx]
        mov  cl, al
        add  dh, [IntensityTableLower + ecx]

        cmp  dh, bl
        jb   NoDraw     

    PutIt:
        mov  [edi], ax

    NoDraw:
        add  edi, 2
        add  esi, 2
        inc  esp
                            
        dec  [innercounter]
        jne  floop

        add  esi, srcadd
        add  edi, dstadd
        add  esp, alphasrcadd

        dec  [outercounter]
        jne  forloop

        jmp  done   

    reverse:
        mov  ecx, [dpval.sheight]

        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

        mov  eax, srcadd
        shr  eax, 1
        mov  alphasrcadd, eax

    revloop:
        mov  [tmpecx], ecx
        mov  ecx, [dpval.swidth]

    rloop:
        mov  dh, 31
        sub  dh, [ebx]
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax
        
        mov  dh, [ebx]
        mov  dl, uint8_t PTR [esi + 1]
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [esi]
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax

        add  edi, 2
        sub  esi, 2
        dec  ebx                    

        dec  ecx
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd
        add  ebx, alphasrcadd

        mov  ecx, [tmpecx]
        dec  ecx
        jne  revloop

    done:
        mov esp, [tmpesp]
    }
    #endif
    return true;
}

bool Alpha(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 4 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return false;
//  
//      else
//          return AlphaDecompress(db, dp);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW
    
    int32_t alphasrcoff = srcoff >> 1;
    int32_t alphasrcadd = srcadd >> 1;

    uint32_t tmpecx;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld
        xor  edx, edx

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines

        mov  ebx, [dbval.alpha]
        add  ebx, alphasrcoff

        mov  ecx, [dpval.drawmode]

        and  ecx, DM_REVERSEHORZ
        jne  reverse

        mov  ecx, [dpval.sheight]

    forloop:
        mov  [tmpecx], ecx
        mov  ecx, [dpval.swidth]

    floop:
        mov  dh, 31
        sub  dh, [ebx]
        jnz  NotZero

        mov  uint16_t ptr [edi], 0
        jmp  ZeroIntensity1

    NotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax

    ZeroIntensity1:
        mov  dh, [ebx]
        or   dh, dh
        je   ZeroIntensity2

        mov  dl, uint8_t PTR [esi + 1]
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [esi]
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax

    ZeroIntensity2:
        add  edi, 2
        add  esi, 2
        inc  ebx
                            
        dec  ecx
        jne  floop

        add  esi, srcadd
        add  edi, dstadd
        add  ebx, alphasrcadd

        mov  ecx, [tmpecx]
        dec  ecx
        jne  forloop

        jmp  done   

    reverse:
        mov  ecx, [dpval.sheight]

        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

        mov  eax, srcadd
        shr  eax, 1
        mov  alphasrcadd, eax

    revloop:
        mov  [tmpecx], ecx
        mov  ecx, [dpval.swidth]

    rloop:
        mov  dh, 31
        sub  dh, [ebx]
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax
        
        mov  dh, [ebx]
        mov  dl, uint8_t PTR [esi + 1]
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [esi]
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax

        add  edi, 2
        sub  esi, 2
        dec  ebx                    

        dec  ecx
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd
        add  ebx, alphasrcadd

        mov  ecx, [tmpecx]
        dec  ecx
        jne  revloop

    done:
    }
    #endif
    return true;
}

bool Alpha8(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 4 || dp->dheight < 1) 
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return false;
//  
//      else
//          return AlphaDecompress8(db, dp, palette);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW
    
    int32_t alphasrcoff = srcoff >> 1;
    int32_t alphasrcadd = srcadd >> 1;

    uint32_t tmpecx;
    uint16_t  pixel16;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld
        xor  edx, edx

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines

        mov  ebx, [dbval.alpha]
        add  ebx, alphasrcoff

        mov  ecx, [dpval.drawmode]

        and  ecx, DM_REVERSEHORZ
        jne  reverse

        mov  ecx, [dpval.sheight]

    forloop:
        mov  [tmpecx], ecx
        mov  ecx, [dpval.swidth]

    floop:
        mov  dh, 31
        sub  dh, [ebx]
        jnz  NotZero

        mov  uint16_t ptr [edi], 0
        jmp  ZeroIntensity1

    NotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax

    ZeroIntensity1:
        mov  al, [esi]
        inc  esi

        and  eax, 000000ffh
        shl  eax, 1

        add  eax, [dbval.palette]
        mov  ax, [eax]
        mov  [pixel16], ax

        mov  dh, [ebx]
        or   dh, dh
        je   ZeroIntensity2

        mov  dl, uint8_t PTR [pixel16 + 1]
        mov  ah, [ColorTableUpperHi + edx]

        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [pixel16]

        add  al, [ColorTableLower + edx]
        adc  ah, 0

        add  [edi], ax

    ZeroIntensity2:
        add  edi, 2
        inc  ebx
                            
        dec  ecx
        jne  floop

        add  esi, srcadd
        add  edi, dstadd
        add  ebx, alphasrcadd

        mov  ecx, [tmpecx]
        dec  ecx
        jne  forloop

        jmp  done   

    reverse:
        mov  ecx, [dpval.sheight]

        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

        mov  eax, srcadd
        shr  eax, 1
        mov  alphasrcadd, eax

    revloop:
        mov  [tmpecx], ecx
        mov  ecx, [dpval.swidth]

    rloop:
        mov  dh, 31
        sub  dh, [ebx]
        jnz  revNotZero

        mov  uint16_t ptr [edi], 0
        jmp  revZeroIntensity1

    revNotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax

    revZeroIntensity1:
        mov  al, [esi]
        dec  esi

        and  eax, 000000ffh
        shl  eax, 1

        add  eax, [dbval.palette]
        mov  ax, [eax]
        mov  [pixel16], ax

        mov  dh, [ebx]
        or   dh, dh
        je   revZeroIntensity2

        mov  dl, uint8_t PTR [pixel16 + 1]
        mov  ah, [ColorTableUpperHi + edx]

        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [pixel16]

        add  al, [ColorTableLower + edx]
        adc  ah, 0

        add  [edi], ax

    revZeroIntensity2:
        add  edi, 2
        dec  ebx
                            
        dec  ecx
        jne  rloop

        add  esi, [srcadd]
        add  edi, [dstadd]

        add  ebx, [alphasrcadd]
        mov  ecx, [tmpecx]

        dec  ecx
        jne  revloop

    done:
    }
    #endif
    return true;
}

bool AlphaZ(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 4 || 
        dp->dheight < 1 || db->dzbuffer == nullptr) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return false;
//  
//      else
//          return AlphaStaticZDecompress(db, dp);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW
    
    int32_t alphasrcoff = srcoff >> 1;
    int32_t alphasrcadd = srcadd >> 1;

    uint32_t tmpecx;
    uint32_t tmpecx2;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld
        xor  edx, edx

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

        mov  ecx, [dbval.dzbuffer]
        add  ecx, dstzoff

  // Load lines

        mov  ebx, [dbval.alpha]
        add  ebx, alphasrcoff

        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  reverse

        mov  eax, [dpval.sheight]
        mov  [tmpecx], eax

    forloop:
        mov  eax, [dpval.swidth]
        mov  [tmpecx2], eax

    floop:
        mov  eax, [ecx]
        cmp  ax, [dpval.zpos]
        jb   ZeroIntensity2 
        
        mov  dh, 31
        sub  dh, [ebx]
        jnz  NotZero

        mov  uint16_t ptr [edi], 0
        jmp  ZeroIntensity1

    NotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax

    ZeroIntensity1:
        mov  dh, [ebx]
        or   dh, dh
        je   ZeroIntensity2

        mov  dl, uint8_t PTR [esi + 1]
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [esi]
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax

    ZeroIntensity2:
        add  edi, 2
        add  esi, 2
        
        add  ecx, 2
        inc  ebx
                            
        dec  [tmpecx2]
        jne  floop

        add  esi, srcadd
        add  edi, dstadd

        add  ecx,  dstzadd
        add  ebx, alphasrcadd
        dec [tmpecx]

        jne  forloop

        jmp  done   

    reverse:
        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

        mov  eax, srcadd
        shr  eax, 1
        mov  alphasrcadd, eax

        mov  eax, [dpval.sheight]
        mov  [tmpecx2], eax

    revloop:
        mov  eax, [dpval.swidth]
        mov  [tmpecx2], eax

    rloop:
        mov  eax, [ecx]
        cmp  ax, [dpval.zpos]
        jb   RevZeroIntensity2 
        
        mov  dh, 31
        sub  dh, [ebx]
        jnz  RevNotZero

        mov  uint16_t ptr [edi], 0
        jmp  RevZeroIntensity1

    RevNotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax
        
    RevZeroIntensity1:
        mov  dh, [ebx]
        or   dh, dh
        je   RevZeroIntensity2

        mov  dl, uint8_t PTR [esi + 1]
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [esi]
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax

    RevZeroIntensity2:
        add  edi, 2
        sub  esi, 2
        dec  ebx                    

        dec  ecx
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd
        add  ebx, alphasrcadd

        mov  ecx, [tmpecx]
        dec  ecx
        jne  revloop

    done:
    }
    #endif
    return true;
}

bool AlphaZ8(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 4 || 
        dp->dheight < 1 || db->dzbuffer == nullptr) 
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return false;
//  
//      else
//          return AlphaStaticZDecompress8(db, dp, palette);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW
    
    int32_t alphasrcoff = srcoff >> 1;
    int32_t alphasrcadd = srcadd >> 1;

    uint32_t tmpecx;
    uint32_t tmpecx2;
    uint16_t  pixel16;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld
        xor  edx, edx

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

        mov  ecx, [dbval.dzbuffer]
        add  ecx, dstzoff

  // Load lines

        mov  ebx, [dbval.alpha]
        add  ebx, alphasrcoff

        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  reverse

        mov  eax, [dpval.sheight]
        mov  [tmpecx], eax

    forloop:
        mov  eax, [dpval.swidth]
        mov  [tmpecx2], eax

    floop:
        mov  eax, [ecx]
        cmp  ax, [dpval.zpos]
        jb   ZeroIntensity2 
        
        mov  dh, 31
        sub  dh, [ebx]
        jnz  NotZero

        mov  uint16_t ptr [edi], 0
        jmp  ZeroIntensity1

    NotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax

    ZeroIntensity1:
        mov  al, [esi]
        inc  esi

        and  eax, 000000ffh
        shl  eax, 1

        add  eax, [dbval.palette]
        mov  ax, [eax]

        mov  [pixel16], ax
        mov  dh, [ebx]

        or   dh, dh
        je   ZeroIntensity2

        mov  dl, uint8_t PTR [pixel16 + 1]
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [pixel16]
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax

    ZeroIntensity2:
        add  edi, 2
        add  ecx, 2

        inc  ebx
        dec  [tmpecx2]

        jne  floop

        add  esi, srcadd
        add  edi, dstadd
        add  ecx, dstzadd

        add  ebx, alphasrcadd
        dec [tmpecx]

        jne  forloop

        jmp  done   

    reverse:
        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

        mov  eax, srcadd
        shr  eax, 1
        mov  alphasrcadd, eax

        mov  eax, [dpval.sheight]
        mov  [tmpecx2], eax

    revloop:
        mov  eax, [dpval.swidth]
        mov  [tmpecx2], eax

    rloop:
        mov  eax, [ecx]
        cmp  ax, [dpval.zpos]
        jb   RevZeroIntensity2 
        
        mov  dh, 31
        sub  dh, [ebx]
        jnz  RevNotZero

        mov  uint16_t ptr [edi], 0
        jmp  RevZeroIntensity1

    RevNotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax
        
    RevZeroIntensity1:
        mov  al, [esi]
        dec  esi

        and  eax, 000000ffh
        shl  eax, 1

        add  eax, [dbval.palette]
        mov  ax, [eax]

        mov  [pixel16], ax
        mov  dh, [ebx]

        or   dh, dh
        je   RevZeroIntensity2

        mov  dl, uint8_t PTR [pixel16 + 1]
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [pixel16]
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax

    RevZeroIntensity2:
        add  edi, 2
        dec  ebx                    

        dec  ecx
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd
        add  ebx, alphasrcadd
        add  ecx, dstzadd

        dec  [tmpecx]
        jne  revloop

    done:
    }
    #endif
    return true;
}

bool AlphaDim(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 4 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return false;
//  
//      else
//          return AlphaDecompress(db, dp);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW
    
    int32_t alphasrcoff = srcoff >> 1;
    int32_t alphasrcadd = srcadd >> 1;

    uint8_t modifier = (uint8_t)(31 - min(dp->intensity, 31));

    uint32_t tmpecx;
    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld
        xor  edx, edx

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines

        mov  ebx, [dbval.alpha]
        add  ebx, alphasrcoff

        mov  ecx, [dpval.drawmode]

        and  ecx, DM_REVERSEHORZ
        jne  reverse

        mov  ecx, [dpval.sheight]

    forloop:
        mov  [tmpecx], ecx
        mov  ecx, [dpval.swidth]

    floop:
        mov  dh, 31
        add  dh, [modifier]
        sub  dh, [ebx]
        cmp  dh, 31
        jle  nowrap
        mov  dh, 31             ; wrapped around, set to 31

    nowrap:
        or   dh, dh
        jnz  NotZero

        mov  uint16_t ptr [edi], 0
        jmp  ZeroIntensity1

    NotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax

    ZeroIntensity1:
        mov  dh, [ebx]
        sub  dh, [modifier]
        jnc  nowrap2
        mov  dh, 0              ; wrapped around

    nowrap2:
        or   dh, dh
        je   ZeroIntensity2

        mov  dl, uint8_t PTR [esi + 1]
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [esi]
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax

    ZeroIntensity2:
        add  edi, 2
        add  esi, 2
        inc  ebx
                            
        dec  ecx
        jne  floop

        add  esi, srcadd
        add  edi, dstadd
        add  ebx, alphasrcadd

        mov  ecx, [tmpecx]
        dec  ecx
        jne  forloop

        jmp  done   

    reverse:
        mov  ecx, [dpval.sheight]

        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

        mov  eax, srcadd
        shr  eax, 1
        mov  alphasrcadd, eax

    revloop:
        mov  [tmpecx], ecx
        mov  ecx, [dpval.swidth]

    rloop:
        mov  dh, 31
        add  dh, [modifier]
        sub  dh, [ebx]
        cmp  dh, 31
        jle  rnowrap
        mov  dh, 31             ; wrapped around, set to 31

    rnowrap:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax
        
        mov  dh, [ebx]
        sub  dh, [modifier]
        jnc  rnowrap2
        mov  dh, 0              ; wrapped around

    rnowrap2:
        mov  dl, uint8_t PTR [esi + 1]
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [esi]
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax

        add  edi, 2
        sub  esi, 2
        dec  ebx                    

        dec  ecx
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd
        add  ebx, alphasrcadd

        mov  ecx, [tmpecx]
        dec  ecx
        jne  revloop

    done:
    }
    #endif
    return true;
}

bool AlphaDimZ(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 4 || dp->dheight < 1
        || db->dzbuffer == nullptr) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
//          return false;
//  
//      else
//          return AlphaDecompress(db, dp);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW
    
    int32_t alphasrcoff = srcoff >> 1;
    int32_t alphasrcadd = srcadd >> 1;

    uint8_t modifier = (uint8_t)(31 - min(dp->intensity, 31));

    uint32_t tmpecx;
    uint32_t tmpecx2;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld
        xor  edx, edx

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

        mov  ecx, [dbval.dzbuffer]
        add  ecx, dstzoff

  // Load lines

        mov  ebx, [dbval.alpha]
        add  ebx, alphasrcoff

        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  reverse

        mov  eax, [dpval.sheight]
        mov  [tmpecx], eax

    forloop:
        mov  eax, [dpval.swidth]
        mov  [tmpecx2], eax

    floop:
        mov  eax, [ecx]
        cmp  ax, [dpval.zpos]
        jg   ZeroIntensity2

        mov  dh, 31
        add  dh, [modifier]
        sub  dh, [ebx]
        cmp  dh, 31
        jle  nowrap
        mov  dh, 31             ; wrapped around, set to 31

    nowrap:
        or   dh, dh
        jnz  NotZero

        mov  uint16_t ptr [edi], 0
        jmp  ZeroIntensity1

    NotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax

    ZeroIntensity1:
        mov  dh, [ebx]
        sub  dh, [modifier]
        jnc  nowrap2
        mov  dh, 0              ; wrapped around

    nowrap2:
        or   dh, dh
        je   ZeroIntensity2

        mov  dl, uint8_t PTR [esi + 1]
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [esi]
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax

    ZeroIntensity2:
        add  edi, 2
        add  esi, 2
        add  ecx, 2
        inc  ebx
                            
        dec  [tmpecx2]
        jne  floop

        add  esi, srcadd
        add  edi, dstadd
        add  ebx, alphasrcadd
        add  ecx, dstzadd

        dec [tmpecx]
        jne  forloop

        jmp  done   

    reverse:
        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax

        mov  eax, srcadd
        shr  eax, 1
        mov  alphasrcadd, eax

        mov  eax, [dpval.sheight]
        mov  [tmpecx], eax

    revloop:
        mov  eax, [dpval.swidth]
        mov  [tmpecx2], eax

    rloop:
        mov  eax, [ecx]
        cmp  ax, [dpval.zpos]
        jg   RevZeroIntensity2

        mov  dh, 31
        add  dh, [modifier]
        sub  dh, [ebx]
        cmp  dh, 31
        jle  rnowrap
        mov  dh, 31             ; wrapped around, set to 31

    rnowrap:
        or   dh, dh
        jnz  RevNotZero

        mov  uint16_t ptr [edi], 0
        jmp  RevZeroIntensity1

    RevNotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax
        
    
    RevZeroIntensity1:
        mov  dh, [ebx]
        sub  dh, [modifier]
        jnc  rnowrap2
        mov  dh, 0              ; wrapped around

    rnowrap2:
        or   dh, dh
        je   RevZeroIntensity2

        mov  dl, uint8_t PTR [esi + 1]
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [esi]
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax

    RevZeroIntensity2:
        add  edi, 2
        sub  esi, 2
        dec  ebx                    

        dec  [tmpecx2]
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd
        add  ebx, alphasrcadd
        add  ecx, dstzadd

        dec  [tmpecx]
        jne  revloop

    done:
    }
    #endif
    return true;
}

// This routine is designed for doing spider webs and glows: it draws an image with
// z and alpha info, but no bitmap (it just uses solid color for the pixel color).
bool AlphaDimZNoBitmap(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 4 || dp->dheight < 1 || db->dzbuffer == nullptr) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
        return false;

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW
    
    int32_t alphasrcoff = srcoff >> 1;
    int32_t alphasrcadd = srcadd >> 1;
    int32_t alphawidth = bmwidth >> 1;

    uint8_t modifier = (uint8_t)(31 - min(dp->intensity, 31));

    uint32_t tmpecx;
    uint32_t tmpecx2;

    uint8_t highbyte = (uint8_t)((dp->color >> 8) & 0xff);
    uint8_t lowbyte = (uint8_t)(dp->color & 0xff);

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld
        xor  edx, edx

  // Load source zbf
        mov  esi, [dbval.szbuffer]
        add  esi, srczoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

        mov  ecx, [dbval.dzbuffer]
        add  ecx, dstzoff

  // Load lines

        mov  ebx, [dbval.alpha]
        add  ebx, alphasrcoff

        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  reverse

        mov  eax, [dpval.sheight]
        mov  [tmpecx], eax

    forloop:
        mov  eax, [dpval.swidth]
        mov  [tmpecx2], eax

    floop:
        mov  ax, [esi]
        cmp  ax, 0x7f7f             ; check keycolor
        je   ZeroIntensity2

        add  ax, [dpval.zpos]       ; check against dest zbuf
        cmp  [ecx], ax
        jb   ZeroIntensity2

        mov  [ecx], ax              ; set it into the zbuffer

        mov  dh, 31
        add  dh, [modifier]
        sub  dh, [ebx]
        cmp  dh, 31
        jle  nowrap
        mov  dh, 31             ; wrapped around, set to 31

    nowrap:
        or   dh, dh
        jnz  NotZero

        mov  uint16_t ptr [edi], 0
        jmp  ZeroIntensity1

    NotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax

    ZeroIntensity1:
        mov  dh, [ebx]
        sub  dh, [modifier]
        jnc  nowrap2
        mov  dh, 0              ; wrapped around

    nowrap2:
        or   dh, dh
        je   ZeroIntensity2

        mov  dl, highbyte
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, lowbyte
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax

    ZeroIntensity2:
        add  edi, 2
        add  esi, 2
        add  ecx, 2
        inc  ebx
                            
        dec  [tmpecx2]
        jne  floop

        add  esi, srczadd
        add  edi, dstadd
        add  ecx, dstzadd
        add  ebx, alphasrcadd

        dec [tmpecx]
        jne  forloop

        jmp  done   

    reverse:
        add  esi, [bmwidth]
        add  ebx, [alphawidth]

        mov  eax, [dpval.sheight]
        mov  [tmpecx], eax

    revforloop:
        mov  eax, [dpval.swidth]
        mov  [tmpecx2], eax

    revfloop:
        sub  esi, 2
        dec  ebx

        mov  ax, [esi]
        cmp  ax, 0x7f7f             ; check keycolor
        je   revZeroIntensity2

        add  ax, [dpval.zpos]       ; check against dest zbuf
        cmp  [ecx], ax
        jb   revZeroIntensity2

        mov  [ecx], ax              ; set it into the zbuffer

        mov  dh, 31
        add  dh, [modifier]
        sub  dh, [ebx]
        cmp  dh, 31
        jle  revnowrap
        mov  dh, 31             ; wrapped around, set to 31

    revnowrap:
        or   dh, dh
        jnz  revNotZero

        mov  uint16_t ptr [edi], 0
        jmp  revZeroIntensity1

    revNotZero:
        mov  dl, uint8_t PTR [edi + 1] 
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]     
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax

    revZeroIntensity1:
        mov  dh, [ebx]
        sub  dh, [modifier]
        jnc  revnowrap2
        mov  dh, 0              ; wrapped around

    revnowrap2:
        or   dh, dh
        je   revZeroIntensity2

        mov  dl, highbyte
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, lowbyte
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax

    revZeroIntensity2:
        add  edi, 2
        add  ecx, 2
                            
        dec  [tmpecx2]
        jne  revfloop

        add  esi, bmwidth
        add  esi, bmwidth
        add  esi, srczadd

        add  ebx, alphawidth
        add  ebx, alphawidth
        add  ebx, alphasrcadd

        add  edi, dstadd
        add  ecx, dstzadd

        dec [tmpecx]
        jne  revforloop

    done:
    }
    #endif
    return true;
}

bool Alpha32(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 4 || dp->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || db->sbufheight != dp->sheight)
            return false;
    
        else
            return false;
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW
    
    int32_t alphasrcoff = srcoff >> 1;
    int32_t alphasrcadd = srcadd >> 1;

    uint32_t tmpecx;
    uint32_t tmpecx2;
    uint32_t color;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld
        xor  edx, edx

  // Load source
        mov  esi, [dbval.source]
        add  esi, srcoff

  // Load destination
        mov  edi, [dbval.dest]
        add  edi, dstoff

  // Load lines

        mov  ebx, [dbval.alpha]
        add  ebx, alphasrcoff

        mov  ecx, [dpval.drawmode]

        and  ecx, DM_REVERSEHORZ
        jne  reverse

        mov  eax, [dpval.sheight]
        mov  [tmpecx], eax

    forloop:
        mov  eax, [dpval.swidth]
        mov  [tmpecx2], eax

    floop:
        mov  dh, 31
        sub  dh, [ebx]
        jnz  NotZero

        mov  uint32_t ptr[edi], uint32_t ptr 0x00000000
        jmp  ZeroIntensity1

    NotZero:
        mov  eax, [edi]
        shr  eax, 16

        mov  dl, al                     // Do Blue
        mov  cl, [ColorTable + edx]

        mov  ch, ah
        shl  ecx, 16
        
        mov  eax, [edi]
        mov  dl, al                     // Do Green

        mov  cl, [ColorTable + edx]
        mov  dl, ah                     // Do Red

        mov  ch, [ColorTable + edx]
        mov  [edi], ecx

    ZeroIntensity1:
        mov  dh, [ebx]
        or   dh, dh

        je   ZeroIntensity2

        xor  ecx, ecx
        mov  cl, [esi]

        mov  eax, Conv16to32Lower[ecx]
        mov  cl, [esi + 1]

        add  eax, Conv16to32Upper[ecx]
        mov  [color], eax

        shr  eax, 16

        mov  dl, al                     // Do Green
        mov  cl, [ColorTable + edx]

        xor  ch, ch
        shl  ecx, 16

        mov  eax, [color] 
        mov  dl, al                     // Do Red

        mov  cl, [ColorTable + edx]
        mov  dl, ah                     // Do Blue

        mov  ch, [ColorTable + edx]
        add  [edi], ecx

    ZeroIntensity2:
        add  esi, 2
        add  edi, 4

        inc  ebx
        dec  [tmpecx2]

        jne  floop
        add  esi, srcadd

        add  edi, dstadd
        add  ebx, alphasrcadd

        dec  [tmpecx]
        jne  forloop

        jmp  done   

    reverse:
        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1

        add  eax, srcadd
        mov  srcadd, eax

        mov  eax, srcadd
        shr  eax, 1

        mov  alphasrcadd, eax

        mov  eax, [dpval.sheight]
        mov  [tmpecx], eax

    revloop:
        mov  eax, [dpval.swidth]
        mov  [tmpecx2], eax

    rloop:
        mov  dh, 31
        sub  dh, [ebx]
        jnz  revNotZero

        mov  uint32_t ptr [edi], uint32_t ptr 0x00000000
        jmp  revZeroIntensity1

    revNotZero:
        mov  eax, [edi]
        shr  eax, 16

        mov  dl, al                     // Do Blue
        mov  cl, [ColorTable + edx]

        mov  ch, ah
        shl  ecx, 16

        mov  eax, [edi]
        mov  dl, uint8_t PTR [edi + 2]     // Do Green

        mov  cl, [ColorTable + edx]

        mov  dl, ah                     // Do Red
        mov  cl, [ColorTable + edx]

        mov  [edi], ecx

    revZeroIntensity1:
        mov  dh, [ebx]
        or   dh, dh

        je   revZeroIntensity2

        xor  ecx, ecx
        mov  cl, [esi]

        mov  eax, Conv16to32Lower[ecx]
        mov  cl, [esi + 1]

        add  eax, Conv16to32Upper[ecx]

        mov  [color], eax

        shr  eax, 16

        mov  dl, al                     // Do Blue
        mov  cl, [ColorTable + edx]

        xor  ch, ch
        shl  ecx, 16

        mov  eax, [color]
        mov  dl, al                     // Do Red

        mov  cl, [ColorTable + edx]
        mov  dl, ah                     // Do Blue

        mov  ch, [ColorTable + edx]

        add  [edi], ecx

    revZeroIntensity2:
        sub  esi, 2
        add  edi, 4
        dec  ebx                    

        dec  ecx
        jne  rloop

        add  esi, srcadd
        add  edi, dstadd
        add  ebx, alphasrcadd

        mov  ecx, [tmpecx]
        dec  ecx
        jne  revloop

    done:
    }
    #endif
    
    return true;
}

bool Alias(PSDrawBlock db, PSDrawParam dp)
// put alias data, which is stored in RLE with end of line codes
{
    if (!TransPut(db, dp))
        return false;

    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (!db->alias || dp->swidth < 1 || dp->sheight < 1 ||
                      dp->dwidth < 1 || dp->dheight < 1)
        return false;
    
    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    // orient dstadd for byte maneuvering
    dstadd = db->dstride << 1;

    // endptr tells where to end the drawing for clipping bottom edge
    uint8_t *endptr = ((uint8_t *)db->dest) + dstoff + (dp->sheight * dstadd);
    // the next two are for left and right clipping
    uint8_t *leftptr = ((uint8_t *)db->dest) + dstoff;
    uint8_t *rightptr = leftptr + (dp->swidth << 1);

    uint32_t nextdestrow;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov  esi, [dbval.alias]             // load source alias data
        mov  edi, [dbval.dest]              // load destination
        add  edi, dstoff                    // skip to start location in dest buf
        sub  edi, [dpval.sx]                // handle left clipping internally
        sub  edi, [dpval.sx]                // do it twice for 2 byte pixels
        mov  nextdestrow, edi               // hang on to the start of the row
        mov  edx, 0                         // clear dx

        mov  eax, [dpval.drawmode]
        and  eax, DM_NOCLIP                 // do we need to bother with clipping?
        jnz  setuprun                       // if so, jump straight in

        // do top clipping
        mov  ecx, [dpval.sy]                // how many lines to clip from top
    topclip:
        or   ecx, ecx
        jz   setuprun
        dec  ecx

    topskiprun:
        mov  al, [esi]                      // load alias code
        inc  esi

        cmp  al, AL_EOL                     // only interested in EOLs
        je   topclip

    //topdatarun:
        and  eax, eax
        mov  al, [esi]

        inc  esi

        add  esi, eax
        add  esi, eax
        add  esi, eax                       // add (al * 3) to esi
        jmp  topskiprun   

        // do the aliasing for non-clipped portion  
    setuprun:
        mov  cl, [esi]                      // load up run code
        inc  esi

        cmp  cl, AL_EOL                     // check for end of line
        je   nextline
        
    skiprun:
        or   cl, cl                         // once skip complete, do data run
        jz   setupdatarun
        
        add  edi, 2                         // skip the pixel in the dest buf
        dec  cl                             // decrement run counter
        jmp  skiprun

    setupdatarun:
        mov  cl, [esi]                      // get run code
        inc  esi

    datarun:
        or   cl, cl                         // check for remaining pixels
        jz   setuprun

        mov  bx, [esi]                      // word pixel (color)
        add  esi, 2
        mov  ch, [esi]                      // byte alias (intensity)
        inc  esi

        cmp  edi, leftptr                   // check for left clipping
        jl   clipedge
        cmp  edi, rightptr                  // check for right clipping
        jge  clipedge

        // pixel lookup and transfer
        mov  dh, 31
        sub  dh, ch                         // reverse intensity (0 = 100%, 31 = 0%)
        mov  dl, uint8_t PTR [edi + 1]         // get background pixel
        mov  ah, [ColorTableUpperHi + edx]  // color table lookup
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]             // other byte (two byte pixels)
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax
        
        mov  dh, ch                         // load intensity
        mov  dl, bh                         // source pixel value
        mov  ah, [ColorTableUpperHi + edx]  // lookup
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, bl                         // do other byte
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax                      // put the pixel

    clipedge:
        add  edi, 2                         // increment the background pointer
        dec  cl                             // decrement run counter
        jmp  datarun

    nextline:
        mov  edi, leftptr                   // borrow edi for a sec
        add  edi, dstadd
        mov  leftptr, edi

        mov  edi, rightptr                  // borrow edi for a sec
        add  edi, dstadd
        mov  rightptr, edi

        mov  edi, nextdestrow               // reset to start of line
        add  edi, dstadd                    // now jump to new line
        mov  nextdestrow, edi               // and save that for next time
                    
        cmp  edi, endptr                    // check for bottom clip
        jge  done

        mov  cl, [esi]                      // load up run code
        inc  esi
             
        cmp  cl, AL_EOL                     // blank line..
        je   nextline

        cmp  cl, AL_EOD                     // check for end of data
        jne  skiprun                        // do the line

    done:
        // if we get to here, EOD was reached...we're done
    }
    #endif

    return true;
}

bool AliasColor(PSDrawBlock db, PSDrawParam dp)
// put alias data, which is stored in RLE with end of line codes
{
    if (!TransPutColor(db, dp))
        return false;

    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (!db->alias || dp->swidth < 1 || dp->sheight < 1 ||
                      dp->dwidth < 1 || dp->dheight < 1)
        return false;
    
    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    // orient dstadd for byte maneuvering
    dstadd = db->dstride << 1;

    // endptr tells where to end the drawing for clipping bottom edge
    uint8_t *endptr = ((uint8_t *)db->dest) + dstoff + (dp->sheight * dstadd);
    // the next two are for left and right clipping
    uint8_t *leftptr = ((uint8_t *)db->dest) + dstoff;
    uint8_t *rightptr = leftptr + (dp->swidth << 1);

    uint32_t nextdestrow;
    uint16_t color = (uint16_t)dp->color;
    uint8_t colorhi = color >> 8;
    uint8_t colorlo = color & 0xff;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov  esi, [dbval.alias]             // load source alias data
        mov  edi, [dbval.dest]              // load destination
        add  edi, dstoff                    // skip to start location in dest buf
        sub  edi, [dpval.sx]                // handle left clipping internally
        sub  edi, [dpval.sx]                // do it twice for 2 byte pixels
        mov  nextdestrow, edi               // hang on to the start of the row
        mov  edx, 0                         // clear dx

        mov  eax, [dpval.drawmode]
        and  eax, DM_NOCLIP                 // do we need to bother with clipping?
        jnz  setuprun                       // if so, jump straight in

        // do top clipping
        mov  ecx, [dpval.sy]                // how many lines to clip from top
    topclip:
        or   ecx, ecx
        jz   setuprun
        dec  ecx

    topskiprun:
        mov  al, [esi]                      // load alias code
        inc  esi

        cmp  al, AL_EOL                     // only interested in EOLs
        je   topclip

    //topdatarun:
        and  eax, eax
        mov  al, [esi]

        inc  esi

        add  esi, eax
        add  esi, eax
        add  esi, eax                       // add (al * 3) to esi
        jmp  topskiprun   

        // do the aliasing for non-clipped portion  
    setuprun:
        mov  cl, [esi]                      // load up run code
        inc  esi

        cmp  cl, AL_EOL                     // check for end of line
        je   nextline
        
    skiprun:
        or   cl, cl                         // once skip complete, do data run
        jz   setupdatarun
        
        add  edi, 2                         // skip the pixel in the dest buf
        dec  cl                             // decrement run counter
        jmp  skiprun

    setupdatarun:
        mov  cl, [esi]                      // get run code
        inc  esi

    datarun:
        or   cl, cl                         // check for remaining pixels
        jz   setuprun

        mov  bx, [esi]                      // word pixel (color)
        add  esi, 2
        mov  ch, [esi]                      // byte alias (intensity)
        inc  esi

        cmp  edi, leftptr                   // check for left clipping
        jl   clipedge
        cmp  edi, rightptr                  // check for right clipping
        jge  clipedge

        // pixel lookup and transfer
        mov  dh, 31
        sub  dh, ch                         // reverse intensity (0 = 100%, 31 = 0%)
        mov  dl, uint8_t PTR [edi + 1]         // get background pixel
        mov  ah, [ColorTableUpperHi + edx]  // color table lookup
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, uint8_t PTR [edi]             // other byte (two byte pixels)
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        mov  [edi], ax
        
        mov  dh, ch                         // load intensity
        mov  dl, colorhi
        mov  ah, [ColorTableUpperHi + edx]  // lookup
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, colorlo                    // do other byte
        add  al, [ColorTableLower + edx]
        adc  ah, 0
        add  [edi], ax                      // put the pixel

    clipedge:
        add  edi, 2                         // increment the background pointer
        dec  cl                             // decrement run counter
        jmp  datarun

    nextline:
        mov  edi, leftptr                   // borrow edi for a sec
        add  edi, dstadd
        mov  leftptr, edi

        mov  edi, rightptr                  // borrow edi for a sec
        add  edi, dstadd
        mov  rightptr, edi

        mov  edi, nextdestrow               // reset to start of line
        add  edi, dstadd                    // now jump to new line
        mov  nextdestrow, edi               // and save that for next time
                    
        cmp  edi, endptr                    // check for bottom clip
        jge  done

        mov  cl, [esi]                      // load up run code
        inc  esi
             
        cmp  cl, AL_EOL                     // blank line..
        je   nextline

        cmp  cl, AL_EOD                     // check for end of data
        jne  skiprun                        // do the line

    done:
        // if we get to here, EOD was reached...we're done
    }
    #endif

    return true;
}

bool Alias32(PSDrawBlock db, PSDrawParam dp)
// Alias for 32 bit buffers.
{
    return false;
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (!db->alias || dp->swidth < 1 || dp->sheight < 1 ||
                      dp->dwidth < 1 || dp->dheight < 1)
        return false;
    
    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_DRAW

    // orient dstadd for byte maneuvering
    dstadd = db->dstride << 2;
    int32_t smalldstoff = dstoff >> 1;
    int32_t smalldstadd = dstadd >> 1;

    // endptr tells where to end the drawing for clipping bottom edge
    uint8_t *endptr = ((uint8_t *)db->dest) + dstoff + (dp->sheight * dstadd);
    // the next two are for left and right clipping
    uint8_t *leftptr = ((uint8_t *)db->dest) + dstoff;
    uint8_t *rightptr = leftptr + (dp->swidth << 2);

    uint32_t nextdestrow, nextzbufrow, nextnormrow;
    uint32_t tmpesp;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov  [tmpesp], esp                  // Save esp
        mov  esi, [dbval.alias]             // load source alias data
        mov  edi, [dbval.dest]              // load destination
        add  edi, dstoff                    // skip to start location in dest buf
        sub  edi, [dpval.sx]                // handle left clipping internally
        sub  edi, [dpval.sx]                // do it twice for 2 byte pixels
        sub  edi, [dpval.sx]                // do it twice for 2 byte pixels
        sub  edi, [dpval.sx]                // do it twice for 2 byte pixels
        mov  nextdestrow, edi               // hang on to the start of the row

        mov  esp, [dbval.dzbuffer]          // esp is dest zbuffer
        add  esp, [smalldstoff]             // go to start in zbuffer
        sub  esp, [dpval.sx]                // handle left clipping internally
        sub  esp, [dpval.sx]                // do it twice for 2 byte pixels
        mov  nextzbufrow, esp               // hang on to the start of the row

        mov  edx, 0                         // we'll need high word of dx clear

        mov  eax, [dpval.drawmode]
        and  eax, DM_NOCLIP                 // do we need to bother with clipping?
        jnz  startdraw                      // if so, jump straight in

        // do top clipping
        mov  ecx, [dpval.sy]                // how many lines to clip from top
    topclip:
        or   ecx, ecx
        jz   setuprun
        dec  ecx

    topskiprun:
        mov  al, [esi]                      // load alias code
        inc  esi

        cmp  al, AL_EOL                     // only interested in EOLs
        je   topclip

    //topdatarun:
        mov  al, [esi]
        inc  esi

        add  esi, eax
        add  esi, eax
        add  esi, eax
        add  esi, eax
        add  esi, eax
        add  esi, eax
        add  esi, eax                       // add (al * 7) to esi
        jmp  topskiprun   

    startdraw:
        mov  ecx, [dbval.dnormals]
        add  ecx, [smalldstoff]             // go to start in normal buffer
        sub  ecx, [dpval.sx]                // handle left clipping internally
        sub  ecx, [dpval.sx]
        mov  nextnormrow, ecx               // hang on to the start of the row

        // do the aliasing for non-clipped portion  
    setuprun:
        mov  ah, [esi]                      // load up run code
        inc  esi

        cmp  ah, AL_EOL                     // check for end of line
        je   nextline
        
    skiprun:
        or   ah, ah                         // once skip complete, do data run
        jz   setupdatarun
        
        add  edi, 4                         // skip the pixel in the dest buf
        add  esp, 2                         // skip the pixel in the dest zbuffer
        add  ecx, 2                         // skip the pixel in the dest normals
        dec  ah                             // decrement run counter
        jmp  skiprun

    setupdatarun:
        mov  ah, [esi]                      // get run code
        inc  esi

    datarun:
        or   ah, ah                         // check for remaining pixels
        jz   setuprun

        cmp  edi, leftptr                   // check for left clipping
        jl   clipedge
        cmp  edi, rightptr                  // check for right clipping
        jge  clipedge

        // check zbuffer
        mov  bx, uint16_t PTR [esi + 3]
        add  bx, [dpval.zpos]
        cmp  [esp], bx
        jb  clipedge

        mov  [esp], bx

//      mov  ax, [esp]
//      mov  ecx, uint32_t PTR [dstnormal]
//      mov  uint16_t PTR [ecx], ax

        // pixel lookup and transfer
        mov  dh, 31
        sub  dh, [esi + 2]
        jnz  NotZero

        mov  uint32_t ptr [edi], 0
        jmp  ZeroIntensity1

    NotZero:
        // Translucent pixel transfer
        mov  dl, uint8_t PTR [edi + 1]     // Do Blue
        mov  al, [ColorTable + edx]
        mov  [edi + 1], al

        mov  dl, uint8_t PTR [edi + 2]     // Do Green
        mov  al, [ColorTable + edx]
        mov  [edi + 2], al

        mov  dl, uint8_t PTR [edi + 3]     // Do Red
        mov  al, [ColorTable + edx]
        mov  [edi + 3], al

    ZeroIntensity1:
        mov  dh, [esi + 2]
        or   dh, dh
        je   ZeroIntensity2

        // Convert 16 bit to 32 bit
        mov  dl, [esi]
        mov  ebx, [Conv16to32Lower + edx * 4]

        mov  dl, [esi + 1]
        add  ebx, [Conv16to32Upper + edx * 4]

        // Translucent pixel transfer
        mov  dl, bh                     // Blue
        mov  al, [ColorTable + edx]
        add  [edi + 1], al

        shr  ebx, 16                    // get at the upper word of ebx
        mov  dl, bl                     // Green
        mov  al, [ColorTable + edx]
        add  [edi + 2], al

        mov  dl, bh                     // Red
        mov  al, [ColorTable + edx]
        add  [edi + 3], al

    ZeroIntensity2:
    clipedge:
        add  esi, 7                         // increment alias buffer
        add  edi, 4                         // increment the background pointer
        add  esp, 2                         // increment zbuffer
        add  ecx, 2                         // increment normal buffer
        dec  ah                             // decrement run counter
        jmp  datarun

    nextline:
        mov  edi, leftptr                   // borrow edi for a sec
        add  edi, dstadd
        mov  leftptr, edi

        mov  edi, rightptr                  // borrow edi for a sec
        add  edi, dstadd
        mov  rightptr, edi

        mov  esp, nextzbufrow               // reset to start of line
        add  esp, smalldstadd               // now jump to new line
        mov  nextzbufrow, esp               // and save that for next time

        mov  ecx, nextnormrow               // reset to start of line
        add  ecx, smalldstadd               // now jump to new line
        mov  nextnormrow, ecx               // and save that for next time

        mov  edi, nextdestrow               // reset to start of line
        add  edi, dstadd                    // now jump to new line
        mov  nextdestrow, edi               // and save that for next time
                    
        cmp  edi, endptr                    // check for bottom clip
        jge  done

        mov  ah, [esi]                      // load up run code
        inc  esi
             
        cmp  ah, AL_EOL                     // blank line..
        je   nextline

        cmp  ah, AL_EOD                     // check for end of data
        jne  skiprun                        // do the line

    done:
        // if we get to here, EOD was reached...we're done
        mov esp, [tmpesp]
    }
    #endif

    return true;
}

bool DrawSelected8(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->szbuffer == nullptr || db->dzbuffer == nullptr)
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
//          db->sbufheight != dp->sheight)
//          return ClipZBufferDecompress8(db, dp, palette);
//      
//      else
//          return ZBufferDecompress8(db, dp, palette);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW

    uint32_t lastpixel;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov  edi, [dbval.dest]      ; Point EDI to destination
        add  edi, [dstoff]          ; Adjusts EDI for offset
 
        mov  ebx, [dbval.szbuffer]  ; Points EBX to Src zbuffer
        add  ebx, [srczoff]         ; Adjusts EDX for offset

        mov  edx, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
        add  edx, [dstzoff]         ; Adjusts EDX for offset

        cld                         ; Forward direction

        mov  esi, [dpval.sheight]

    OuterLoop:
        mov  ecx, [dpval.swidth]    ; ECX contains number of rows to do.
        mov  [lastpixel], 0

    InnerLoop:
        mov  ax, [ebx]
        cmp  ax, 7f7fh
        je   RightSide

        add  ax, [dpval.zpos]
        cmp  [edx], ax
        jb   RightSide

        cmp  [lastpixel], 0
        jne  SkipRaw

        mov  [lastpixel], 1
        cmp  ecx, [dpval.swidth]
        je   SkipRaw                ; when we're the left edge don't draw

        mov  eax, [dpval.color]
        mov  [edi], ax
        jmp  SkipRaw

    RightSide:
        cmp  [lastpixel], 0
        je   Skip

        cmp  ecx, 1
        je   Skip                   ; when we're on the right edge don't draw

        mov  eax, [dpval.color]
        mov  [edi-2], ax

    Skip:
        mov [lastpixel], 0

    SkipRaw:
        add  edi, 2
        add  ebx, 2
        add  edx, 2

        dec  ecx
        jne  InnerLoop

        add  edi, dstadd
        add  ebx, srczadd
        add  edx, dstzadd

        dec  esi
        jne  OuterLoop
    }
    #endif

    return true;
}

bool DrawSelected(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->szbuffer == nullptr || db->dzbuffer == nullptr)
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
//          db->sbufheight != dp->sheight)
//          return ClipZBufferDecompress(db, dp);
//      
//      else
//          return ZBufferDecompress(db, dp);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW

    uint32_t tmpecx;   // Stores Temp ECX value in OuterLoop
    uint32_t lastpixel;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov  edi, [dbval.dest]      ; Point EDI to destination
        add  edi, [dstoff]          ; Adjusts EDI for offset
 
        mov  ebx, [dbval.szbuffer]  ; Points EBX to Src zbuffer
        add  ebx, [srczoff]         ; Adjusts EDX for offset

        mov  edx, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
        add  edx, [dstzoff]         ; Adjusts EDX for offset

        cld                         ; Forward direction

        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  Reverse

        mov  ecx, [bmheight]

    OuterLoop:
        mov  [tmpecx], ecx
        mov  ecx, [dpval.swidth]    ; ECX contains number of rows to do.
        mov  [lastpixel], 0

    InnerLoop:
        mov  ax, [ebx]
        add  ax, [dpval.zpos]
        cmp  [edx], ax
        jb   RightSide

        cmp  [lastpixel], 0
        jne  SkipRaw

        mov  [lastpixel], 1
        cmp  ecx, [dpval.swidth]
        je   SkipRaw                ; when we're the left edge don't draw

        mov  eax, [dpval.color]
        mov  [edi], ax
        jmp  SkipRaw

    RightSide:
        cmp  [lastpixel], 0
        je   Skip

        cmp  ecx, 1
        je   Skip                   ; when we're on the right edge don't draw

        mov  eax, [dpval.color]
        mov  [edi-2], ax

    Skip:
        mov [lastpixel], 0

    SkipRaw:
        add  edi, 2 
        add  ebx, 2
        add  edx, 2

        dec  ecx
        jne  InnerLoop

        add  edi, dstadd
        add  ebx, srczadd
        add  edx, dstzadd

        mov  ecx, [tmpecx]
        dec  ecx
        jne  OuterLoop
        jmp  Done

    Reverse:
        mov  ecx, [bmheight]
        add  ebx, [bmwidth]

    RevOuterLoop:
        mov  [tmpecx], ecx
        mov  ecx, [dpval.swidth]    ; ECX contains number of rows to do.
        mov  [lastpixel], 0

    RevInnerLoop:
        sub  ebx, 2

        mov  ax, [ebx]
        add  ax, [dpval.zpos]
        cmp  [edx], ax
        jb   RevRightSide

        cmp  [lastpixel], 0
        jne  RevSkipRaw

        mov  [lastpixel], 1
        cmp  ecx, 1
        je   RevSkipRaw             ; when we're the left edge don't draw

        mov  eax, [dpval.color]
        mov  [edi], ax
        jmp  RevSkipRaw

    RevRightSide:
        cmp  [lastpixel], 0
        je   RevSkip

        cmp  ecx, [dpval.swidth]
        je   RevSkip                    ; when we're on the right edge don't draw

        mov  eax, [dpval.color]
        mov  [edi-2], ax

    RevSkip:
        mov [lastpixel], 0

    RevSkipRaw:
        add  edi, 2 
        add  edx, 2

        dec  ecx
        jne  RevInnerLoop

        add  edi, dstadd
        add  edx, dstzadd

        add  ebx, bmwidth
        add  ebx, bmwidth
        add  ebx, srczadd

        mov  ecx, [tmpecx]
        dec  ecx
        jne  RevOuterLoop

    Done:
    }
    #endif

    return true;
}

bool ZPut(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->szbuffer == nullptr || db->dzbuffer == nullptr)
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
//          db->sbufheight != dp->sheight)
//          return ClipZBufferDecompress(db, dp);
//      
//      else
//          return ZBufferDecompress(db, dp);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW

    uint32_t tmpecx;   // Stores Temp ECX value in OuterLoop

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov  esi, [dbval.source]    ; Point ESI to source
        add  esi, [srcoff]          ; Adjusts EDI for offset

        mov  edi, [dbval.dest]      ; Point EDI to destination
        add  edi, [dstoff]          ; Adjusts EDI for offset
 
        mov  ebx, [dbval.szbuffer]  ; Points EBX to Src zbuffer
        add  ebx, [srczoff]         ; Adjusts EDX for offset

        mov  edx, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
        add  edx, [dstzoff]         ; Adjusts EDX for offset

        cld                         ; Forward direction

        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  Reverse

        mov  ecx, [bmheight]

    OuterLoop:
        mov  [tmpecx], ecx
        mov  ecx, [dpval.swidth]    ; ECX contains number of rows to do.

    InnerLoop:
        mov  ax, [ebx]
        cmp  ax, 0x7f7f             ; check keycolor
        je   SkipRaw

        add  ax, [dpval.zpos]       ; check against dest zbuf
        cmp  [edx], ax
        jb   SkipRaw

        mov  [edx], ax
        
        inc  ax
        and  ax, 0fffeh
        mov  ax, [esi]
        mov  [edi], ax

    SkipRaw:
        add  esi, 2 
        add  edi, 2 
        add  ebx, 2
        add  edx, 2

        dec  ecx
        jne  InnerLoop

        add  esi, srcadd
        add  edi, dstadd
        add  ebx, srczadd
        add  edx, dstzadd

        mov  ecx, [tmpecx]
        dec  ecx
        jne  OuterLoop
        jmp  Done

    Reverse:
        xor  eax, eax

        add  esi, [bmwidth]
        add  ebx, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1
        add  eax, srcadd
        mov  srcadd, eax
        mov  ecx, [bmheight]

    RevOuterLoop:
        mov  [tmpecx], ecx
        mov  ecx, [dpval.swidth]    ; ECX contains number of rows to do.

    RevInnerLoop:
        mov  ax, [ebx]
        cmp  [edx], ax
        add  ax, [dpval.zpos]
        jb   RevSkipRaw

        mov  [edx], ax
        
        inc  ax
        and  ax, 0fffeh
        mov  ax, [esi]
        mov  [edi], ax

    RevSkipRaw:
        sub  esi, 2 
        add  edi, 2 
        sub  ebx, 2
        add  edx, 2

        dec  ecx
        jne  RevInnerLoop

        add  esi, srcadd
        add  edi, dstadd
        add  ebx, srczadd
        add  edx, dstzadd

        mov  ecx, [tmpecx]
        dec  ecx
        jne  RevOuterLoop

    Done:
    }
    #endif

    return true;
}

bool ZPut8(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->szbuffer == nullptr || db->dzbuffer == nullptr)
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette + sizeof(uint16_t) * 256);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
//          db->sbufheight != dp->sheight)
//          return ClipZBufferDecompress832(db, dp, palette);
//      
//      else
//          return ZBufferDecompress832(db, dp, palette);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW

    uint32_t tmpecx;   // Stores Temp ECX value in OuterLoop
    uint32_t tmpedx;   // Stores Temp ECX value in OuterLoop
    uint32_t tmpesp;   // Stores Temp ECX value in OuterLoop

    uint32_t xloop = dpval.dwidth;
    uint32_t yloop = dpval.dheight;

    uint32_t srcstride = dbval.sstride;
    uint32_t srczstride = dbval.sstride << 1;
    uint32_t dststride = dbval.dstride << 2;
    uint32_t dstzstride = dbval.dstride << 1;

    uint32_t zval = dpval.zpos;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov  [tmpesp], esp          ; Save esp value

        mov  esi, [dbval.source]    ; Point ESI to source
        add  esi, [srcoff]          ; Adjusts ESI for offset

        mov  edi, [dbval.dest]      ; Point EDI to destination
        add  edi, [dstoff]          ; Adjusts EDI for offset
 
        mov  ebx, [dbval.szbuffer]  ; Points EBX to Src zbuffer
        add  ebx, [srczoff]     ; Adjusts EBX for offset

        mov  esp, [dbval.dzbuffer]  ; Points ESP to Dest zbuffer
        add  esp, [dstzoff]     ; Adjusts ESP for offset

        mov  edx, [palette]         ; Points EDX to palette

        cld                         ; Forward direction

        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  Reverse

        xor  eax, eax

    OuterLoop:
        xor  ecx, ecx

    InnerLoop:
        mov  al, [ebx + ecx * 2]
        mov  ah, [ebx + ecx * 2 + 1] 

        cmp  eax, 7f7fh
        je   SkipRaw

        add  eax, zval
        cmp  [esp + ecx * 2], ax
        jb   SkipRaw

        mov  [esp + ecx * 2], ax

        xor  eax, eax
        mov  al, [esi + ecx]
        mov  eax, [edx + eax * 4]
        shr  eax, 3
        mov  [edi + ecx * 4], eax
        xor  eax, eax

    SkipRaw:
        inc  ecx

        cmp  ecx, xloop
        jne  InnerLoop

        add  esi, srcstride
        add  edi, dststride
        add  ebx, srczstride
        add  esp, dstzstride

        dec  yloop
        jne  OuterLoop

        jmp  Done

    Reverse:
        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1

        add  ebx, eax

        add  eax, srcadd

        mov  srcadd, eax
        mov  eax, [bmheight]
        mov  [tmpedx], eax

    RevOuterLoop:
        mov  eax, [dpval.swidth]    ; ECX contains number of rows to do.
        mov  [tmpecx], eax

    RevInnerLoop:
        mov  ax, [ebx]
        cmp  ax, 7f7fh

        je   RevSkipRaw
        add  ax, [dpval.zpos]

        cmp  [edx], ax
        jb   RevSkipRaw

        mov  [edx], ax

        xor  eax, eax
        mov  al, [esi]
        shl  eax, 2
        add  eax, [palette]
        mov  ax, [eax]

        mov  [edi], eax

    RevSkipRaw:
        dec  esi
        add  edi, 4
        sub  ebx, 2
        add  edx, 2

        dec  [tmpecx]
        jne  RevInnerLoop

        add  esi, [srcadd]
        add  ebx, [srczadd]
        add  edi, [dstadd]
        add  edx, [dstzadd]

        dec  [tmpedx]
        jne  RevOuterLoop

    Done:
        mov  esp, [tmpesp]
    
    }
    #endif

    return true;
}

bool ZPut816(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->szbuffer == nullptr || db->dzbuffer == nullptr)
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
//          db->sbufheight != dp->sheight)
//          return ClipZBufferDecompress8(db, dp, palette);
//      
//      else
//          return ZBufferDecompress8(db, dp, palette);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW

    uint32_t tmpedx;   // Stores Temp ECX value in OuterLoop
    uint32_t tmpecx;   // Stores Temp ECX value in InnerLoop

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov  esi, [dbval.source]    ; Point ESI to source
        add  esi, [srcoff]          ; Adjusts ESI for offset

        mov  edi, [dbval.dest]      ; Point EDI to destination
        add  edi, [dstoff]          ; Adjusts EDI for offset

        mov  ebx, [dbval.szbuffer]  ; Points EBX to Src zbuffer
        add  ebx, [srczoff]     ; Adjusts EBX for offset

        mov  edx, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
        add  edx, [dstzoff]         ; Adjusts EDX for offset

        cld                         ; Forward direction

        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  Reverse

        mov  eax, [bmheight]
        mov  [tmpedx], eax

    OuterLoop:
        mov  eax, [dpval.swidth]    ; ECX contains number of rows to do.
        mov  [tmpecx], eax

    InnerLoop:
        mov  ax, [ebx]

        cmp  ax, 7f7fh
        je   SkipRaw

        add  ax, [dpval.zpos]

        cmp  [edx], ax
        jb   SkipRaw

        mov  [edx], ax
        xor  eax, eax

        mov  al, [esi]
        shl  eax, 1

        add  eax, [palette]
        mov  ax, [eax]

        mov  [edi], ax

    SkipRaw:
        inc  esi
        add  ebx, 2

        add  edi, 2
        add  edx, 2

        dec  [tmpecx]
        jne  InnerLoop

        add  esi, [srcadd]
        add  ebx, [srczadd]
                  
        add  edi, [dstadd]
        add  edx, [dstzadd]

        dec  [tmpedx]
        jne  OuterLoop

        jmp  Done

    Reverse:
        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1

        add  ebx, eax

        add  eax, srcadd
        mov  srcadd, eax
        mov  eax, [bmheight]
        mov  [tmpedx], eax

    RevOuterLoop:
        mov  eax, [dpval.swidth]    ; ECX contains number of rows to do.
        mov  [tmpecx], eax

    RevInnerLoop:
        mov  ax, [ebx]

        cmp  ax, 7f7fh
        je   RevSkipRaw

        add  ax, [dpval.zpos]
        cmp  [edx], ax
        jb   RevSkipRaw

        mov  [edx], ax

        xor  eax, eax
        mov  al, [esi]
        shl  eax, 1
        add  eax, [palette]
        mov  ax, [eax]

        mov  [edi], ax

    RevSkipRaw:
        dec  esi
        add  edi, 2
        sub  ebx, 2
        add  edx, 2

        dec  [tmpecx]
        jne  RevInnerLoop

        add  esi, [srcadd]
        add  ebx, [srczadd]
        add  edi, [dstadd]
        add  edx, [dstzadd]

        dec  [tmpedx]
        jne  RevOuterLoop

    Done:
    }
    #endif

    return true;
}

bool ZPut32(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->szbuffer == nullptr || db->dzbuffer == nullptr)
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
//          db->sbufheight != dp->sheight)
//          return ClipZBufferDecompress32(db, dp);
//
//      else 
//          return ZBufferDecompress32(db, dp);
    }

    if (db->dstbitmapflags & BM_32BIT)
    {
        SDrawBlock dbval = *db;
        SDrawParam dpval = *dp;

        SETUP_Z_DRAW

        uint32_t tmpecx;   // Stores Temp ECX value in OuterLoop
        uint32_t tmpesp;   // Stores Temp ECX value in InnerLoop
    
        uint32_t xloop = dpval.dwidth << 1;
        uint32_t yloop = dpval.dheight;

        uint32_t srcwidth = dbval.sstride << 1;
        uint32_t dstwidth = dbval.dstride << 2;
        uint32_t srczwidth = dbval.sstride << 1;
        uint32_t dstzwidth = dbval.dstride << 1;

        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            mov  [tmpesp], esp

            mov  esi, [dbval.source]    ; Point ESI to source
            add  esi, [srcoff]          ; Adjusts EDI for offset

            mov  edi, [dbval.dest]      ; Point EDI to destination
            add  edi, [dstoff]          ; Adjusts EDI for offset
 
            mov  ebx, [dbval.szbuffer]  ; Points EBX to Src zbuffer
            add  ebx, [srczoff]         ; Adjusts EDX for offset

            mov  esp, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
            add  esp, [dstzoff]         ; Adjusts EDX for offset

            cld                         ; Forward direction
        
            xor  edx, edx

            mov  eax, [dpval.drawmode]

            and  eax, DM_REVERSEHORZ
            jne  Reverse32

            mov  ecx, [bmheight]

        OuterLoop32:
            xor  ecx, ecx           ; ECX contains number of rows to do.

        InnerLoop32:
            mov  al, [ebx + ecx]
            mov  ah, [ebx + ecx + 1]
            add  al, uint8_t PTR [dpval.zpos]
            adc  ah, uint8_t PTR [dpval.zpos + 1]
            cmp  [esp + ecx], ax
            jb   SkipRaw32

//          inc  ax
//          and  ax, 0fffeh
            mov  [esp + ecx], ax

            mov  dl, [esi + ecx]
            mov  eax, [Conv16to32Lower + edx * 4]

            mov  dl, [esi + ecx + 1]
            add  eax, [Conv16to32Upper + edx * 4]

            mov  [edi + ecx * 2], eax

        SkipRaw32:
            add  ecx, 2

            cmp  ecx, xloop
            jne  InnerLoop32

            add  esi, srcwidth  ; Src
            add  edi, dstwidth  ; Dst
            add  ebx, srczwidth ; Src Z
            add  esp, dstzwidth ; Dst Z

            dec  yloop
            jne  OuterLoop32
            jmp  Done32

        Reverse32:
            add  esi, [bmwidth]
            add  ebx, [bmwidth]

            mov  eax, [bmwidth]
            shl  eax, 1
            add  eax, srcadd
            mov  srcadd, eax
    
            mov  ecx, [bmheight]

        RevOuterLoop32:
            xor  ecx, ecx               ; ECX contains number of rows to do.

        RevInnerLoop32:
            mov  ax, [ebx]
            add  ax, [dpval.zpos]
            cmp  [esp], ax
            jb   RevSkipRaw32

            inc  ax
            and  ax, 0fffeh
            mov  [esp], ax

            mov  dl, [esi]
            mov  eax, [Conv16to32Lower + edx * 4]

            mov  dl, [esi + 1]
            add  eax, [Conv16to32Upper + edx * 4]

            mov  [edi], eax

        RevSkipRaw32:
            sub  esi, 2 
            add  edi, 4 
            sub  ebx, 2
            add  esp, 2

            dec  ecx
            jne  RevInnerLoop32

            add  esi, srcadd
            add  edi, dstadd
            add  ebx, srczadd
            add  esp, dstzadd

            mov  ecx, [tmpecx]
            dec  ecx
            jne  RevOuterLoop32

        Done32:
            mov  esp, [tmpesp]
        }
        #endif
    }

    if (db->dstbitmapflags & BM_24BIT)
    {
        SDrawBlock dbval = *db;
        SDrawParam dpval = *dp;

        SETUP_Z_DRAW
    
        uint32_t tmpecx;   // Stores Temp ECX value in OuterLoop
        uint32_t tmpesp;   // Stores Temp ESP value;

        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            mov  [tmpesp], esp

            mov  esi, [dbval.source]    ; Point ESI to source
            add  esi, [srcoff]          ; Adjusts EDI for offset

            mov  edi, [dbval.dest]      ; Point EDI to destination
            add  edi, [dstoff]          ; Adjusts EDI for offset
 
            mov  ebx, [dbval.szbuffer]  ; Points EBX to Src zbuffer
            add  ebx, [srczoff]         ; Adjusts EDX for offset

            mov  esp, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
            add  esp, [dstzoff]         ; Adjusts EDX for offset

            cld                         ; Forward direction

            xor  edx, edx

            mov  eax, [dpval.drawmode]

            and  eax, DM_REVERSEHORZ
            jne  Reverse24

            mov  ecx, [bmheight]

        OuterLoop24:
            mov  [tmpecx], ecx
            mov  ecx, [dpval.swidth]            ; ECX contains number of rows to do.

        InnerLoop24:
            mov  ax, [ebx]
            add  ax, [dpval.zpos]
            cmp  [esp], ax
            jb   SkipRaw24

            inc  ax
            and  ax, 0fffeh
            mov  [esp], ax

            mov  dl, [esi]
            mov  eax, [Conv16to32Lower + edx * 4]

            mov  dl, [esi + 1]
            add  eax, [Conv16to32Upper + edx * 4]

            mov  [edi], eax

        SkipRaw24:
            add  esi, 2 
            add  edi, 3 
            add  ebx, 2
            add  esp, 2

            dec  ecx
            jne  InnerLoop24

            add  esi, srcadd
            add  edi, dstadd
            add  ebx, srczadd
            add  esp, dstzadd


            mov  ecx, [tmpecx]
            dec  ecx
            jne  OuterLoop24
            jmp  Done24

        Reverse24:
            add  esi, [bmwidth]
            add  ebx, [bmwidth]

            mov  eax, [bmwidth]
            shl  eax, 1
            add  eax, srcadd
            mov  srcadd, eax
    
            mov  ecx, [bmheight]

        RevOuterLoop24:
            mov  [tmpecx], ecx
            mov  ecx, [dpval.swidth]            ; ECX contains number of rows to do.

        RevInnerLoop24:

            mov  ax, [ebx]
            add  ax, [dpval.zpos]
            cmp  [esp], ax
            jb   RevSkipRaw24

            inc  ax
            and  ax, 0fffeh
            mov  [esp], ax

            mov  dl, [esi]
            mov  eax, [Conv16to32Lower + edx * 4]

            mov  dl, [esi + 1]
            add  eax, [Conv16to32Upper + edx * 4]

            mov  [edi], eax

        RevSkipRaw24:
            sub  esi, 2 
            add  edi, 3 
            sub  ebx, 2
            add  esp, 2

            dec  ecx
            jne  RevInnerLoop24

            add  esi, srcadd
            add  edi, dstadd
            add  ebx, srczadd
            add  esp, dstzadd

            mov  ecx, [tmpecx]
            dec  ecx
            jne  RevOuterLoop24

        Done24:
            mov  esp, [tmpesp]
        }
        #endif
    }

    return true;
}

bool ShutterZPut(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->szbuffer == nullptr || db->dzbuffer == nullptr)
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
//          db->sbufheight != dp->sheight)
//          return ClipZBufferDecompress(db, dp);
//      
//      else
//          return ZBufferDecompress(db, dp);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW

    uint32_t tmpecx;   // Stores Temp ECX value in OuterLoop
    uint32_t tmpedx;   // Stores Temp EDX value in OuterLoop

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov  esi, [dbval.source]    ; Point ESI to source
        add  esi, [srcoff]          ; Adjusts EDI for offset

        mov  edi, [dbval.dest]      ; Point EDI to destination
        add  edi, [dstoff]          ; Adjusts EDI for offset
 
        mov  ebx, [dbval.szbuffer]  ; Points EBX to Src zbuffer
        add  ebx, [srczoff]         ; Adjusts EDX for offset

        mov  edx, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
        add  edx, [dstzoff]         ; Adjusts EDX for offset

        cld                         ; Forward direction

        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  Reverse

        mov  eax, [bmheight]
        mov  [tmpedx], eax

    OuterLoop:
        mov  eax, [dpval.swidth]    ; ECX contains number of rows to do.
        mov  [tmpecx], eax

    InnerLoop:
        mov  eax, [dpval.dy]
        add  eax, [tmpedx]

        test eax, 1
        jne  line0
        
        mov  eax, [dpval.dx]
        add  eax, [tmpecx]

        test eax, 1
        je   SkipRaw
    
        jmp  draw   

    line0:
        mov  eax, [dpval.dx]
        add  eax, [tmpecx]

        test eax, 1
        jne  SkipRaw
    
    draw:
        mov  ax, [ebx]
        add  ax, [dpval.zpos]

        cmp  [edx], ax
        jb   SkipRaw

        mov  [edx], ax
        
        inc  ax
        and  ax, 0fffeh

        mov  ax, [esi]
        mov  [edi], ax

    SkipRaw:
        add  esi, 2 
        add  edi, 2 

        add  ebx, 2
        add  edx, 2

        dec  [tmpecx]
        jne  InnerLoop

        add  esi, srcadd
        add  edi, dstadd

        add  ebx, srczadd
        add  edx, dstzadd

        dec  [tmpedx]
        jne  OuterLoop

        jmp  Done

    Reverse:
        add  esi, [bmwidth]
        add  ebx, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1

        add  eax, srcadd
        mov  srcadd, eax

        mov  eax, [bmheight]
        mov  [tmpedx], eax

    RevOuterLoop:
        mov  eax, [dpval.swidth]    ; ECX contains number of rows to do.
        mov  [tmpecx], eax

    RevInnerLoop:
        test [tmpedx], 1
        jne  Revline0
        
        test [tmpecx], 1
        je   RevSkipRaw
    
        jmp  Revdraw    

    Revline0:
        test [tmpecx], 1
        jne  RevSkipRaw
    
    Revdraw:
        mov  ax, [ebx]
        add  ax, [dpval.zpos]

        cmp  [edx], ax
        jb   RevSkipRaw

        mov  [edx], ax
        inc  ax

        and  ax, 0fffeh
        mov  ax, [esi]

        mov  [edi], ax

    RevSkipRaw:
        sub  esi, 2 
        add  edi, 2 
        sub  ebx, 2
        add  edx, 2

        dec  [tmpecx]
        jne  RevInnerLoop

        add  esi, srcadd
        add  edi, dstadd

        add  ebx, srczadd
        add  edx, dstzadd

        dec  [tmpedx]
        jne  RevOuterLoop

    Done:
    }
    #endif

    return true;
}

bool ShutterZPut8(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->szbuffer == nullptr || db->dzbuffer == nullptr)
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette + sizeof(uint16_t) * 256);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
//          db->sbufheight != dp->sheight)
//          return ClipZBufferDecompress832(db, dp, palette);
//      
//      else
//          return ZBufferDecompress832(db, dp, palette);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW

    uint32_t pixelx;   // Stores Temp ECX value in OuterLoop
    uint32_t pixely;   // Stores Temp ECX value in OuterLoop
    uint32_t tmpecx;   // Stores Temp ECX value in OuterLoop
    uint32_t tmpedx;   // Stores Temp ECX value in OuterLoop

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov  esi, [dbval.source]    ; Point ESI to source
        add  esi, [srcoff]          ; Adjusts ESI for offset

        mov  edi, [dbval.dest]      ; Point EDI to destination
        add  edi, [dstoff]          ; Adjusts EDI for offset
 
        mov  ebx, [dbval.szbuffer]  ; Points EBX to Src zbuffer
        add  ebx, [srczoff]     ; Adjusts EBX for offset

        mov  edx, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
        add  edx, [dstzoff]     ; Adjusts EDX for offset

        cld                         ; Forward direction

        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  Reverse

        mov  eax, [bmheight]
        mov [tmpedx], eax

        mov  eax, [dpval.dy]
        mov [pixely], eax

    OuterLoop:
        mov  eax, [dpval.swidth]    ; ECX contains number of rows to do.
        mov  [tmpecx], eax

        mov  eax, [dpval.dx]
        mov [pixelx], eax

    InnerLoop:
        test [pixely], 1
        jne  line0
        
        test [pixelx], 1
        je   SkipRaw
    
        jmp  draw   

    line0:
        test [pixelx], 1
        jne  SkipRaw
    
    draw:
        mov  ax, [ebx]

        cmp  ax, 7f7fh
        je   SkipRaw

        add  ax, [dpval.zpos]

        cmp  [edx], ax
        jb   SkipRaw

        mov  [edx], ax

        xor  eax, eax
        mov  al, [esi]

        shl  eax, 2
        add  eax, [palette]

        mov  eax, [eax]
        mov  [edi], eax

    SkipRaw:
        inc  esi
        add  ebx, 2

        add  edi, 4
        add  edx, 2

        inc  [pixelx]
        dec  [tmpecx]

        jne  InnerLoop

        add  esi, [srcadd]
        add  ebx, [srczadd]

        add  edx, [dstzadd]
        add  edi, [dstadd]

        inc  [pixely]
        dec  [tmpedx]

        jne  OuterLoop

        jmp  Done

    Reverse:
        add  esi, [bmwidth]

        mov  eax, [bmwidth]
        shl  eax, 1

        add  ebx, eax

        add  eax, [srcadd]
        mov  [srcadd], eax

        mov  eax, [bmheight]
        mov  [tmpedx], eax

        mov  eax, [dpval.dy]
        mov  [pixely], eax

    RevOuterLoop:
        mov  eax, [dpval.swidth]    ; ECX contains number of rows to do.
        mov  [tmpecx], eax

        mov  eax, [dpval.dx]
        mov  [pixelx], eax

    RevInnerLoop:
        test [pixely], 1
        jne  Revline0
        
        test [pixelx], 1
        je   RevSkipRaw
    
        jmp  Revdraw    

    Revline0:
        test [pixelx], 1
        jne  RevSkipRaw
    
    Revdraw:
        mov  ax, [ebx]

        cmp  ax, 7f7fh
        je   RevSkipRaw

        add  ax, [dpval.zpos]
        cmp  [edx], ax
        jb   RevSkipRaw

        mov  [edx], ax
        xor  eax, eax

        mov  al, [esi]
        shl  eax, 1

        add  eax, [palette]
        mov  ax, [eax]

        mov  [edi], ax

    RevSkipRaw:
        dec  esi
        sub  ebx, 2

        add  edi, 2
        add  edx, 2

        inc  [pixelx]
        dec  [tmpecx]

        jne  RevInnerLoop

        add  esi, [srcadd]
        add  ebx, [srczadd]

        add  edi, [dstadd]
        add  edx, [dstzadd]

        inc  [pixely]
        dec  [tmpedx]

        jne  RevOuterLoop

    Done:
    }
    #endif

    return true;
}

bool ShutterZPut816(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->szbuffer == nullptr || db->dzbuffer == nullptr)
        return false;

    uint32_t *palette = (uint32_t *)((uint8_t *)db->palette);

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
//          db->sbufheight != dp->sheight)
//          return ClipZBufferDecompress8(db, dp, palette);
//      
//      else
//          return ZBufferDecompress8(db, dp, palette);
    }

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    SETUP_Z_DRAW

    uint32_t tmpecx;   // Stores Temp ECX value in OuterLoop
    uint32_t tmpedx;   // Stores Temp ECX value in OuterLoop
    uint32_t pixelx;   // Stores current pixel position on screen
    uint32_t pixely;   // Stores Temp ECX value in OuterLoop

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        cld                         ; Forward direction
                   
        mov  esi, [dbval.source]    ; Point ESI to source
        add  esi, [srcoff]          ; Adjusts ESI for offset

        mov  edi, [dbval.dest]      ; Point EDI to destination
        add  edi, [dstoff]          ; Adjusts EDI for offset
 
        mov  ebx, [dbval.szbuffer]  ; Points EBX to Src zbuffer
        add  ebx, [srczoff]     ; Adjusts EB for offset

        mov  edx, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
        add  edx, [dstzoff]         ; Adjusts EDX for offset

        mov  eax, [dpval.drawmode]

        and  eax, DM_REVERSEHORZ
        jne  Reverse

        mov  eax, [bmheight]
        mov  [tmpedx], eax

        mov  eax, [dpval.dy]
        mov  [pixely], eax

    OuterLoop:
        mov  eax, [dpval.swidth]    ; ECX contains number of rows to do.
        mov  [tmpecx], eax

        mov  eax, [dpval.dx]
        mov  [pixelx], eax

    InnerLoop:
        test [pixely], 1
        jne  line0
        
        test [pixelx], 1
        je   SkipRaw
    
        jmp  draw   

    line0:
        test [pixelx], 1
        jne  SkipRaw
    
    draw:
        mov  ax, [ebx]

        cmp  ax, 7f7fh
        je   SkipRaw

        add  ax, [dpval.zpos]

        cmp  [edx], ax
        jb   SkipRaw

        mov  [edx], ax
        xor  eax, eax

        mov  al, [esi]
        shl  eax, 1

        add  eax, [palette]
        mov  ax, [eax]

        mov  [edi], ax

    SkipRaw:
        inc  esi
        add  ebx, 2

        add  edi, 2
        add  edx, 2

        inc  [pixelx]
        dec  [tmpecx]

        jne  InnerLoop

        add  esi, [srcadd]
        add  ebx, [srczadd]

        add  edi, [dstadd]
        add  edx, [dstzadd]

        inc  [pixely]
        dec  [tmpedx]

        jne  OuterLoop

        jmp  Done

    Reverse:
        add  esi, [bmwidth]
        mov  eax, [bmwidth]
        shl  eax, 1

        add  ebx, eax

        add  eax, [srcadd]
        mov  [srcadd], eax

        mov  eax, [bmheight]
        mov  [tmpedx], eax

        mov  eax, [dpval.dy]
        mov  [pixely], eax

    RevOuterLoop:
        mov  eax, [dpval.swidth]    ; ECX contains number of rows to do.
        mov  [tmpecx], eax

        mov  eax, [dpval.dx]
        mov  [pixelx], eax

    RevInnerLoop:
        test [tmpedx], 1
        jne  Revline0
        
        test [tmpecx], 1
        je   RevSkipRaw
    
        jmp  Revdraw    

    Revline0:
        test [tmpecx], 1
        jne  RevSkipRaw
    
    Revdraw:
        mov  ax, [ebx]

        cmp  ax, 7f7fh
        je   RevSkipRaw

        add  ax, [dpval.zpos]

        cmp  [edx], ax
        jb   RevSkipRaw

        mov  [edx], ax
        xor  eax, eax

        mov  al, [esi]
        shl  eax, 1

        add  eax, [palette]
        mov  ax, [eax]

        mov  [edi], ax

    RevSkipRaw:
        dec  esi
        sub  ebx, 2

        add  edi, 2
        add  edx, 2

        inc  [pixelx]
        dec  [tmpecx]
                            
        jne  RevInnerLoop

        add  esi, [srcadd]
        add  ebx, [srczadd]

        add  edi, [dstadd]
        add  edx, [dstzadd]

        inc  [pixely]
        dec  [tmpedx]

        jne  RevOuterLoop

    Done:
    }
    #endif

    return true;
}

bool ShutterZPut32(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    if (db->szbuffer == nullptr || db->dzbuffer == nullptr)
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        return false;
//      if (dp->sx || dp->sy || db->sbufwidth != dp->swidth || 
//          db->sbufheight != dp->sheight)
//          return ClipZBufferDecompress32(db, dp);
//
//      else 
//          return ZBufferDecompress32(db, dp);
    }

    if (db->dstbitmapflags & BM_32BIT)
    {
        SDrawBlock dbval = *db;
        SDrawParam dpval = *dp;

        SETUP_Z_DRAW

        uint32_t tmpecx;   // Stores Temp ECX value in OuterLoop
        uint32_t tmpecx2;  // Stores Temp ECX value in OuterLoop
        uint32_t tmpesp;   // Stores Temp ECX value in InnerLoop
        
        uint32_t pixelx;   // Screen Pos of currently drawn pixel
        uint32_t pixely;

        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            cld                         ; Forward direction
            mov  [tmpesp], esp

            mov  esi, [dbval.source]    ; Point ESI to source
            add  esi, [srcoff]          ; Adjusts EDI for offset

            mov  edi, [dbval.dest]      ; Point EDI to destination
            add  edi, [dstoff]          ; Adjusts EDI for offset
 
            mov  ebx, [dbval.szbuffer]  ; Points EBX to Src zbuffer
            add  ebx, [srczoff]         ; Adjusts EDX for offset

            mov  esp, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
            add  esp, [dstzoff]         ; Adjusts EDX for offset

            xor  edx, edx
            mov  eax, [dpval.drawmode]

            and  eax, DM_REVERSEHORZ
            jne  Reverse32

            mov  eax, [bmheight]
            mov  [tmpecx], eax

            mov  eax, [dpval.dy]
            mov  [pixely], eax

        OuterLoop32:
            mov  eax, [dpval.swidth]            ; ECX contains number of rows to do.
            mov  [tmpecx2], eax

            mov  eax, [dpval.dx]
            mov  [pixelx], eax

        InnerLoop32:
            test [pixely], 1
            jne  line32
        
            test [pixelx], 1
            je   SkipRaw32
    
            jmp  draw32

        line32:
            test [pixelx], 1
            jne  SkipRaw32
    
        draw32:
            mov  ax, [ebx]
            add  ax, [dpval.zpos]

            cmp  [esp], ax
            jb   SkipRaw32

            inc  ax
            and  ax, 0fffeh

            mov  [esp], ax
            mov  dl, [esi]

            mov  eax, [Conv16to32Lower + edx * 4]
            mov  dl, [esi + 1]

            add  eax, [Conv16to32Upper + edx * 4]
            mov  [edi], eax

        SkipRaw32:
            add  esi, 2 
            add  edi, 4 

            inc  [pixelx]
            add  ebx, 2

            add  esp, 2
            dec  [tmpecx2]

            jne  InnerLoop32

            add  esi, srcadd
            add  edi, dstadd

            add  ebx, srczadd
            inc  [pixely]

            add  esp, dstzadd
            dec  [tmpecx]

            jne  OuterLoop32

            jmp  Done32

        Reverse32:
            add  esi, [bmwidth]
            add  ebx, [bmwidth]

            mov  eax, [bmwidth]
            shl  eax, 1

            add  eax, srcadd
            mov  srcadd, eax
    
            mov  eax, [bmheight]
            mov  [tmpecx], eax

            mov  eax, [dpval.dy]
            mov  [pixely], eax

        RevOuterLoop32:
            mov  eax, [dpval.swidth]            ; ECX contains number of rows to do.
            mov  [tmpecx2], eax

            mov  eax, [dpval.dx]
            mov  [pixelx], eax

        RevInnerLoop32:
            test [pixely], 1
            jne  Revline32
        
            test [pixelx], 1
            je   RevSkipRaw32
    
            jmp  Revdraw32

        Revline32:
            test [pixelx], 1
            jne  RevSkipRaw32
    
        Revdraw32:
            mov  ax, [ebx]
            add  ax, [dpval.zpos]

            cmp  [esp], ax
            jb   RevSkipRaw32

            inc  ax
            and  ax, 0fffeh

            mov  [esp], ax
            mov  dl, [esi]

            mov  eax, [Conv16to32Lower + edx * 4]
            mov  dl, [esi + 1]

            add  eax, [Conv16to32Upper + edx * 4]
            mov  [edi], eax

        RevSkipRaw32:
            sub  esi, 2 
            add  edi, 4 

            sub  ebx, 2
            inc  [pixelx]

            add  esp, 2
            dec  [tmpecx2]

            jne  RevInnerLoop32

            add  esi, srcadd
            add  edi, dstadd

            add  ebx, srczadd
            inc  [pixely]

            add  esp, dstzadd
            dec  [tmpecx]

            jne  RevOuterLoop32

        Done32:
            mov  esp, [tmpesp]
        }
        #endif
    }

    if (db->dstbitmapflags & BM_24BIT)
    {
        SDrawBlock dbval = *db;
        SDrawParam dpval = *dp;

        SETUP_Z_DRAW
    
        uint32_t tmpecx;   // Stores Temp ECX value in OuterLoop
        uint32_t tmpecx2;  // Stores Temp ECX value in OuterLoop
        uint32_t tmpesp;   // Stores Temp ESP value;

        uint32_t pixelx;   // Screen Pos of currently drawn pixel
        uint32_t pixely;

        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            cld                         ; Forward direction
            mov  [tmpesp], esp

            mov  esi, [dbval.source]    ; Point ESI to source
            add  esi, [srcoff]          ; Adjusts EDI for offset

            mov  edi, [dbval.dest]      ; Point EDI to destination
            add  edi, [dstoff]          ; Adjusts EDI for offset
 
            mov  ebx, [dbval.szbuffer]  ; Points EBX to Src zbuffer
            add  ebx, [srczoff]         ; Adjusts EDX for offset

            mov  esp, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
            add  esp, [dstzoff]         ; Adjusts EDX for offset

            xor  edx, edx
            mov  eax, [dpval.drawmode]

            and  eax, DM_REVERSEHORZ
            jne  Reverse24

            mov  eax, [bmheight]
            mov  [tmpecx], eax
            
            mov  eax, [dpval.dy]
            mov  [pixely], eax

        OuterLoop24:
            mov  eax, [dpval.swidth]            ; ECX contains number of rows to do.
            mov  [tmpecx2], eax

            mov  eax, [dpval.dx]
            mov  [pixelx], eax

        InnerLoop24:
            test [pixely], 1
            jne  line24
        
            test [pixelx], 1
            je   SkipRaw24
    
            jmp  draw24

        line24:
            test [pixelx], 1
            jne  SkipRaw24
    
        draw24:
            mov  ax, [ebx]
            add  ax, [dpval.zpos]

            cmp  [esp], ax
            jb   SkipRaw24

            inc  ax
            and  ax, 0fffeh

            mov  [esp], ax
            mov  dl, [esi]

            mov  eax, [Conv16to32Lower + edx * 4]
            mov  dl, [esi + 1]

            add  eax, [Conv16to32Upper + edx * 4]
            mov  [edi], eax

        SkipRaw24:
            add  esi, 2 
            add  edi, 3 

            add  ebx, 2
            inc  [pixelx]

            add  esp, 2
            dec  [tmpecx2]

            jne  InnerLoop24

            add  esi, srcadd
            add  edi, dstadd

            add  ebx, srczadd
            inc  [pixely]

            add  esp, dstzadd
            dec  [tmpecx]

            jne  OuterLoop24
            jmp  Done24

        Reverse24:
            add  esi, [bmwidth]
            add  ebx, [bmwidth]

            mov  eax, [bmwidth]
            shl  eax, 1
            add  eax, srcadd
            mov  srcadd, eax
    
            mov  eax, [bmheight]
            mov  [tmpecx], eax
            
            mov  eax, [dpval.dy]
            mov  [pixely], eax 

        RevOuterLoop24:
            mov  eax, [dpval.swidth]            ; ECX contains number of rows to do.
            mov  [tmpecx2], eax

            mov  eax, [dpval.dx]
            mov  [pixelx], eax 

        RevInnerLoop24:
            test [pixely], 1
            jne  Revline24
        
            test [pixelx], 1
            je   RevSkipRaw24
    
            jmp  Revdraw24

        Revline24:
            test [pixelx], 1
            jne  RevSkipRaw24
    
        Revdraw24:
            mov  ax, [ebx]
            add  ax, [dpval.zpos]

            cmp  [esp], ax
            jb   RevSkipRaw24

            inc  ax
            and  ax, 0fffeh

            mov  [esp], ax
            mov  dl, [esi]

            mov  eax, [Conv16to32Lower + edx * 4]
            mov  dl, [esi + 1]

            add  eax, [Conv16to32Upper + edx * 4]
            mov  [edi], eax

        RevSkipRaw24:
            sub  esi, 2 
            add  edi, 3 
        
            sub  ebx, 2
            inc  [pixelx]

            add  esp, 2
            dec  [tmpecx2]

            jne  RevInnerLoop24

            add  esi, srcadd
            add  edi, dstadd

            add  ebx, srczadd
            inc  [pixely]

            add  esp, dstzadd
            dec  [tmpecx]

            jne  RevOuterLoop24

        Done24:
            mov  esp, [tmpesp]
        }
        #endif
    }

    return true;
}

bool ZFind(PSDrawBlock db, PSDrawParam dp)
{
    SDrawBlock dbval = *db;
    PSDrawParam dpa = dp;

    if (dpa->swidth < 1 || dpa->sheight < 1 || dpa->dwidth < 1 || dpa->dheight < 1) 
        return false;

    if (db->srcbitmapflags & BM_COMPRESSED)
    {
        bool retval = false;

        SETUP_Z_DRAW

        uint16_t zval = ZFindChunk(db, dpa);

        if (zval == 0x7f7f)
            return false;

        zval += dpa->zpos;

        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            mov  edx, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
            add  edx, [dstzoff]         ; Adjusts EDX for offset

            mov  ax, [zval]
            cmp  [edx], ax
            jb   Exit

            mov  eax, true
            mov  retval, eax

        Exit:
        }
        #endif
        
        return retval;
    }

    if (db->dstbitmapflags & BM_32BIT)
    {
        SDrawBlock dbval = *db;
        SDrawParam dpval = *dpa;

        SETUP_Z_DRAW

        bool retval = false;

        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            mov  ebx, [dbval.szbuffer]  ; Points EBX to Src zbuffer
            add  ebx, [srczoff]         ; Adjusts EDX for offset

            mov  edx, [dbval.dzbuffer]  ; Points EDX to Dest zbuffer
            add  edx, [dstzoff]         ; Adjusts EDX for offset

            xor  eax, eax
            mov  ecx, [dpval.drawmode]
            and  ecx, DM_ZBUFFER
            jz  Compare32

            mov  ecx, [dbval.srcbitmapflags]
            and  ecx, BM_8BIT
            jnz  Do8Bit32

            mov  ax, [ebx]              ; Get source zbuf value
            cmp  ax, 0x7f7f
            je   Done32
            jmp  Compare32

        Do8Bit32:
            mov  al, [ebx]
            cmp  al, 0ffh
            je   Done32
            shl  eax, 2

        Compare32:
            add  ax, [dpval.zpos]
            cmp  [edx], ax              ; Is it the highest object displayed?
            jb   Done32                 ; If not, return false

            mov  [retval], 1            ; Otherwise true

        Done32:
        }
        #endif

        if (retval && dp->drawmode & DM_ZSTATIC)
        {
            // for static, check transparency on the bitmap
            uint8_t *ptr = (uint8_t *)db->source + srczoff;
            if (db->srcbitmapflags & BM_8BIT)
                return (*((uint8_t *)ptr) != db->keycolor);
            else if (db->srcbitmapflags & (BM_15BIT | BM_16BIT))
                return (*((uint16_t *)ptr) != db->keycolor);
        }

        return retval;
    }

    return false;
}

bool ZStretch(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    return false;
}

bool ZMaskStretch(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    return false;
}

bool Box(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    SDrawBlock dbval = *db;
    SDrawParam dpval = *dp;

    for (int32_t c = 0; c < 3; c++) // Graphics, Z, and Normal buffers
    {
        int32_t bmheight, bmwidth, srcoff, srcadd, dstoff, dstadd;
        int32_t dstzoff, dstzadd;
        int32_t oddwords = 0;
        
        uint32_t color, pixeldwordshift;
        uint32_t dstincr;

        void *dest;
        uint32_t dstbitmapflags;
        uint32_t dcolor;
        if (c == 0) // Dest buffer
        {
            dest = db->dest;
            if (!dest || (dp->drawmode & DM_NODRAW))
                continue;
            dstbitmapflags = db->dstbitmapflags;
            dcolor = dp->color;
        }
        else if (c == 1) // Z buffer
        {
            dest = db->dzbuffer;
            if (!dest || !(dp->drawmode & DM_ZBUFFER))
                continue;
            dstbitmapflags = BM_16BIT;
            dcolor = dp->zpos;
        }
        else if (c == 2) // Normal buffer
        {
            dest = db->dnormals;
            if (!dest || !(dp->drawmode & DM_NORMALS))
                continue;
            dstbitmapflags = BM_16BIT;
            dcolor = dp->normal;
        }

        if (dstbitmapflags & BM_8BIT)
        {
            color = (((dcolor & 0xff) << 24) | ((dcolor & 0xff) << 16) 
                  | ((dcolor & 0xff) << 8) | (dcolor & 0xff)); 
                              
            pixeldwordshift = 2;
            dstincr = 1;
        }

        else if (dstbitmapflags & BM_15BIT || dstbitmapflags & BM_16BIT) 
        {
            color = ((dcolor & 0xffff) << 16) | ((dcolor & 0xffff));
            pixeldwordshift = 1;
            dbval.dstbitmapflags = dstbitmapflags;
            SetupDraw(&bmwidth, &bmheight, &srcoff, &srcadd,
                &dstoff, &dstadd, &dbval, &dpval);
            
            dstzoff = dstoff;
            dstzadd = dstadd;
            dstincr = 4;

            oddwords = dpval.dwidth & 1;
        }

        else if (dstbitmapflags & BM_24BIT || dstbitmapflags & BM_32BIT) 
        {
            color = dcolor;
            pixeldwordshift = 0;
            
            if (dstbitmapflags & BM_32BIT)
            {
                dbval.dstbitmapflags = dstbitmapflags;
                SetupDraw(&bmwidth, &bmheight, &srcoff, &srcadd,
                    &dstoff, &dstadd, &dbval, &dpval);
                dstzoff = dstoff >> 1;
                dstzadd = dstadd >> 1;
                dstincr = 4;
            }           

            else
            {
                dbval.dstbitmapflags = dstbitmapflags;
                SetupDraw(&bmwidth, &bmheight, &srcoff, &srcadd, 
                    &dstoff, &dstadd, &dbval, &dpval); 
                dstzoff = (dstoff >> 1) / 3;
                dstzadd = (dstadd >> 1) / 3;
                dstincr = 3;
            }
        }

        uint32_t dstwidth = (dpval.dwidth >> pixeldwordshift);

        if (dest)
        {
            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {
                cld 

                mov  edi, [dest]
                add  edi, [dstoff]

                mov  eax, [color]
                mov  edx, [bmheight]
                mov  ebx, [oddwords]

            OuterLoop:
                mov  ecx, [dstwidth]    ;ECX contains number of rows to do.
                cmp  ecx, 0
                jz   SingleWord         ;Odd cases where only 16 bits per row

            InnerLoop:
                mov  [edi], eax
                add  edi, [dstincr]

                dec  ecx
                jne  InnerLoop

                or   ebx, ebx
                je   NotOdd

            SingleWord:
                mov  [edi], ax
                add  edi, 2
                
            NotOdd:
                add  edi, [dstadd]

                dec  edx
                jne  OuterLoop
            }
            #endif
        }
    }

    return true;
}

bool Convert15to16(PTBitmapData bitmap)
{
    if (Display->BitsPerPixel() == 15)
        return true;

    if (bitmap->flags & BM_8BIT)
        return ConvertPal15to16(bitmap);

    TBitmapData bmd = *bitmap;
    uint8_t *data = (uint8_t *)bitmap->data16;
    uint8_t *tmp = (uint8_t *)bitmap->zbuffer.ptr();

    if (bitmap->flags & BM_COMPRESSED)
    {
        uint8_t  DLE1 = *(data++);
        uint8_t  DLE2 = *(data++);
    
        uint8_t  AltDLE1;
        uint8_t  AltDLE2;
        uint8_t  Alt1 = 0;
        uint8_t  Alt2 = 0;

        switch(DLE1 & 0x03)
        {
            case 0:
                switch(DLE2 & 0x03)
                {
                    case 0:
                        Alt1 = 0x01;
                        Alt2 = 0x02;
                        break;

                    case 1:
                        Alt1 = 0x02;
                        Alt2 = 0x03;
                        break;

                    case 2:
                        Alt1 = 0x01;
                        Alt2 = 0x03;
                        break;

                    case 3:
                        Alt1 = 0x01;
                        Alt2 = 0x02;
                        break;
                }
                
            break;

            case 1:
                switch(DLE2 & 0x03)
                {
                    case 0:
                        Alt1 = 0x02;
                        Alt2 = 0x03;
                        break;

                    case 1:
                        Alt1 = 0x00;
                        Alt2 = 0x02;
                        break;

                    case 2:
                        Alt1 = 0x00;
                        Alt2 = 0x03;
                        break;

                    case 3:
                        Alt1 = 0x00;
                        Alt2 = 0x02;
                        break;
                }
                
            break;

            case 2:
                switch(DLE2 & 0x03)
                {
                    case 0:
                        Alt1 = 0x01;
                        Alt2 = 0x03;
                        break;

                    case 1:
                        Alt1 = 0x00;
                        Alt2 = 0x03;
                        break;

                    case 2:
                        Alt1 = 0x00;
                        Alt2 = 0x01;
                        break;

                    case 3:
                        Alt1 = 0x00;
                        Alt2 = 0x01;
                        break;
                }
                
            break;

            case 3:
                switch(DLE2 & 0x03)
                {
                    case 0:
                        Alt1 = 0x01;
                        Alt2 = 0x02;
                        break;

                    case 1:
                        Alt1 = 0x00;
                        Alt2 = 0x02;
                        break;

                    case 2:
                        Alt1 = 0x00;
                        Alt2 = 0x01;
                        break;

                    case 3:
                        Alt1 = 0x00;
                        Alt2 = 0x01;
                        break;
                }
            }

        AltDLE1 = DLE1 & 0xfc | Alt1;           
        AltDLE2 = DLE2 & 0xfc | Alt2;           

        int32_t loop = (bitmap->datasize - 2) & 0xfffffffe;

        while(loop > 0)
        {
            if (*data == DLE1)
            {
                if ( *(data + 1) == 0)
                {
                    data += 2;
                    loop -= 2;
                    continue;
                }

                else if (*(data + 1)  > 0x80)
                {
                    data += 2;
                    loop -= 2;
                    continue;
                }

                
                else
                {
                    data += 3;
                    loop -= 3;
                }
            }

            else if (*data == DLE2)
            {
                if (*(data + 1) == 0)
                {
                    data += 3;
                    loop -= 3;
                }

                else
                {
                    data += 4;
                    loop -= 4;
                }
            }
            
            else
            {
                #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
                __asm
                {
                    mov  esi, [data]
                    mov  ax, uint16_t ptr [esi]
                    mov  bx, ax
                    and  bx, 7fe0h
                    add  ax, bx

                    cmp  al, DLE1
                    jne  TestDLE2

                    mov  al, [AltDLE1]

                TestDLE2:
                    cmp  al, DLE2
                    jne  EndDLECmp2

                    mov  al, [AltDLE2]

                EndDLECmp2:
                    mov  uint16_t ptr [esi], ax
                }
                #endif
                
                data += 2;
                loop -= 2;      
            }
        }
    }
    
    else
    {
        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            cld

      // Load source
            mov  esi, [data]

      // Load lines
            mov  edx, [bmd.height]

        forward:
            mov  ecx, [bmd.width]     ; Do two pixels at a time

            test ecx, 1
            je   notodd

            mov  ax, [esi]
            and  ax, 7fe0h
            add  [esi], ax
            add  esi, 2

        notodd:
            shr  ecx, 1

        floop:
            mov  eax, [esi]
            and  eax, 7fe07fe0h           ; Ands out blue
            add  [esi], eax
            add  esi, 4                   ; doubles green
            dec  ecx
            jne  floop

            dec  edx
            jne  forward
        }
        #endif
    }

    // transform alias data (RLE)
    if (bitmap->flags & BM_ALIAS)
    {
        uint32_t *alias = (uint32_t *) bitmap->alias.ptr();

        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            mov  esi, [alias]                   // load alias data

        setuprun:
            mov  cl, [esi]                      // load up run code
            inc  esi
            cmp  cl, AL_EOL                     // check for end of line
            je   nextline
            
        skiprun:
            or   cl, cl                         // once skip complete, do data run
            jz   setupdatarun
            
            dec  cl                             // decrement run counter
            jmp  skiprun

        setupdatarun:
            mov  cl, [esi]                      // get run code
            inc  esi

        datarun:
            or   cl, cl                         // check for remaining pixels
            jz   setuprun

            mov  ax, [esi]                      // pixel to transform
            mov  bx, ax                         // save it
            and  bx, 7fe0h                      // and out the blue
            add  ax, bx                         // double green
            mov  [esi], ax                      // overwrite old (15-bit) pixel

            add  esi, 3                         // skip pixel data and intensity
            dec  cl                             // decrement run counter
            jmp  datarun

        nextline:
            mov  cl, [esi]                      // load up run code
            inc  esi

            cmp  cl, AL_EOL                     // blank line..
            je   nextline

            cmp  cl, AL_EOD                     // check for end of data
            jne  skiprun                            // do the line

            // if we get to here, EOD was reached...we're done
        }
        #endif

    }
    
    data = (uint8_t *)bitmap->zbuffer.ptr();

    bitmap->flags = (bitmap->flags & ~BM_15BIT) | BM_16BIT;
    int32_t r = (bitmap->keycolor >> 10) & 0x1F;
    int32_t g = (bitmap->keycolor >> 5) & 0x1F;
    int32_t b = bitmap->keycolor & 0x1F;
    bitmap->keycolor = (r << 11) | (g << 6) | b;

    return true;
}

bool ConvertPal15to16(PTBitmapData bitmap)
{
    if (Display->BitsPerPixel() == 15)
        return true;

    uint16_t *pal = (uint16_t *)bitmap->palette.ptr();

    for (int32_t i = 0; i < 256; pal++, i++)
    {
        uint16_t red = (*pal >> 10) & 0x1F;
        uint16_t green = (*pal >> 5) & 0x1F;
        uint16_t blue = *pal & 0x1F;
        *pal = (red << 11) | (green << 6) | blue;
    }

    return true;
}

bool Convert16to15(PTBitmapData bitmap)
{
    if (Display->BitsPerPixel() == 15)
        return true;

    if (bitmap->flags & BM_8BIT)
        return ConvertPal16to15(bitmap);

    TBitmapData bmd = *bitmap;
    uint8_t *data = (uint8_t *)bitmap->data16;
    uint8_t *tmp = (uint8_t *)bitmap->zbuffer.ptr();

    if (bitmap->flags & BM_COMPRESSED)
    {
        uint8_t  DLE1 = *(data++);
        uint8_t  DLE2 = *(data++);
    
        uint8_t  AltDLE1;
        uint8_t  AltDLE2;
        uint8_t  Alt1 = 0;
        uint8_t  Alt2 = 0;

        switch(DLE1 & 0x03)
        {
            case 0:
                switch(DLE2 & 0x03)
                {
                    case 0:
                        Alt1 = 0x01;
                        Alt2 = 0x02;
                        break;

                    case 1:
                        Alt1 = 0x02;
                        Alt2 = 0x03;
                        break;

                    case 2:
                        Alt1 = 0x01;
                        Alt2 = 0x03;
                        break;

                    case 3:
                        Alt1 = 0x01;
                        Alt2 = 0x02;
                        break;
                }
                
            break;

            case 1:
                switch(DLE2 & 0x03)
                {
                    case 0:
                        Alt1 = 0x02;
                        Alt2 = 0x03;
                        break;

                    case 1:
                        Alt1 = 0x00;
                        Alt2 = 0x02;
                        break;

                    case 2:
                        Alt1 = 0x00;
                        Alt2 = 0x03;
                        break;

                    case 3:
                        Alt1 = 0x00;
                        Alt2 = 0x02;
                        break;
                }
                
            break;

            case 2:
                switch(DLE2 & 0x03)
                {
                    case 0:
                        Alt1 = 0x01;
                        Alt2 = 0x03;
                        break;

                    case 1:
                        Alt1 = 0x00;
                        Alt2 = 0x03;
                        break;

                    case 2:
                        Alt1 = 0x00;
                        Alt2 = 0x01;
                        break;

                    case 3:
                        Alt1 = 0x00;
                        Alt2 = 0x01;
                        break;
                }
                
            break;

            case 3:
                switch(DLE2 & 0x03)
                {
                    case 0:
                        Alt1 = 0x01;
                        Alt2 = 0x02;
                        break;

                    case 1:
                        Alt1 = 0x00;
                        Alt2 = 0x02;
                        break;

                    case 2:
                        Alt1 = 0x00;
                        Alt2 = 0x01;
                        break;

                    case 3:
                        Alt1 = 0x00;
                        Alt2 = 0x01;
                        break;
                }
            }

        AltDLE1 = DLE1 & 0xfc | Alt1;           
        AltDLE2 = DLE2 & 0xfc | Alt2;           

        int32_t loop = (bitmap->datasize - 2) & 0xfffffffe;

        while(loop > 0)
        {
            if (*data == DLE1)
            {
                if ( *(data + 1) == 0)
                {
                    data += 2;
                    loop -= 2;
                    continue;
                }

                else if (*(data + 1)  > 0x80)
                {
                    data += 2;
                    loop -= 2;
                    continue;
                }

                
                else
                {
                    data += 3;
                    loop -= 3;
                }
            }

            else if (*data == DLE2)
            {
                data += 4;
                loop -= 4;
            }
            
            else
            {
                #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
                __asm
                {
                    mov  esi, [data]
                    mov  ax, uint16_t ptr [esi]
                    mov  bx, ax
                    and  bx, 7fe0h
                    add  ax, bx

                    cmp  al, DLE1
                    jne  TestDLE2

                    mov  al, [AltDLE1]

                TestDLE2:
                    cmp  al, DLE2
                    jne  EndDLECmp2

                    mov  al, [AltDLE2]

                EndDLECmp2:
                    mov  uint16_t ptr [esi], ax
                }
                #endif
                
                data += 2;
                loop -= 2;      
            }
        }
    }
    
    else
    {
        uint16_t *ptr = (uint16_t *)data;
        for (int32_t i = 0; i < bmd.height * bmd.width; i++)
        {
            int32_t r = (*ptr >> 11) & 0x1F;
            int32_t g = (*ptr >> 5) & 0x3F;
            int32_t b = *ptr & 0x1F;
            *ptr++ = (r << 10) | ((g >> 1) << 5) | b;
        }
    }

    // transform alias data (RLE)
    if (bitmap->flags & BM_ALIAS)
    {
        uint32_t *alias = (uint32_t *) bitmap->alias.ptr();

        #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
        __asm
        {
            mov  esi, [alias]                   // load alias data

        setuprun:
            mov  cl, [esi]                      // load up run code
            inc  esi
            cmp  cl, AL_EOL                     // check for end of line
            je   nextline
            
        skiprun:
            or   cl, cl                         // once skip complete, do data run
            jz   setupdatarun
            
            dec  cl                             // decrement run counter
            jmp  skiprun

        setupdatarun:
            mov  cl, [esi]                      // get run code
            inc  esi

        datarun:
            or   cl, cl                         // check for remaining pixels
            jz   setuprun

            mov  ax, [esi]                      // pixel to transform
            mov  bx, ax                         // save the pixel
            shr  ax, 6                          // divide green by two
            and  ax, 1Fh                        // and out the red
            shl  ax, 5                          // realign green bits
            or   ax, bx                         // add back in the red and the blue
            mov  [esi], ax                      // overwrite old (15-bit) pixel

            add  esi, 3                         // skip pixel data and intensity
            dec  cl                             // decrement run counter
            jmp  datarun

        nextline:
            mov  cl, [esi]                      // load up run code
            inc  esi

            cmp  cl, AL_EOL                     // blank line..
            je   nextline

            cmp  cl, AL_EOD                     // check for end of data
            jne  skiprun                            // do the line

            // if we get to here, EOD was reached...we're done
        }
        #endif

    }
    
    data = (uint8_t *)bitmap->zbuffer.ptr();

    bitmap->flags = (bitmap->flags & ~BM_16BIT) | BM_15BIT;

    return true;
}

bool ConvertPal16to15(PTBitmapData bitmap)
{
    if (Display->BitsPerPixel() == 15)
        return true;

    uint16_t *pal = (uint16_t *)bitmap->palette.ptr();

    for (int32_t i = 0; i < 256; pal++, i++)
    {
        uint16_t red = (*pal >> 11) & 0x1F;
        uint16_t green = (*pal >> 6) & 0x1F;
        uint16_t blue = *pal & 0x1F;
        *pal = (red << 10) | (green << 5) | blue;
    }

    return true;
}

bool LineDraw(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    int32_t x1 = ((SLineParam *)dp->data)->x1;
    int32_t y1 = ((SLineParam *)dp->data)->y1;
    int32_t x2 = ((SLineParam *)dp->data)->x2;
    int32_t y2 = ((SLineParam *)dp->data)->y2;
    int32_t dx = x2 - x1 + 1;
    int32_t dy = y2 - y1 + 1;
    uint32_t color = dp->color;

  // Trivial clipping
    if (x2 < dp->dx || y2 < dp->dy ||
        x1 >= dp->dx + dp->dwidth || y1 >= dp->dy + dp->dheight)
        return false;

  // Non trivial clipping (anybody every played non trivial clipping pursuit?)
    if (x1 < dp->dx)
    {
        y1 += (dp->dx - x1) * dy / dx;  // Add to y1 a proportional amount
        x1 = dp->dx;
    }
    if (y1 < dp->dy)
    {
        x1 += (dp->dy - y1) * dx / dy;  // Add to x1 a proportional amount 
        y1 = dp->dy;
    }
    if (x1 >= dp->dx + dp->dwidth)
    {
        y1 -= (x1 - dp->dx + dp->dwidth + 1) * dy / dx;  // Subtract proportion
        x1 = dp->dx + dp->dwidth - 1;
    }
    if (y1 >= dp->dy + dp->dheight)
    {
        x1 -= (y1 - dp->dy + dp->dheight + 1) * dx / dy;    // Subtract proportion
        y1 = dp->dy + dp->dheight - 1;
    }
    
    x1 += dp->originx;
    y1 += dp->originy;
    x2 += dp->originx;
    y2 += dp->originy;

    int32_t width  = db->dstride;
    int32_t height = db->dbufheight;
    
    SDrawBlock drawb = *db;

    if ((db->dstbitmapflags & BM_15BIT) || (db->dstbitmapflags & BM_16BIT))
    {
        uint32_t newcolor = color;

        width       = width << 1;
        int32_t start   = y1 * width + ( x1 << 1);
        int32_t xchange = x2 - x1;
        int32_t ychange = y2 - y1;

        if (xchange == 0)
        {
            ychange++;
            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {
                mov  ax, uint16_t PTR newcolor
                mov  edi, drawb.dest
                add  edi, start

                mov  ecx, ychange

            vertloop:
                mov  [edi], ax
                add  edi, width

                dec  ecx
                jne  vertloop
            }
            #endif
        }

        else if (ychange == 0)
        {
            xchange++;
            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {
                mov  ax, uint16_t PTR newcolor
                shl  eax, 16
                mov  ax, uint16_t PTR newcolor

                mov  edi, drawb.dest
                add  edi, start

                mov  ecx, xchange
                test ebx, 1
                je   notodd
                mov  [edi], ax
                add  edi, 2

            notodd:
                shr  ecx, 1

            horzloop:
                mov  [edi], eax
                add  edi, 4

                dec  ecx
                jne  horzloop
            }
            #endif
        }

        else if (ychange == xchange)
        {
            uint16_t xincr = (int32_t)(((double)xchange / (double)ychange) * 65536.0);

            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {
                mov  ax, uint16_t PTR newcolor
                shl  eax, 16
                mov  ax, uint16_t PTR newcolor

                mov  edi, drawb.dest
                add  edi, start

                mov  ecx, ychange
            diagloop:
                mov  [edi], ax
                add  edi, width
                add  edi, 2

                dec  ecx
                jne  diagloop
            }
            #endif
        }

        else if (ychange > xchange)
        {
            uint16_t xincr = (int32_t)(((double)xchange / (double)ychange) * 65536.0);

            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {
                mov  ax, uint16_t PTR newcolor
                shl  eax, 16
                mov  ax, uint16_t PTR newcolor

                mov  edi, drawb.dest
                add  edi, start

                xor  ebx, ebx                   ; Error term

                mov  ecx, ychange
            yloop:
                mov  [edi], ax
                add  edi, width

                add  bx, xincr                  ; Check if error term is > 1 
                jnc  noincrement  

                add  edi, 2

            noincrement:
                dec  ecx
                jne  yloop
            }
            #endif
        }

        else                                    
        {
            uint16_t yincr = (int32_t)(((double)xchange / (double)ychange) * 65536.0);
            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {                                   ; xchange > ychange
                mov  ax, uint16_t PTR newcolor
                shl  eax, 16
                mov  ax, uint16_t PTR newcolor

                mov  edi, drawb.dest
                add  edi, start

                xor  ebx, ebx                   ; Error term

                mov  ecx, xchange

            xloop:
                mov  [edi], ax
                add  edi, 2

                add  bx, yincr                  ; Check if error term is > 1 
                jnc  noincrementx

                add  edi, width

            noincrementx:
                dec  ecx
                jne  xloop
            }
            #endif
        }
    }

    else if (db->dstbitmapflags & BM_24BIT)     
    {
        width = (width << 1) + width;
        int32_t start = y1 * width + (x1 << 1 + x1);
        int32_t xchange = x2 - x1;
        int32_t ychange = y2 - y1;

        if (xchange == 0)
        {
            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {                                   ; Draw 24 bit line
                mov  ah, color.red
                mov  al, color.green
                shl  eax, 16
                mov  ah, color.blue

                mov  edi, drawb.dest
                add  edi, start

                mov  ecx, ychange

            vertloop24:
                mov  al, [edi + 3]
                mov  [edi], eax
                add  edi, width

                dec  ecx
                jne  vertloop24
            }
            #endif
        }

        else if (ychange == 0)
        {
            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {
                mov  ah, color.red
                mov  al, color.green
                shl  eax, 16
                mov  ah, color.blue
                xor  al, al

                mov  edi, drawb.dest
                add  edi, start

                mov  ecx, xchange
                dec  ecx                        ; Do one less write since last write 
                                                ; must not change neighboring pixel
            horzloop24:
                mov  [edi], eax
                add  edi, 3

                dec  ecx
                jne  horzloop24
                
                mov  al, [edi + 3]              ; Write out last pixel the SLOOOW way
                mov  [edi], eax
            }
            #endif
        }

        else if (ychange = xchange)
        {
            uint16_t xincr = (int32_t)(((double)xchange / (double)ychange) * 65536.0);

            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {
                mov  ah, color.red
                mov  al, color.green
                mov  dl, color.blue

                mov  edi, drawb.dest
                add  edi, start

                mov  ecx, ychange
            
            diagloop24:
                mov  al, [edi + 3]
                mov  [edi], eax
                add  edi, 3
                add  edi, width

                dec  ecx
                jne  diagloop24
            }
            #endif
        }

        else if (ychange > xchange)
        {
            uint16_t xincr = (int32_t)(((double)xchange / (double)ychange) * 65536.0);

            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {
                mov  ah, color.red
                mov  al, color.green
                mov  dl, color.blue

                mov  edi, drawb.dest
                add  edi, start

                xor  ebx, ebx                   ; Error term

                mov  ecx, ychange
            yloop24:
                mov  al, [edi + 3]
                mov  [edi], eax
                add  edi, width

                add  bx, xincr                  ; Check if error term is > 1 
                jnc  noincrementy  

                add  edi, 3

            noincrementy:
                dec  ecx
                jne  yloop24
            }
            #endif
        }

        else                                    
        {
            uint16_t yincr = (int32_t)(((double)xchange / (double)ychange) * 65536.0);
            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {                                   ; xchange > ychange
                mov ah, color.red
                mov al, color.green
                mov dl, color.blue

                mov  edi, drawb.dest
                add  edi, start

                xor  ebx, ebx                   ; Error term

                mov  ecx, xchange
            xloop24:
                mov  al, [edi + 3]
                mov  [edi], eax
                add  edi, 3

                add  bx, yincr                  ; Check if error term is > 1 
                jnc  noincrement24x

                add  edi, width

            noincrement24x:
                dec  ecx
                jne  xloop24
            }
            #endif
        }
    }

    else if (db->dstbitmapflags & BM_32BIT)     
    {
        width = (width << 2);
        int32_t start = y1 * width + (x1 << 2);
        int32_t xchange = x2 - x1;
        int32_t ychange = y2 - y1;

        if (xchange == 0)
        {
            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {                                   ; Draw 32 bit line
                mov  ah, color.red
                mov  al, color.green
                shl  eax, 16
                mov  ah, color.blue

                mov  edi, drawb.dest
                add  edi, start

                mov  ecx, ychange

            vertloop32:
                mov  [edi], eax
                add  edi, width

                dec  ecx
                jne  vertloop32 
            }
            #endif
        }

        else if (ychange == 0)
        {
            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {
                mov  ah, color.red;
                mov  al, color.green;
                shl  eax, 16
                mov  ah, color.blue;

                mov  edi, drawb.dest
                add  edi, start

                mov  ecx, xchange

            horzloop32:
                mov  [edi], eax
                add  edi, 4

                dec  ecx
                jne  horzloop32
            }
            #endif
        }

        else if (ychange = xchange)
        {
            uint16_t xincr = (int32_t)(((double)xchange / (double)ychange) * 65536.0);

            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {
                mov  ah, color.red
                mov  al, color.green
                shl  eax, 16
                mov  ah, color.blue

                mov  edi, drawb.dest
                add  edi, start

                mov  ecx, ychange
            
            diagloop32:
                mov  [edi], eax
                add  edi, 4
                add  edi, width

                dec  ecx
                jne  diagloop32
            }
            #endif
        }

        else if (ychange > xchange)
        {
            uint16_t xincr = (int32_t)(((double)xchange / (double)ychange) * 65536.0);

            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {
                mov  ah, color.red
                mov  al, color.green
                shl  eax, 16
                mov  ah, color.blue

                mov  edi, drawb.dest
                add  edi, start

                xor  ebx, ebx                   ; Error term

                mov  ecx, ychange

            yloop32:
                mov  [edi], eax
                add  edi, width

                add  bx, xincr                  ; Check if error term is > 1 
                jnc  noincrementy32

                add  edi, 4

            noincrementy32:
                dec  ecx
                jne  yloop32
            }
            #endif
        }

        else                                    
        {
            uint16_t yincr = (int32_t)(((double)xchange / (double)ychange) * 65536.0);
            #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
            __asm
            {                                   ; xchange > ychange
                mov  ah, color.red
                mov  al, color.green
                shl  eax, 16
                mov  ah, color.blue

                mov  edi, drawb.dest
                add  edi, start

                xor  ebx, ebx                   ; Error term

                mov  ecx, xchange
            xloop32:
                mov  [edi], eax
                add  edi, 4

                add  bx, yincr                  ; Check if error term is > 65535
                jnc  noincrementx32

                add  edi, width

            noincrementx32:
                dec  ecx
                jne  xloop32
            }
            #endif
        }
    }

    return true;
}

bool TextDraw(PSDrawBlock db, PSDrawParam dp)
{
    int32_t startline = ((STextParam *)dp->data)->startline;
    int32_t numlines = ((STextParam *)dp->data)->numlines;
    char *text = ((STextParam *)dp->data)->text;
    TFont *font = ((STextParam *)dp->data)->font;
    int32_t wrapwidth = ((STextParam *)dp->data)->wrapwidth;
    int32_t justify = ((STextParam *)dp->data)->justify;
    bool draw = ((STextParam *)dp->data)->draw;
    int32_t linespace = ((STextParam *)dp->data)->linespace;            
    int32_t lineheight = font->height + linespace;
                    
    if (!text || !*text)
        return false;

    SDrawBlock ndb;
    SDrawParam ndp;
    PTBitmap ch;
    int32_t tmpy;

    ndb = *db;                              // preserve arguments
    ndp = *dp;

    ndp.func = nullptr;
    ndp.callback = nullptr;
    ndp.dy += lineheight;
    if (!((STextParam *)dp->data)->noclip)
        ndp.drawmode &= ~DM_NOCLIP;     // used to skip initial clip

    SRect update(1000000, 1000000, -1000000, -1000000);     // update rectangle for callback

    bool needsprocessing = wrapwidth < 0 ? false : true;    // wordwrap processing
    bool wrap = false;

    int32_t width = 0;                  // width of current line

    bool wasdrawing = draw;
    char *linestart = text;

    if (draw && justify & JUSTIFY_CLIP)
    {
        // do a single run through to determine text height after wordwrap
        ((STextParam *)dp->data)->draw = false;
        TextDraw(db, dp);
        ((STextParam *)dp->data)->draw = draw;

        if (((STextParam *)dp->data)->length > 1)
            ndp.dy -= (((STextParam *)dp->data)->length - 1) * lineheight;
    }

    if (!(justify & JUSTIFY_LEFT))
        draw = false;               // first pass just check text width

    int32_t l = 0;

    do
    {
        if (needsprocessing || (*text == ' ' && wrapwidth >= 0))
        {
            int32_t x = width;
            for (char *ptr = (text+1); *ptr && *ptr != ' ' && !wrap; ptr++)
            {
                x += font->DrawRight(*ptr) - font->DrawLeft(*ptr);
                if (x >= wrapwidth)
                    wrap = true;
            }
            needsprocessing = false;
        }

        if (wrap || *text == '\n' || *text == 0)
        {
            if (wasdrawing && !draw)
            {
                // figure out alignment then draw for real
                if (justify & JUSTIFY_CENTER)
                    ndp.dx = dp->dx - ((ndp.dx - dp->dx) / 2);
                else if (justify & JUSTIFY_RIGHT)
                    ndp.dx = dp->dx - (ndp.dx - dp->dx);

                if (justify & JUSTIFY_CLIP)
                {
                    // force it to stay within the given rect
                    int32_t left = dp->clipx - dp->originx + 16;
                    if (ndp.dx < left)
                        ndp.dx = left;

                    int32_t right = dp->clipx - dp->originx + dp->clipwidth - 16;
                    if ((ndp.dx + width) >= right)
                        ndp.dx = right - width;

                    int32_t top = dp->clipy - dp->originy + 24 + lineheight;
                    if (ndp.dy < top)
                        ndp.dy = top;
                }

                draw = true;
                text = linestart;
            }
            else
            {
                l++;
                if (l > startline)
                    ndp.dy += lineheight;
                ndp.dx = dp->dx;
                width = 0;

                if (!(justify & JUSTIFY_LEFT))
                {
                    linestart = text;
                    draw = false;
                }
            }

            wrap = false;
            width = 0;
        }

        if (*text != '\n')
        {
            ch = font->GetChar(*text);
            if (ch)
            {
                tmpy = ndp.dy;
                ndp.dy -= font->StartHeight(*text);
                ndp.dx -= font->DrawLeft(*text);

                if (draw && l >= startline && *text != ' ')
                {
                    ndb.source     = ch->data16;
                    ndb.sbufwidth  = ndp.swidth = ndp.dwidth = ch->width;
                    ndb.sbufheight = ndp.sheight = ndp.dheight = ch->height;
                    ndb.sstride    = ndb.sbufwidth;
                    ndb.alias      = (uint8_t *)ch->alias.ptr();

                    Draw(&ndb, &ndp);

                    // check bounds on update rectangle
                    if (ndp.dy < update.top)
                        update.top = ndp.dy;
                    if ((ndp.dy + ch->height - 1) > update.bottom)
                        update.bottom = ndp.dy + ch->height - 1;
                    if (ndp.dx < update.left)
                        update.left = ndp.dx;
                    if ((ndp.dx + ch->width - 1) > update.right)
                        update.right = ndp.dx + ch->width - 1;
                }

                ndp.dx += font->DrawRight(*text);
                ndp.dy = tmpy;

                width += font->DrawRight(*text) - font->DrawLeft(*text);
            }
        }
    } while (l < (startline + numlines) && *text++);

    if (dp->callback && ((STextParam *)dp->data)->draw)
    {
        ndp.dx = max(update.left, dp->clipx - dp->originx);
        ndp.dy = max(update.top, dp->clipy - dp->originy);
        ndp.dwidth = min(update.right - update.left + 1, dp->clipwidth);
        ndp.dheight = min(update.bottom - update.top + 1, dp->clipheight);
        dp->callback(&ndb, &ndp);
        dp->drawmode |= DM_DOESCALLBACK; // Tells draw function we already did callback
    }

    ((STextParam *)dp->data)->length = l - startline;

    return true;
}


// *********** Chunk Z Find Routine *********
uint16_t ZFindChunk(PSDrawBlock db, PSDrawParam dp)
{
    SChunkHeader* hdr = nullptr;
    
    if (dp->drawmode & DM_ZBUFFER)
        hdr = (SChunkHeader*)db->szbuffer;
    else if (dp->drawmode & DM_ZSTATIC)
        hdr = (SChunkHeader*)db->source;

    if (hdr == nullptr)
        return 0x7f7f;

    int32_t ULx = dp->sx / CHUNKWIDTH;
    int32_t ULy = dp->sy / CHUNKHEIGHT;

    int32_t sx = dp->sx % CHUNKWIDTH;
    int32_t sy = dp->sy % CHUNKHEIGHT;

    if (dp->drawmode & DM_ZBUFFER)
    {
        uint16_t *szbuffer = (uint16_t *)ChunkCache.AddChunkZ(hdr->block[ULy * hdr->width + ULx].ptr(), 2);

        if (szbuffer)
            return szbuffer[CHUNKWIDTH * sy + sx];
    }
    else if (dp->drawmode & DM_ZSTATIC)
    {
        if (db->srcbitmapflags & BM_8BIT)
        {
            uint8_t *buffer = (uint8_t *)ChunkCache.AddChunk(hdr->block[ULy * hdr->width + ULx].ptr(), 1);

            if (buffer && buffer[CHUNKWIDTH * sy + sx] != db->keycolor)
                return 0;

            return 0x7f7f;
        }
        else if (db->srcbitmapflags & (BM_15BIT | BM_16BIT))
        {
            uint16_t *buffer = (uint16_t *)ChunkCache.AddChunk16(hdr->block[ULy * hdr->width + ULx].ptr(), 1);

            if (buffer && buffer[CHUNKWIDTH * sy + sx] != db->keycolor)
                return 0;

            return 0x7f7f;
        }
    }

    return 0x7f7f;
}

