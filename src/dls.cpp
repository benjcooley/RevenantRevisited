// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    dls.cpp - Scene system module                      *
// *************************************************************************

#include "dls.h"

#include "bitmap.h"
#include "display.h"
#include "graphics.h"
#include "surface.h"

#include <math.h>

#define RADIUS 254
#define DIAMETER (RADIUS << 1)

// rdtsc macro removed — was only used for x86 cycle-count profiling in commented-out asm

uint8_t IntTable[256];

extern SColor LightColors[NUMBASELIGHTS];
extern int32_t LightUseCount[NUMBASELIGHTS];

extern uint8_t  DistTable[256][256];
extern uint8_t  AngleTable[256][256];
extern uint8_t  CosCos[64*256];
extern uint8_t  IntCos[256*256];
extern uint8_t  SinSin[64*256];
extern uint8_t  LightDropOffTable[32768];
extern uint16_t *LightTable;
extern uint8_t  ColorTable[32 * 256];
extern uint8_t  ColorTable16[32 * 256];
extern uint8_t  ColorTableUpperHi[32 * 256];
extern uint8_t  ColorTableUpperLo[32 * 256];
extern uint8_t  ColorTableLower[32 * 256];
extern uint32_t Conv16to32Upper[256];
extern uint32_t Conv16to32Lower[256];
extern uint32_t MMXLightTable[256];

// Aqua color light
extern double lred;
extern double lgreen;
extern double lblue;

/*
Normal buffer stuff is commented out to save some memory.
If you want to put it back in, you need to find every
occurance of 'nb' in this file and 'NormalBuffer' in
other files (like colortable.cpp) and uncomment it.
*/

// *****************************************************************************
// *                   L I G H T I N G   F U N C T I O N S                     *
// *****************************************************************************

//void DrawLight(S3DPoint& pos, RSColor color, int32_t intensity, TSurface* surface)
//{
//  See OLDDLS.CPP for source for this function
//}

_STRUCTDEF(SLightData)
struct SLightData
{
    int32_t z;
    SColor color;
    int32_t intensity;
    TSurface* surface;
    int32_t id;
};

void DrawStaticLightBlock(int32_t ptrpos, uint16_t *zbufptr, uint32_t *ubptr, 
    uint32_t z, int32_t intensity, int32_t nextline, uint8_t xpos, uint8_t xend, uint8_t ypos, uint8_t yend) 
{
    if (ypos == yend || xpos == xend)
        return;

    int32_t xdir, xdir2;
    if (xpos > xend)
        xdir = -1;
    else
        xdir = 1;
    xdir2 = xdir + xdir;

    char ydir;
    if (ypos > yend)
        ydir = -1;
    else
        ydir = 1;

    uint32_t curxend = 0;
    uint8_t iseven = !(abs(xend - xpos) & 1);

    uintptr_t ubpos = ptrpos + ((uintptr_t)ubptr / 4);
    uintptr_t zoffset = (uintptr_t)zbufptr - ((uintptr_t)ubptr / 2);

    uint32_t saveesp = 0;

//  uint32_t begtime, endtime;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
//      rdtsc
//      mov begtime, eax
//      rdtsc
//      mov endtime, eax

        mov  saveesp, esp

        mov  esi, ubpos
        mov  esp, zoffset
        xor  eax, eax
        xor  ebx, ebx
        xor  ecx, ecx
        xor  edx, edx

        mov  ch, ypos           
        mov  cl, xend

    yloop1:
        mov  curxend, ecx
        mov  cl, xpos
        mov  edi, ecx
        xor  ecx, ecx
        
        mov  al, iseven
        or   al, al
        jne  twoforone

        mov  al, DistTable[edi]                 ; Get XY dist
        cmp  al, 0FFh
        je singlenext1
        
        mov  bl, [esp + esi * 2]                ; Get Z
        mov  bh, [esp + esi * 2 + 1]
        sub  ebx, z
        jns  singlepos
        neg  ebx
    singlepos:
        cmp  ebx, intensity                     ; Make sure Z is in light range
        ja   singlenext1
        
        mov  bh, al
        mov  al, DistTable[ebx]                 ; d-z plane distance
        add  al, IntTable[eax]

        jc   singlenext1                        ; skip if dist too great

        mov  bl, uint8_t PTR [esi * 4 + 3]
        and  bl, 0FCh
        add  bl, LightDropOffTable[eax]         ; get brightness based on dist
        jnc  singlenochange
        or   bl, 0FCh
    singlenochange:
        mov  uint8_t PTR [esi * 4 + 3], bl

    singlenext1:
        inc  esi
        add  edi, xdir

        cmp  edi, curxend
        je   nexty1

        ; **** Do 2for1 block (count em.. 2for1)

    twoforone:
        mov  bh, DistTable[edi]                 ; Get XY dist
        mov  bl, DistTable[edi + 1]             ; Get XY dist 2
        cmp  ebx, 0FFFFh                        ; Both out of range
        je next1
        
        mov  al, [esp + esi * 2 + 0]            ; Get Z
        mov  ah, [esp + esi * 2 + 1]
        mov  dl, [esp + esi * 2 + 2]            ; Get Z 2
        mov  dh, [esp + esi * 2 + 3]
        sub  eax, z
        jns  twoforonepos1
        neg  eax
    twoforonepos1:
        sub  edx, z
        jns  twoforonepos2
        neg  edx
    twoforonepos2:
        cmp  eax, intensity                     ; Make sure Z is in light range
        ja   do2only
        cmp  edx, intensity
        ja   do1only

        mov  dh, bh                             ; XY dist 2
        mov  bh, al                             ; XY dist 1
        mov  al, DistTable[ebx]                 ; d-z plane distance
        mov  cl, DistTable[edx]                 ; d-z plane distance 2
        add  al, IntTable[eax]
        jc   nodraw1
        add  cl, IntTable[ecx]
        jc   nodraw2                            ; skip if dist too great

        mov  bl, uint8_t PTR [esi * 4 + 3]
        mov  bh, uint8_t PTR [esi * 4 + 7]
        and  ebx, 0FCFCh
        add  bl, LightDropOffTable[eax]         ; get brightness based on dist
        jnc  nochange1
        or   bl, 0FCh
    nochange1:
        add  bh, LightDropOffTable[ecx]         
        mov  uint8_t PTR [esi * 4 + 3], bl
        jnc  nochange2
        or   bh, 0FCh
    nochange2:
        mov  uint8_t PTR [esi * 4 + 7], bh
        jmp  next1

        ; **** Do only blocks

    do2only:
        cmp  edx, intensity
        ja   next1
        mov  dh, bh                             ; XY dist 2
        mov  cl, DistTable[edx]                 ; d-z plane distance 2
        add  cl, IntTable[ecx]
        jc   next1                              ; skip if dist too great

        mov  dl, uint8_t PTR [esi * 4 + 7]
        and  dl, 0FCh
        add  dl, LightDropOffTable[ecx]         
        jnc  do2onlynochange
        or   dl, 0FCh
    do2onlynochange:
        mov  uint8_t PTR [esi * 4 + 7], dl
        jmp  next1

    do1only:
        mov  bh, al                             ; XY dist 1
        mov  al, DistTable[ebx]                 ; d-z plane distance
        add  al, IntTable[eax]
        jc   next1

        mov  bl, uint8_t PTR [esi * 4 + 3]
        and  bl, 0FCh
        add  bl, LightDropOffTable[eax]         ; get brightness based on dist
        jnc  do1onlynochange
        or   bl, 0FCh
    do1onlynochange:
        mov  uint8_t PTR [esi * 4 + 3], bl
        jmp  next1

        ; **** Do no draw blocks

    nodraw1:
        add  cl, IntTable[ecx]                  ; d-z plane distance 2
        jc   next1                              ; skip if dist too great
        mov  dl, uint8_t PTR [esi * 4 + 7]
        and  dl, 0FCh
        add  dl, LightDropOffTable[ecx]         
        jnc  nodraw1nochange
        or   dl, 0FCh
    nodraw1nochange:
        mov  uint8_t PTR [esi * 4 + 7], dl
        jmp  next1

    nodraw2:
        mov  bl, uint8_t PTR [esi * 4 + 3]
        and  bl, 0FCh
        add  bl, LightDropOffTable[eax]         ; get brightness based on dist
        jnc  nodraw2nochange
        or   bl, 0FCh
    nodraw2nochange:
        mov  uint8_t PTR [esi * 4 + 3], bl
        jmp  next1

    next1:
        add  esi,2
        add  edi, xdir2

        cmp  edi, curxend
        jne  twoforone

    nexty1:
        add  esi, nextline

        mov  ecx, edi
        add  ch, ydir
        mov  curxend, ecx
        cmp  ch, uint8_t PTR yend
        jne  yloop1

        mov  esp, saveesp

//      rdtsc
//      mov endtime, eax
    }
    #endif

//  int32_t diff = (int32_t)(endtime - begtime);
//  int32_t pixels = (abs(xend - xpos) + 1) * (abs(yend - ypos) + 1);
//  int32_t cyclesperpixel = diff / pixels;
//  diff = 0;
}

bool DrawStaticLightNoNormalsFunc(PSDrawBlock db, PSDrawParam dp)
{
    RSLightData ld = *(PSLightData)dp->data;
    RSColor color = ld.color;
    int32_t intensity = ld.intensity;
    TSurface* surface = ld.surface;
    int32_t id = ld.id;

    if (dp->dwidth <= 0 || dp->dheight <= 0)
        return false;

  // Calculate center of light in this coordinate system
    int32_t x = dp->dx + dp->originx - dp->sx;  // Subtract source pos to get light center
    int32_t y = dp->dy + dp->originy - dp->sy;  
    int32_t z = ld.z;
    
    uint32_t *ubptr = (uint32_t *)db->dest;
    uint16_t *zbufptr = (uint16_t *)db->dzbuffer;

    SRect r;
    r.left   = dp->dx + dp->originx;
    r.top    = dp->dy + dp->originy;
    r.right  = r.left + dp->dwidth - 1;
    r.bottom = r.top + dp->dheight - 1;
        
    int32_t ptrpos, width, height, nextline;
    int32_t ypos, yend, xpos, xend;
    
    // Upper left quadrant
    int32_t xstart = x - intensity;
    int32_t ystart = y - intensity;
    width  = intensity;
    height = intensity;
    
    if ((xstart + width > r.left) && (ystart + height > r.top) &&
        (xstart <= r.right) && (ystart <= r.bottom))
    {
        xpos = width;
        ypos = height;
                  
        if (xstart < r.left)
        {
            width += (xstart - r.left);
            xpos  -= (r.left - xstart);
            xstart = r.left;
        }

        if (xstart + width > r.right + 1)
            width = r.right + 1 - xstart;

        if (ystart < r.top)
        {
            height += (ystart - r.top);
            ypos   -= (r.top - ystart);
            ystart  = r.top;
        }
    
        if (ystart + height > r.bottom + 1)
            height = r.bottom + 1 - ystart;
        
        xend = xpos - width;
        yend = ypos - height;
                
        ptrpos   = (ystart * db->dstride + xstart);
        nextline = (db->dstride - width);

        DrawStaticLightBlock(ptrpos, zbufptr, ubptr, z, intensity, nextline, xpos, xend, ypos, yend);
    }

    // Upper right quadrant
    xstart = x;
    ystart = y - intensity;
    width  = intensity;
    height = intensity;
    
    if ((xstart + width > r.left) && (ystart + height > r.top) &&
        (xstart <= r.right) && (ystart <= r.bottom))
    {
        xpos = 0;
        ypos = height;
                  
        if (xstart < r.left)
        {          
            width += (xstart - r.left);
            xpos  += (r.left - xstart);
            xstart = r.left;
        }
        
        if (xstart + width > r.right + 1)
            width = r.right + 1 - xstart;

        if (ystart < r.top)
        {
            height += (ystart - r.top);
            ypos   -= (r.top - ystart);
            ystart  = r.top;
        }
        
        if (ystart + height > r.bottom + 1)
            height = r.bottom + 1 - ystart;
            
        xend = xpos + width;
        yend = ypos - height;
                    
        ptrpos   = (ystart * db->dstride + xstart);
        nextline = (db->dstride - width);
          
        DrawStaticLightBlock(ptrpos, zbufptr, ubptr, z, intensity, nextline, xpos, xend, ypos, yend);
    }
    
    // Lower left quadrant
    xstart = x - intensity;
    ystart = y;
    width  = intensity;
    height = intensity;
    
    if ((xstart + width > r.left) && (ystart + height > r.top) &&
        (xstart <= r.right) && (ystart <= r.bottom))
    {
        xpos = width;
        ypos = 0;
                    
        if (xstart < r.left)
        {
            width += (xstart - r.left);
            xpos  -= (r.left - xstart);
            xstart = r.left;
        }

        if (xstart + width > r.right + 1)
            width = r.right + 1 - xstart;

        if (ystart < r.top)
        {
            height += (ystart - r.top);
            ypos   += (r.top - ystart);
            ystart  = r.top;
        }

        if (ystart + height > r.bottom + 1)
            height = r.bottom + 1 - ystart;
                
        xend = xpos - width;
        yend = ypos + height;
                            
        ptrpos   = (ystart * db->dstride + xstart);
        nextline = (db->dstride - width);
                      
        DrawStaticLightBlock(ptrpos, zbufptr, ubptr, z, intensity, nextline, xpos, xend, ypos, yend);
    }

  // Lower right quadrant
    xstart = x;
    ystart = y;
    width  = intensity;
    height = intensity;

    if ((xstart + width > r.left) && (ystart + height > r.top) &&
        (xstart <= r.right) && (ystart <= r.bottom))
    {
        xpos = 0;
        ypos = 0;

        if (xstart < r.left)
        {
            width += (xstart - r.left);
            xpos  += (r.left - xstart);
            xstart = r.left;
        }

        if (xstart + width > r.right + 1)
            width = r.right + 1 - xstart;
        
        if (ystart < r.top)
        {
            height += (ystart - r.top);
            ypos   += (r.top - ystart);
            ystart  = r.top;
        }
        
        if (ystart + height > r.bottom + 1)
            height = r.bottom + 1 - ystart;

        xend = xpos + width;
        yend = ypos + height;
        
        ptrpos   = (ystart * db->dstride + xstart);
        nextline = (db->dstride - width);

        DrawStaticLightBlock(ptrpos, zbufptr, ubptr, z, intensity, nextline, xpos, xend, ypos, yend);
    }

    return true;
}

void DrawStaticLightNoNormals(S3DPoint& pos, RSColor color, int32_t intensity, TSurface* surface, int32_t id)
{
    if (surface->BitsPerPixel() != 32 || id >= NUMBASELIGHTS)
        return;

    if (intensity > 254)
        intensity = 254;

    if (intensity < 0)
        intensity = 0;

    uint8_t coloridx = id & 3;
    int32_t intratio = (254 << 16) / intensity + 256;
    int32_t intaccum = 0;
    for (int32_t c = 0; c < 256; c++)
    {
      // Set the color id for the lighting table
      LightDropOffTable[c] = LightDropOffTable[c] & 0xFC | coloridx;

      // Set the intensity ramp for the intensity table
      intaccum += intratio;
      IntTable[c] = (uint8_t)(min((intaccum >> 16) - c, 255));
    }

    SLightData ld;
    ld.z = pos.z;
    ld.color = color;
    ld.intensity = intensity;
    ld.surface = surface;
    ld.id = id;

    int32_t x = pos.x - intensity;
    int32_t y = pos.y - intensity;
    int32_t w = intensity * 2;
    int32_t h = intensity * 2;

    SDrawParam dp;
    MakeDP(dp, x, y, -intensity, -intensity, w, h, DM_ZBUFFER);
    dp.func = DrawStaticLightNoNormalsFunc;
    dp.data = (void *)&ld;

    surface->ParamDraw(&dp);
}

bool DrawAmbientLightFunc(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    SETUP_DRAW

    int32_t width  = dp->dwidth;
    int32_t height = dp->dheight;

    int32_t ypos = dp->dheight;
    
    uintptr_t dstptr  = (uintptr_t)db->dest + dstoff;
      
    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
        mov  esi, dstptr

        xor  eax, eax
        xor  edx, edx

    ayloop1:
        mov  ebx, width

    axloop1:
        mov  uint8_t PTR [esi + 3], dh
        add  esi, 4

        dec  ebx
        jne  axloop1

        add  esi, dstadd

        dec  ypos
        jne  ayloop1
    }
    #endif

    return true;
}

void DrawAmbientLight(TSurface* surface, RSRect r)
{
    SDrawParam dp;
    MakeDPNoSrc(dp, r.x(), r.y(), r.w(), r.h(), DM_DEFAULT);
    dp.func = DrawAmbientLightFunc;

    surface->ParamDraw(&dp);
}

bool Transfer32to16MMXFunc(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    SETUP_DRAW

    uintptr_t srcptr = (uintptr_t)db->source;
    srcptr += srcoff;
    uintptr_t dstptr = (uintptr_t)db->dest;
    dstptr += dstoff;

    int32_t height = dp->dheight;

    // Note: we're doing two pixels at a time below.. Make sure that the paired pixels
    // are uint32_t aligned by finding out whether the beginning and ending of the line
    // is even or odd, then draw the first pixel, all the pairs, then the last pixel.

    uint32_t dowidth = (dp->dwidth << 1);
    uint32_t dofirst = 0;
    uint32_t dolast = 0;
    if (dowidth < 8)
    {
        dofirst = dowidth;
        dolast = 0;
        dowidth = 0;
    }
    else
    {
      if ((dstptr & 7) != 0)  // Is first pixel uint32_t aligned
      {
        dofirst = dstptr & 7;
        dowidth -= dofirst;
      }
      if ((dowidth & 7) != 0)
      {
        dolast = dowidth & 7;
        dowidth -= dolast;
      }
    }

    uint32_t loopdone = 0;

    // Optimization notes:
    //
    // The tables below are big (64k or 128k each), but this shouldn't hash the cache
    // since the lower 8 bits holds only a 5 bit color value whose lookup fits neatly into
    // one cache entry, and the upper 8 bits change only on the brightness value (upper 6 bits).
    // The Blue and Red table should only use about 2k in cache entries (64 brightness values
    // times 32 color values times 1 byte per value), and the Green table should only be about
    // 4k (2k times 2 bytes per value) for a total cache use of only 8k, which fits inside
    // the on chip cache no problem.
    //
    // In essence, there are large numbers of empty 32 byte chunks in the lookup tables
    // that are never loaded into the cache.

    uint8_t BLUEMASK = 0x1F;
    uint8_t REDMASK = 0xF8;
    uint32_t GREENMASK = 0x07E0;

    uint64_t ZERO = 0;
    uint64_t MULFACT = 0x0008200000082000;
    uint64_t REDBLUE = 0x00F800F800F800F8;
    uint64_t GREEN =   0x0000F8000000F800;
    uint64_t MASKBLUE16 =0x7FE07FE07FE07FE0;
    uint64_t TEST = 0;

//  uint32_t begtime, endtime;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
//      rdtsc
//      mov begtime, eax
//      rdtsc
//      mov endtime, eax

        mov  edi, dstptr
        mov  esi, srcptr

        xor  eax, eax
        xor  ebx, ebx
        xor  ecx, ecx

    DoFirst:
        mov  ecx, dofirst
        or   ecx, ecx
        je   DoPairs
        add  ecx, edi
        mov  loopdone, ecx
        xor  ebx, ebx

    DoFirstLoop:
        mov         bl, [esi + 3]           ; Get light value 1
        movq        mm0, [esi]              ; Get RGB value 1-2
        movd        mm2, MMXLightTable[ebx * 4] ; Get RGB multiplier 1
        punpcklbw   mm0, ZERO               ; Unpack RGB 1
        punpcklbw   mm2, ZERO               ; Unpack light value 1
        pmullw      mm0, mm2                ; Multiply pixel 1
        packuswb    mm0, ZERO               ; Pack RGB words back to bytes 1-2
        movq        mm1, mm0                ; Save the original data 1-2
        pand        mm0, REDBLUE            ; Mask out all but hte 5 MSBits of red and blue
        pmaddwd     mm0, MULFACT            ; Multiply each word by 2**13, 2**3, 2**13, 2**3
        pand        mm1, GREEN              ; Mask out all but the 4 MSBits of green
        por         mm0, mm1                ; Combine the red, green, and blue bits
        psrld       mm0, 6                  ; Shift to final position
        packssdw    mm0, ZERO               ; Pack back to dword all four pixels
        movq        mm1, mm0                ; CONVERT 15->16 BIT Copy final words
        pand        mm1, MASKBLUE16         ; CONVERT 15->16 BIT Mask blue 16 bit
        paddusw     mm0, mm1                ; CONVERT 15->16 BIT Double Red/Green to 16 bit
        movd        eax, mm0                ; Copy to 32 bit reg

        add         esi, 4
        mov         [edi], ax
        add         edi, 2
        cmp         edi, loopdone
        jne         DoFirstLoop

    DoPairs:
        mov  ecx, dowidth
        or   ecx, ecx
        je   DoLast
        add  ecx, edi
        mov  loopdone, ecx
        xor  ecx, ecx

//      rdtsc
//      mov begtime, eax

        xor  eax, eax
        xor  ebx, ebx
        xor  ecx, ecx
        xor  edx, edx

    DoMainLoop:
        mov         al, [esi + 3]           ; Get light value 1
        mov         bl, [esi + 7]           ; Get light value 2
        mov         cl, [esi + 11]          ; Get light value 3
        mov         dl, [esi + 15]          ; Get light value 4
        movq        mm0, [esi]              ; Get RGB value 1-2
        movq        mm4, [esi + 8]          ; Get RGB value 3-4
        add         esi, 16
        movd        mm2, MMXLightTable[eax * 4] ; Get RGB multiplier 1
        movd        mm3, MMXLightTable[ebx * 4] ; Get RGB multiplier 2
        movd        mm6, MMXLightTable[ecx * 4] ; Get RGB multiplier 3
        movd        mm7, MMXLightTable[edx * 4] ; Get RGB multiplier 4
        movq        mm1, mm0                ; Copy 1-2
        movq        mm5, mm4                ; Copy 3-4
        punpcklbw   mm0, ZERO               ; Unpack RGB 1
        punpckhbw   mm1, ZERO               ; Unpack RGB 2
        punpcklbw   mm2, ZERO               ; Unpack light value 1
        punpcklbw   mm3, ZERO               ; Unpack light value 2
        punpcklbw   mm4, ZERO               ; Unpack RGB 3
        punpckhbw   mm5, ZERO               ; Unpack RGB 4
        punpcklbw   mm6, ZERO               ; Unpack light value 3
        punpcklbw   mm7, ZERO               ; Unpack light value 4
        pmullw      mm0, mm2                ; Multiply pixel 1
        pmullw      mm1, mm3                ; Multiply pixel 2
        pmullw      mm4, mm6                ; Multiply pixel 3
        pmullw      mm5, mm7                ; Multiply pixel 4
        packuswb    mm0, mm1                ; Pack RGB words back to bytes 1-2
        packuswb    mm4, mm5                ; Pack 3-4
        movq        mm1, mm0                ; Save the original data 1-2
        movq        mm5, mm4                ; Save 3-4
        pand        mm0, REDBLUE            ; Mask out all but hte 5 MSBits of red and blue
        pand        mm4, REDBLUE            ; Mask 3-4
        pmaddwd     mm0, MULFACT            ; Multiply each word by 2**13, 2**3, 2**13, 2**3
        pmaddwd     mm4, MULFACT            ; Multiply 3-4
        pand        mm1, GREEN              ; Mask out all but the 4 MSBits of green
        pand        mm5, GREEN              ; Mask 3-4
        por         mm0, mm1                ; Combine the red, green, and blue bits
        por         mm4, mm5                ; Combine 3-4
        psrld       mm0, 6                  ; Shift to final position
        psrld       mm4, 6                  ; Shift 3-4 
        packssdw    mm0, mm4                ; Pack back to dword all four pixels
        movq        mm1, mm0                ; CONVERT 15->16 BIT Copy final words
        pand        mm1, MASKBLUE16         ; CONVERT 15->16 BIT Mask blue 16 bit
        paddusw     mm0, mm1                ; CONVERT 15->16 BIT Double Red/Green to 16 bit

        movq        [edi], mm0
        add  edi, 8

        cmp  edi, loopdone
        jne  DoMainLoop

//      rdtsc
//      mov endtime, eax

    DoLast:
        mov  ecx, dolast
        or   ecx, ecx
        je  YDone
        add  ecx, edi
        mov  loopdone, ecx
        mov  edx, LightTable
        xor  ebx, ebx

    DoLastLoop:
        mov         bl, [esi + 3]           ; Get light value 1
        movq        mm0, [esi]              ; Get RGB value 1-2
        movd        mm2, MMXLightTable[ebx * 4] ; Get RGB multiplier 1
        punpcklbw   mm0, ZERO               ; Unpack RGB 1
        punpcklbw   mm2, ZERO               ; Unpack light value 1
        pmullw      mm0, mm2                ; Multiply pixel 1
        packuswb    mm0, ZERO               ; Pack RGB words back to bytes 1-2
        movq        mm1, mm0                ; Save the original data 1-2
        pand        mm0, REDBLUE            ; Mask out all but hte 5 MSBits of red and blue
        pmaddwd     mm0, MULFACT            ; Multiply each word by 2**13, 2**3, 2**13, 2**3
        pand        mm1, GREEN              ; Mask out all but the 4 MSBits of green
        por         mm0, mm1                ; Combine the red, green, and blue bits
        psrld       mm0, 6                  ; Shift to final position
        packssdw    mm0, ZERO               ; Pack back to dword all four pixels
        movq        mm1, mm0                ; CONVERT 15->16 BIT Copy final words
        pand        mm1, MASKBLUE16         ; CONVERT 15->16 BIT Mask blue 16 bit
        paddusw     mm0, mm1                ; CONVERT 15->16 BIT Double Red/Green to 16 bit
        movd        eax, mm0                ; Copy to 32 bit reg

        add         esi, 4
        mov         [edi], ax
        add         edi, 2
        cmp         edi, loopdone
        jne         DoLastLoop

    YDone:
        add  esi, srcadd
        add  edi, dstadd

        dec  height
        jne  DoFirst

//      rdtsc
//      mov endtime, eax

        emms
    }
    #endif

//  int32_t diff = (int32_t)(endtime - begtime);
//  int32_t pixels = dp->swidth * dp->sheight;
//  int32_t cyclesperpixel = diff / pixels;

    return true;
}

bool Transfer32to15MMXFunc(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    SETUP_DRAW

    uintptr_t srcptr = (uintptr_t)db->source;
    srcptr += srcoff;
    uintptr_t dstptr = (uintptr_t)db->dest;
    dstptr += dstoff;

    int32_t height = dp->dheight;

    // Note: we're doing two pixels at a time below.. Make sure that the paired pixels
    // are uint32_t aligned by finding out whether the beginning and ending of the line
    // is even or odd, then draw the first pixel, all the pairs, then the last pixel.

    uint32_t dowidth = (dp->dwidth << 1);
    uint32_t dofirst = 0;
    uint32_t dolast = 0;
    if (dowidth < 8)
    {
        dofirst = dowidth;
        dolast = 0;
        dowidth = 0;
    }
    else
    {
      if ((dstptr & 7) != 0)  // Is first pixel uint32_t aligned
      {
        dofirst = dstptr & 7;
        dowidth -= dofirst;
      }
      if ((dowidth & 7) != 0)
      {
        dolast = dowidth & 7;
        dowidth -= dolast;
      }
    }

    uint32_t loopdone = 0;

    // Optimization notes:
    //
    // The tables below are big (64k or 128k each), but this shouldn't hash the cache
    // since the lower 8 bits holds only a 5 bit color value whose lookup fits neatly into
    // one cache entry, and the upper 8 bits change only on the brightness value (upper 6 bits).
    // The Blue and Red table should only use about 2k in cache entries (64 brightness values
    // times 32 color values times 1 byte per value), and the Green table should only be about
    // 4k (2k times 2 bytes per value) for a total cache use of only 8k, which fits inside
    // the on chip cache no problem.
    //
    // In essence, there are large numbers of empty 32 byte chunks in the lookup tables
    // that are never loaded into the cache.

    uint8_t BLUEMASK = 0x1F;
    uint8_t REDMASK = 0x7C;
    uint32_t GREENMASK = 0x03E0;

    uint64_t ZERO = 0;
    uint64_t MULFACT = 0x0008200000082000;
    uint64_t REDBLUE = 0x00F800F800F800F8;
    uint64_t GREEN =   0x0000F8000000F800;
    uint64_t MASKBLUE16 =0x7FE07FE07FE07FE0;
    uint64_t TEST = 0;

//  uint32_t begtime, endtime;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
//      rdtsc
//      mov begtime, eax
//      rdtsc
//      mov endtime, eax

        mov  edi, dstptr
        mov  esi, srcptr

        xor  eax, eax
        xor  ebx, ebx
        xor  ecx, ecx

    DoFirst:
        mov  ecx, dofirst
        or   ecx, ecx
        je   DoPairs
        add  ecx, edi
        mov  loopdone, ecx
        xor  ebx, ebx

    DoFirstLoop:
        mov         bl, [esi + 3]           ; Get light value 1
        movd        mm0, [esi]              ; Get RGB value 1
        movd        mm2, MMXLightTable[ebx * 4] ; Get RGB multiplier 1
        punpcklbw   mm0, ZERO               ; Unpack RGB 1
        punpcklbw   mm2, ZERO               ; Unpack light value 1
        pmullw      mm0, mm2                ; Multiply pixel 1
        packuswb    mm0, ZERO               ; Pack RGB words back to bytes 1
        movq        mm1, mm0                ; Save the original data 1
        pand        mm0, REDBLUE            ; Mask out all but hte 5 MSBits of red and blue
        pmaddwd     mm0, MULFACT            ; Multiply each word by 2**13, 2**3, 2**13, 2**3
        pand        mm1, GREEN              ; Mask out all but the 4 MSBits of green
        por         mm0, mm1                ; Combine the red, green, and blue bits
        psrld       mm0, 6                  ; Shift to final position
        packssdw    mm0, ZERO               ; Pack back to dword all four pixels
        movd        eax, mm0                ; Put in regular 32 bit reg

        add         esi, 4
        mov         [edi], ax               ; Put word to output buffer
        add         edi, 2
        cmp         edi, loopdone
        jne         DoFirstLoop

    DoPairs:
        mov  ecx, dowidth
        or   ecx, ecx
        je   DoLast
        add  ecx, edi
        mov  loopdone, ecx
        xor  ecx, ecx

//      rdtsc
//      mov begtime, eax

        xor  eax, eax
        xor  ebx, ebx
        xor  ecx, ecx
        xor  edx, edx

    DoMainLoop:
        mov         al, [esi + 3]           ; Get light value 1
        mov         bl, [esi + 7]           ; Get light value 2
        mov         cl, [esi + 11]          ; Get light value 3
        mov         dl, [esi + 15]          ; Get light value 4
        movq        mm0, [esi]              ; Get RGB value 1-2
        movq        mm4, [esi + 8]          ; Get RGB value 3-4
        add         esi, 16
        movd        mm2, MMXLightTable[eax * 4] ; Get RGB multiplier 1
        movd        mm3, MMXLightTable[ebx * 4] ; Get RGB multiplier 2
        movd        mm6, MMXLightTable[ecx * 4] ; Get RGB multiplier 1
        movd        mm7, MMXLightTable[edx * 4] ; Get RGB multiplier 2
        movq        mm1, mm0                ; Copy 1-2
        movq        mm5, mm4                ; Copy 3-4
        punpcklbw   mm0, ZERO               ; Unpack RGB 1
        punpckhbw   mm1, ZERO               ; Unpack RGB 2
        punpcklbw   mm2, ZERO               ; Unpack light value 1
        punpcklbw   mm3, ZERO               ; Unpack light value 2
        punpcklbw   mm4, ZERO               ; Unpack RGB 3
        punpckhbw   mm5, ZERO               ; Unpack RGB 4
        punpcklbw   mm6, ZERO               ; Unpack light value 3
        punpcklbw   mm7, ZERO               ; Unpack light value 4
        pmullw      mm0, mm2                ; Multiply pixel 1
        pmullw      mm1, mm3                ; Multiply pixel 2
        pmullw      mm4, mm6                ; Multiply pixel 3
        pmullw      mm5, mm7                ; Multiply pixel 4
        packuswb    mm0, mm1                ; Pack RGB words back to bytes 1-2
        packuswb    mm4, mm5                ; Pack 3-4
        movq        mm1, mm0                ; Save the original data 1-2
        movq        mm5, mm4                ; Save 3-4
        pand        mm0, REDBLUE            ; Mask out all but hte 5 MSBits of red and blue
        pand        mm4, REDBLUE            ; Mask 3-4
        pmaddwd     mm0, MULFACT            ; Multiply each word by 2**13, 2**3, 2**13, 2**3
        pmaddwd     mm4, MULFACT            ; Multiply 3-4
        pand        mm1, GREEN              ; Mask out all but the 4 MSBits of green
        pand        mm5, GREEN              ; Mask 3-4
        por         mm0, mm1                ; Combine the red, green, and blue bits
        por         mm4, mm5                ; Combine 3-4
        psrld       mm0, 6                  ; Shift to final position
        psrld       mm4, 6                  ; Shift 3-4 
        packssdw    mm0, mm4                ; Pack back to dword all four pixels

        movq        [edi], mm0
        add  edi, 8

        cmp  edi, loopdone
        jne  DoMainLoop

//      rdtsc
//      mov endtime, eax

    DoLast:
        mov  ecx, dolast
        or   ecx, ecx
        je  YDone
        add  ecx, edi
        mov  loopdone, ecx
        xor  ebx, ebx

    DoLastLoop:
        mov         bl, [esi + 3]           ; Get light value 1
        movd        mm0, [esi]              ; Get RGB value 1
        movd        mm2, MMXLightTable[ebx * 4] ; Get RGB multiplier 1
        punpcklbw   mm0, ZERO               ; Unpack RGB 1
        punpcklbw   mm2, ZERO               ; Unpack light value 1
        pmullw      mm0, mm2                ; Multiply pixel 1
        packuswb    mm0, ZERO               ; Pack RGB words back to bytes 1
        movq        mm1, mm0                ; Save the original data 1
        pand        mm0, REDBLUE            ; Mask out all but hte 5 MSBits of red and blue
        pmaddwd     mm0, MULFACT            ; Multiply each word by 2**13, 2**3, 2**13, 2**3
        pand        mm1, GREEN              ; Mask out all but the 4 MSBits of green
        por         mm0, mm1                ; Combine the red, green, and blue bits
        psrld       mm0, 6                  ; Shift to final position
        packssdw    mm0, ZERO               ; Pack back to dword all four pixels
        movd        eax, mm0                ; Put in regular 32 bit reg

        add         esi, 4
        mov         [edi], ax               ; Put word to output buffer
        add         edi, 2
        cmp         edi, loopdone
        jne         DoLastLoop

    YDone:
        add  esi, srcadd
        add  edi, dstadd

        dec  height
        jne  DoFirst

//      rdtsc
//      mov endtime, eax

        emms
    }
    #endif

//  int32_t diff = (int32_t)(endtime - begtime);
//  int32_t pixels = dp->swidth * dp->sheight;
//  int32_t cyclesperpixel = diff / pixels;

    return true;
}

bool Transfer32to16Func(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->swidth < 1 || dp->sheight < 1 || dp->dwidth < 1 || dp->dheight < 1) 
        return false;

    SETUP_DRAW

    uintptr_t srcptr = (uintptr_t)db->source;
    srcptr += srcoff;
    uintptr_t dstptr = (uintptr_t)db->dest;
    dstptr += dstoff;

    int32_t height = dp->dheight;

    // Note: we're doing two pixels at a time below.. Make sure that the paired pixels
    // are uint32_t aligned by finding out whether the beginning and ending of the line
    // is even or odd, then draw the first pixel, all the pairs, then the last pixel.

    uint8_t dofirst = 0;
    if ((dstptr & 3) != 0 || dp->dwidth == 1)  // Is first pixel uint32_t aligned
        dofirst = 1;
    uint8_t dolast = 0;                                    // Is last pixel uint32_t aligned
    if (((dstptr + (dp->dwidth << 1)) & 3) != 0 && dp->dwidth > 1)
        dolast = 1;
    uint32_t dopairs = (dp->dwidth - dofirst - dolast) << 1;   // Get number of paired bytes
    uint32_t pairsdone = 0;

    uint32_t saveesp = 0;

    uint8_t BLUEMASK = 0x1F;
    uint8_t REDMASK = (Display.BitsPerPixel() == 15) ? 0x7C : 0xF8;
    uint32_t GREENMASK = (Display.BitsPerPixel() == 15) ? 0x03E0 : 0x07E0;

    // Optimization notes:
    //
    // The tables below are big (64k or 128k each), but this shouldn't hash the cache
    // since the lower 8 bits holds only a 5 bit color value whose lookup fits neatly into
    // one cache entry, and the upper 8 bits change only on the brightness value (upper 6 bits).
    // The Blue and Red table should only use about 2k in cache entries (64 brightness values
    // times 32 color values times 1 byte per value), and the Green table should only be about
    // 4k (2k times 2 bytes per value) for a total cache use of only 8k, which fits inside
    // the on chip cache no problem.
    //
    // In essence, there are large numbers of empty 32 byte chunks in the lookup tables
    // that are never loaded into the cache.


//  uint32_t begtime, endtime;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
//      rdtsc
//      mov begtime, eax
//      rdtsc
//      mov endtime, eax

        mov  saveesp, esp

        mov  edi, dstptr
        mov  esi, srcptr
        mov  esp, LightTable

        xor  eax, eax
        xor  ebx, ebx
        xor  ecx, ecx
        xor  edx, edx

    DoFirstPixel:
        mov  al, dofirst
        or   al, al
        je   DoPairs

        mov  bh, uint8_t PTR [esi + 3]
                
        mov  bl, uint8_t PTR [esi + 2]
        mov  al, [esp + ebx * 2]                ; get blue
        and  al, BLUEMASK
        mov  bl, uint8_t PTR [esi]
        mov  ah, [esp + ebx * 2 + 1]            ; get red
        and  ah, REDMASK
        mov  bl, uint8_t PTR [esi + 1]  
        mov  cl, [esp + ebx * 2]                ; l/u green & blue
        mov  ch, [esp + ebx * 2 + 1]            ; l/u green & blue
        and  ecx, GREENMASK
        or   eax, ecx

        add  esi, 4

        mov  [edi], ax
        add  edi, 2

    DoPairs:
        mov  ecx, dopairs
        or   ecx, ecx
        je   DoLastPixel
        add  ecx, edi
        mov  pairsdone, ecx
//      xor  eax, eax

    DoPairsLoop:
        mov  bh, uint8_t PTR [esi + 3]
        mov  dh, uint8_t PTR [esi + 7]
                
        mov  bl, uint8_t PTR [esi + 2]
        mov  dl, uint8_t PTR [esi + 6]
        mov  al, [esp + ebx * 2]                ; get blue
        mov  cl, [esp + edx * 2]
        and  al, BLUEMASK
        and  cl, BLUEMASK
        mov  bl, uint8_t PTR [esi]
        mov  dl, uint8_t PTR [esi + 4]                        
        mov  ah, [esp + ebx * 2 + 1]            ; get red
        mov  ch, [esp + edx * 2 + 1]
        and  ah, REDMASK
        and  ch, REDMASK
        mov  bl, uint8_t PTR [esi + 1]  
        mov  dl, [esp + ebx * 2]                ; l/u green & blue
        and  dl, uint8_t PTR [GREENMASK]
        or   al, dl
        mov  dl, uint8_t PTR [esp + ebx * 2 + 1]
        and  dl, uint8_t PTR [GREENMASK + 1]
        or   ah, dl
        mov  dl, uint8_t PTR [esi + 5]                      
        mov  bl, [esp + edx * 2]            ; l/u green & blue
        mov  bh, [esp + edx * 2 + 1]
        and  ebx, GREENMASK
        or   ecx, ebx

        add  esi, 8

        shl  ecx, 16
        or   ecx, eax

        mov  [edi], ecx
        add  edi, 4

        cmp  edi, pairsdone
        jne  DoPairsLoop

    DoLastPixel:
        mov  al, dolast
        or   al, al
        je  YDone

        mov  bh, uint8_t PTR [esi + 3]
                
        mov  bl, uint8_t PTR [esi + 2]
        mov  al, [esp + ebx * 2]                ; get blue
        and  al, BLUEMASK
        mov  bl, uint8_t PTR [esi]
        mov  ah, [esp + ebx * 2 + 1]            ; get red
        and  ah, REDMASK
        mov  bl, uint8_t PTR [esi + 1]  
        mov  cl, [esp + ebx * 2]                ; l/u green & blue
        mov  ch, [esp + ebx * 2 + 1]            ; l/u green & blue
        and  ecx, GREENMASK
        or   eax, ecx

        add  esi, 4

        mov  [edi], ax
        add  edi, 2

    YDone:
        add  esi, srcadd
        add  edi, dstadd

        dec  height
        jne  DoFirstPixel

        mov  esp, saveesp

//      rdtsc
//      mov endtime, eax
    }
    #endif

//  int32_t diff = (int32_t)(endtime - begtime);
//  int32_t pixels = dp->swidth * dp->sheight;
//  int32_t cyclesperpixel = diff / pixels;

    return true;
}

void TransferAndLight32to16(TSurface* dest, TSurface* source, RSRect r)
{
    SDrawParam dp;
    MakeDP(dp, r.x(), r.y(), r.x(), r.y(), r.w(), r.h(), DM_WRAPCLIP | DM_WRAPCLIPSRC);

    if (!IsMMX)
        dp.func = Transfer32to16Func;
    else
    {
        if (Display.BitsPerPixel() == 16)
            dp.func = Transfer32to16MMXFunc;
        else
            dp.func = Transfer32to15MMXFunc;
    }

    dest->ParamBlit(&dp, source);
}

#if 0

// Transfer/Light for 15 bit source buffer
    DoMainLoop:
        mov         al, [esi + 3]           ; Get light value 1
        mov         bl, [esi + 7]           ; Get light value 2
        mov         cl, [esi + 11]          ; Get light value 3
        mov         dl, [esi + 15]          ; Get light value 4

        movq        mm0, [esp]              ; Get RGB value 1-4 (Convert from 16 to 32
        movq        mm1, mm0
        punpcklwd   mm0, ZERO
        punpckhwd   mm1, ZERO
        pslld       mm0, 6
        pslld       mm1, 6
        movq        mm2, mm0
        movq        mm3, mm1
        pmaddwd     mm0, SPLITFACT
        pmaddwd     mm1, SPLITFACT
        pand        mm2, GREEN
        pand        mm3, GREEN
        por         mm0, mm2
        por         mm1, mm3

        add         esi, 4
        add         esp, 8

        movd        mm2, MMXLightTable[eax * 4] ; Get RGB multiplier 1
        movd        mm3, MMXLightTable[ebx * 4] ; Get RGB multiplier 2
        movd        mm6, MMXLightTable[ecx * 4] ; Get RGB multiplier 1
        movd        mm7, MMXLightTable[edx * 4] ; Get RGB multiplier 2
        movq        mm1, mm0                ; Copy 1-2
        movq        mm5, mm4                ; Copy 3-4
        punpcklbw   mm0, ZERO               ; Unpack RGB 1
        punpckhbw   mm1, ZERO               ; Unpack RGB 2
        punpcklbw   mm2, ZERO               ; Unpack light value 1
        punpcklbw   mm3, ZERO               ; Unpack light value 2
        punpcklbw   mm4, ZERO               ; Unpack RGB 3
        punpckhbw   mm5, ZERO               ; Unpack RGB 4
        punpcklbw   mm6, ZERO               ; Unpack light value 3
        punpcklbw   mm7, ZERO               ; Unpack light value 4
        pmullw      mm0, mm2                ; Multiply pixel 1
        pmullw      mm1, mm3                ; Multiply pixel 2
        pmullw      mm4, mm6                ; Multiply pixel 3
        pmullw      mm5, mm7                ; Multiply pixel 4
        packuswb    mm0, mm1                ; Pack RGB words back to bytes 1-2
        packuswb    mm4, mm5                ; Pack 3-4

        movq        mm1, mm0                ; Save the original data 1-2
        movq        mm5, mm4                ; Save 3-4
        pand        mm0, REDBLUE            ; Mask out all but hte 5 MSBits of red and blue
        pand        mm4, REDBLUE            ; Mask 3-4
        pmaddwd     mm0, MULFACT            ; Multiply each word by 2**13, 2**3, 2**13, 2**3
        pmaddwd     mm4, MULFACT            ; Multiply 3-4
        pand        mm1, GREEN              ; Mask out all but the 4 MSBits of green
        pand        mm5, GREEN              ; Mask 3-4
        por         mm0, mm1                ; Combine the red, green, and blue bits
        por         mm4, mm5                ; Combine 3-4
        psrld       mm0, 6                  ; Shift to final position
        psrld       mm4, 6                  ; Shift 3-4 
        packssdw    mm0, mm4                ; Pack back to dword all four pixels
        movq        mm1, mm0                ; CONVERT 15->16 BIT Copy final words
        pand        mm1, MASKBLUE16         ; CONVERT 15->16 BIT Mask blue 16 bit
        paddusw     mm0, mm1                ; CONVERT 15->16 BIT Double Red/Green to 16 bit


/*      This WONT WORK!!


        movq    mm0, [esi]                          ; Get z buffer values
        psubw   mm0, LIGHTPOS                       ; Get dist from light
        movq    mm1, mm0                            ; Copy results
        pcmpgtw ZERO, mm1                           ; Get not mask to reverse neg values
        pxor    mm0, ONE                            ; Get absolute values (though neg is 1 off)
        packuswb mm0, ZERO                          ; Pack and saturate down to bytes
        punpcklbw mm0, ZERO                         ; Return to words with 255 maximum value
        pxor    mm0, REVERSE                        ; Reverse directon of low bytes
        paddusw mm0, ADDVAL                         ; Add val (to flatten exp curve)
        movd    mm1, [esp]                          ; Get dist values from table
        punpcklbw mm1, ZERO                         ; Unpack
        paddus  mm1, ADDVAL                         ; Add val (to flaten exp curve)
        movq    mm2, mm0
        pmullw  mm0, mm0                            ; Square distance (should exactly wrap to 10000)
        pmulhw  mm2, mm2
        movq    mm3, mm0
        pmullw  mm1, mm1                            ; Square distance
        pmulhw  mm3, mm3                            ; Square distance
        paddusw mm0, mm1
        paddusw mm2, mm3
        pcmpgtw mm2, ZERO                           ; Saturate low word by checking hi word for stuff
        por     mm0, mm2                            ; Then or'ing low word with full mask if bits found in high
        psub    mm0, SUBVAL                         ; Sub val (to flaten exp curve)
        psrlw   mm0, 8                              ; Divide (to get lighting brightness)
        pand    mm0, LIGHTCODEMASK                  ; Put lighting code in bottom byte
        por     mm0, LIGHTCODE                      
        packuswb mm0, ZERO
        movd    [edi], mm0                          ; Save out final result

        pcmpgtw mm3, ZERO
        por     mm1, 
        movq    mm3, mm0
        paddus  mm0, mm1
        psrlw   mm0, 10                             ; Divide result by 1024 (to flatten exp curve)

*/


void DrawStaticLightBlockMMX(int32_t ptrpos, uint16_t *zbufptr, uint32_t *ubptr, 
    uint32_t z, int32_t intensity, int32_t nextline, uint8_t xpos, uint8_t xend, uint8_t ypos, uint8_t yend) 
{
    if (ypos == yend || xpos == xend)
        return;

    int32_t xdir, xdir2;
    if (xpos > xend)
        xdir = -1;
    else
        xdir = 1;
    xdir2 = xdir + xdir;

    char ydir;
    if (ypos > yend)
        ydir = -1;
    else
        ydir = 1;

    uint32_t curxend = 0;
    uint8_t iseven = !(abs(xend - xpos) & 1);

    uintptr_t ubpos = ptrpos + ((uintptr_t)ubptr / 4);
    uintptr_t zoffset = (uintptr_t)zbufptr - ((uintptr_t)ubptr / 2);

    uint32_t saveesp = 0;

//  uint32_t begtime, endtime;

    #if 0 // TODO(port): MMX/x86 inline assembly — Phase 3 blit rewrite
    __asm
    {
//      rdtsc
//      mov begtime, eax
//      rdtsc
//      mov endtime, eax

        mov  saveesp, esp

        mov  esi, ubpos
        mov  esp, zoffset
        xor  eax, eax
        xor  ebx, ebx
        xor  ecx, ecx
        xor  edx, edx

        mov  ch, ypos           
        mov  cl, xend

    yloop1:
        mov  curxend, ecx
        mov  cl, xpos
        mov  edi, ecx
        xor  ecx, ecx
        
        mov  al, iseven
        or   al, al
        jne  twoforone

        mov  al, DistTable[edi]                 ; Get XY dist
        cmp  al, 0FFh
        je singlenext1
        
        mov  bl, [esp + esi * 2]                ; Get Z
        mov  bh, [esp + esi * 2 + 1]
        sub  ebx, z
        jns  singlepos
        neg  ebx
    singlepos:
        cmp  ebx, intensity                     ; Make sure Z is in light range
        ja   singlenext1
        
        mov  bh, al
        mov  al, DistTable[ebx]                 ; d-z plane distance
        add  al, IntTable[eax]

        jc   singlenext1                        ; skip if dist too great

        mov  bl, uint8_t PTR [esi * 4 + 3]
        and  bl, 0FCh
        add  bl, LightDropOffTable[eax]         ; get brightness based on dist
        jnc  singlenochange
        or   bl, 0FCh
    singlenochange:
        mov  uint8_t PTR [esi * 4 + 3], bl

    singlenext1:
        inc  esi
        add  edi, xdir

        cmp  edi, curxend
        je   nexty1

        ; **** Do 2for1 block (count em.. 2for1)

    twoforone:
        movd mm3, DistTable[edi]                ; Get XY dist
        movq mm0, [esp + esi * 2]               ; Get Z buffer values
        psubus mm0, LIGHTPOSZ
        movq mm1, ZERO
        pcmpgtw mm1, mm0                        ; Negate negative distances
        pxor mm0, mm1
        packusbw mm0, ZERO                      ; Saturate bytes
        punpcklbw mm0, mm3                      ; Combine with distable bytes
        movq mm1, mm0
        punpcklwd mm0, ZERO                     ; Make 2 DWORDs
        punpcklwd mm1, ZERO                     ; Make 2 DWORDs

pixel1:
        movd eax, mm0
        cmp eax, MAXDIST
        jle pixel2
        mov bl, DistTable[eax]
        adc bl, IntTable[ebx]
        cmovcc bl, LightDropOffTable[ebx]
        cmovcc cl, [esi]
        cmovcs cl, 0



    twoforonepos2:
        cmp  eax, intensity                     ; Make sure Z is in light range
        ja   do2only
        cmp  edx, intensity
        ja   do1only

        mov  dh, bh                             ; XY dist 2
        mov  bh, al                             ; XY dist 1
        mov  al, DistTable[ebx]                 ; d-z plane distance
        mov  cl, DistTable[edx]                 ; d-z plane distance 2
        add  al, IntTable[eax]
        jc   nodraw1
        add  cl, IntTable[ecx]
        jc   nodraw2                            ; skip if dist too great

        mov  bl, uint8_t PTR [esi * 4 + 3]
        mov  bh, uint8_t PTR [esi * 4 + 7]
        and  ebx, 0FCFCh
        add  bl, LightDropOffTable[eax]         ; get brightness based on dist
        jnc  nochange1
        or   bl, 0FCh
    nochange1:
        add  bh, LightDropOffTable[ecx]         
        mov  uint8_t PTR [esi * 4 + 3], bl
        jnc  nochange2
        or   bh, 0FCh
    nochange2:
        mov  uint8_t PTR [esi * 4 + 7], bh
        jmp  next1

        ; **** Do only blocks

    do2only:
        cmp  edx, intensity
        ja   next1
        mov  dh, bh                             ; XY dist 2
        mov  cl, DistTable[edx]                 ; d-z plane distance 2
        add  cl, IntTable[ecx]
        jc   next1                              ; skip if dist too great

        mov  dl, uint8_t PTR [esi * 4 + 7]
        and  dl, 0FCh
        add  dl, LightDropOffTable[ecx]         
        jnc  do2onlynochange
        or   dl, 0FCh
    do2onlynochange:
        mov  uint8_t PTR [esi * 4 + 7], dl
        jmp  next1

    do1only:
        mov  bh, al                             ; XY dist 1
        mov  al, DistTable[ebx]                 ; d-z plane distance
        add  al, IntTable[eax]
        jc   next1

        mov  bl, uint8_t PTR [esi * 4 + 3]
        and  bl, 0FCh
        add  bl, LightDropOffTable[eax]         ; get brightness based on dist
        jnc  do1onlynochange
        or   bl, 0FCh
    do1onlynochange:
        mov  uint8_t PTR [esi * 4 + 3], bl
        jmp  next1

        ; **** Do no draw blocks

    nodraw1:
        add  cl, IntTable[ecx]                  ; d-z plane distance 2
        jc   next1                              ; skip if dist too great
        mov  dl, uint8_t PTR [esi * 4 + 7]
        and  dl, 0FCh
        add  dl, LightDropOffTable[ecx]         
        jnc  nodraw1nochange
        or   dl, 0FCh
    nodraw1nochange:
        mov  uint8_t PTR [esi * 4 + 7], dl
        jmp  next1

    nodraw2:
        mov  bl, uint8_t PTR [esi * 4 + 3]
        and  bl, 0FCh
        add  bl, LightDropOffTable[eax]         ; get brightness based on dist
        jnc  nodraw2nochange
        or   bl, 0FCh
    nodraw2nochange:
        mov  uint8_t PTR [esi * 4 + 3], bl
        jmp  next1

    next1:
        add  esi,2
        add  edi, xdir2

        cmp  edi, curxend
        jne  twoforone

    nexty1:
        add  esi, nextline

        mov  ecx, edi
        add  ch, ydir
        mov  curxend, ecx
        cmp  ch, uint8_t PTR yend
        jne  yloop1

        mov  esp, saveesp

//      rdtsc
//      mov endtime, eax
    }
    #endif

//  int32_t diff = (int32_t)(endtime - begtime);
//  int32_t pixels = (abs(xend - xpos) + 1) * (abs(yend - ypos) + 1);
//  int32_t cyclesperpixel = diff / pixels;
//  diff = 0;
}

#endif
