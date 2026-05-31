// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                resource.cpp - Resource object module                  *
// *************************************************************************

#include "resource.h"

#include "bitmap.h"
#include "graphics.h"
#include "resourcehdr.h"
#include "revutils.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

static inline void strupr(char *s)
{
    for (; *s; ++s) *s = (char)toupper((unsigned char)*s);
}

// True when the last path component has a '.' — used to decide whether to
// append ".DAT". Plain strchr on the full path is wrong because a leading
// "./"/".\" in ResourcePath contains a dot.
static inline bool has_basename_ext(const char *path)
{
    const char *base = path;
    for (const char *q = path; *q; ++q)
        if (*q == '/' || *q == '\\')
            base = q + 1;
    return strchr(base, '.') != nullptr;
}

static char erropen[]    = "Couldn't open resource file %s!";
static char errnotcgs[]  = "%s is not a CGS resource file!";
static char errreading[] = "Error reading resource file %s!";
static char errwriting[] = "Error writing resource file %s!";
static char erroldver[]  = "%s has an older resource file version!";
static char errnewver[]  = "%s has a newer resource file version!";
static char erralloc[]   = "Couldn't alloc memory for %s!";

//CBox ImageryBox("c:\\exile\\nonormal\\imagery.box", BOX_READ_WRITE);

void *LoadResource(const char *name, int32_t id, uint32_t *ressize)
{
    char *ptr;
    char filename[81];
    char fname[81];
    FileResHdr frh;
    uint32_t *bitmaptable;

    if (id >= 0 && id <= 999)
    {
        sprintf(fname, "%s.%03d", name, id);
        strcpy(filename, ResourcePath);
        strcat(filename, fname);
    }

    else
    {
        strcpy(filename, ResourcePath);
        strcat(filename, name);
        if (!has_basename_ext(filename))
            strcat(filename, ".DAT");
    }

    strupr(filename);

    FILE *fl = rev_fopen(filename, "rb");

    // Missing files are recoverable — let the caller decide. This matches the
    // LoadResourceHeader policy (Status + return null). Keep FatalError for
    // format errors where the file exists but is corrupt.
    if (!fl)
    {
        Status(erropen, filename);
        return nullptr;
    }

  // Load header and check validity
    if (fread(&frh, sizeof(FileResHdr), 1, fl) < 1)
        FatalError(errreading, filename);

    if (frh.resmagic != RESMAGIC)
        FatalError(errnotcgs, filename);

    if (frh.version < RESVERSION)
        FatalError(erroldver, filename);

    if (frh.version > RESVERSION)
        FatalError(errnewver, filename);

  // Skip over header
    if (frh.hdrsize > 0)
        fseek(fl, frh.hdrsize + sizeof(FileResHdr), 0);

  // Read bitmap table
    //int32_t here = ftell(fl);

    if (frh.topbm)
    {
        bitmaptable = (uint32_t *)malloc(sizeof(uint32_t) * frh.topbm);
        if (fread(bitmaptable, sizeof(uint32_t), frh.topbm, fl) < frh.topbm)
            FatalError(errreading, filename);
    }

  // Read resource
    ptr = (char *)malloc(frh.objsize);

    if (!ptr)
        FatalError(erralloc, name);

    int32_t retval = fread(ptr, frh.datasize, 1, fl);
    fclose(fl);

    if (retval < 1)
        FatalError(errreading, filename);

    // Touch resource to force vm system to keep pages loaded
    char *m = ptr;
    for (int32_t c = 0; c < (int32_t)frh.datasize; c += 2048, m += 2048)
    {
        int32_t dummy = *m;
    }

    // Do runtime conversion to 16 bit, if necessary.
    if (frh.topbm)
    {
        for (int32_t i = 0; i < frh.topbm; i++)
        {
            PTBitmap bm = (PTBitmap) (ptr + bitmaptable[i]);

            // Sanity check
            if ((uint32_t)bm->width > (uint32_t)8192 || (uint32_t)bm->height > (uint32_t)8192)
                FatalError("Corrupted bitmap list in resource %s", name);

            // Legacy 555 -> Revenant-565 conversion (Convert15to16 / ConvertPal15to16)
            // is intentionally disabled in this port. The retail engine blitted
            // into a 565 backbuffer so it had to convert at load time; our
            // renderer decodes 555 directly to RGBA8 (see bitmapdecode.cpp).
            // Convert15to16's output isn't standard 565 either — it packs as
            // R<<11 | G<<6 | B (5-bit green with a gap at bit 5, see
            // graphics.cpp:9237-9240) which would corrupt every TMulti bitmap
            // (chrome, icons, etc.) when our decoder sees it. We already
            // disabled the same call on baked .i3d icons in 3dimage.cpp for
            // the same reason. Leaving the calls out keeps all loaded bitmaps
            // in their on-disk format (BM_15BIT 555 / BM_16BIT true-565 / 8-bit
            // palette), and the renderer's per-flag branch handles each.
            (void)bm;
        }
        free(bitmaptable);
    }

    if (ressize)
        *ressize = frh.objsize;

    return ptr;
}

void *LoadResourceHeader(const char *name, int32_t id, uint32_t *ressize)
{
    static char filename[81];
    static char fname[81];

    FileResHdr frh;

    if (id >= 0 && id <= 999)
    {
        sprintf(fname, "%s.%03d", name, id);
        strcpy(filename, ResourcePath);
        strcat(filename, fname);
    }

    else
    {
        strcpy(filename, ResourcePath);
        strcat(filename, name);
        if (!has_basename_ext(filename))
            strcat(filename, ".DAT");
    }

    strupr(filename);

    FILE *fl = rev_fopen(filename, "rb");

    if (!fl)
    {
        Status(erropen, filename);
        return nullptr;
    }

    if (fread(&frh, sizeof(FileResHdr), 1, fl) < 1)
    {
        fclose(fl);
        Status(errreading, filename);
        return nullptr;
    }

    if (frh.resmagic != RESMAGIC)
    {
        fclose(fl);
        Status(errnotcgs, filename);
        return nullptr;
    }

    if (frh.version < RESVERSION)
    {
        fclose(fl);
        Status(erroldver, filename);
        return nullptr;
    }

    if (frh.version > RESVERSION)
    {
        fclose(fl);
        Status(errnewver, filename);
        return nullptr;
    }

    char *ptr = nullptr;

    if (frh.hdrsize > 0)
    {
        ptr = (char *)malloc(frh.hdrsize);

        if (!ptr)
        {
            fclose(fl);
            Status(erralloc, name);
            return nullptr;
        }

        int32_t retval = fread(ptr, frh.hdrsize, 1, fl);
        fclose(fl);

        if (retval < 1)
        {
            Status(errreading, filename);
            return nullptr;
        }
    }

    if (ressize)
        *ressize = frh.hdrsize;

    return ptr;
}

// This doesn't work right now!
/*bool SaveResource(const char *name, void *ptr, int32_t id)
{
    static char filename[81];
    static char fname[81];
    FileResHdr frh;

    if (id >= 0)
    {
        wsprintf(fname, "%s.%03d", name, id);
        strcpy(filename, ResourcePath);
        strcat(filename, fname);
    }

    else
    {
        strcpy(filename, ResourcePath);
        strcat(filename, name);
        if (!has_basename_ext(filename))
            strcat(filename, ".dat");
    }

    FILE *fl = fopen(filename, "wb");

    if (!fl)
        FatalError(erropen, filename);

    frh.resmagic = RESMAGIC;
    frh.topbm = 0;
    frh.comptype = COMP_ZIP;
    frh.version = RESVERSION;
    frh.objsize = GlobalSize((HGLOBAL)LOWORD(GlobalHandle
                                ((const void *)HIWORD((uint32_t)ptr))));
    frh.datasize = 0;
    frh.hdrsize = 0;

    if (fwrite(&frh, sizeof(FileResHdr), 1, fl) < 1)
        FatalError(errwriting, filename);

    if (fwrite(ptr, frh.objsize, 1, fl) < 1)
        FatalError(errwriting, filename);

    frh.datasize = ftell(fl) - sizeof(FileResHdr);
    fseek(fl, 0, 0);
    if (fwrite(&frh, sizeof(FileResHdr), 1, fl) < 1)
        FatalError(errwriting, filename);

    fclose(fl);

    return FALSETRUE;
}
*/

bool SaveResourceHeader(const char *name, void *header, int32_t hdrsize, int32_t id)
{
    static char filename[81];
    static char fname[81];
    char errreading[] = "Error reading resource file %s!";

    FileResHdr frh;

    if (hdrsize == 0 || !header)
        return true;

    if (id >= 0 && id <= 999)
    {
        sprintf(fname, "%s.%03d", name, id);
        strcpy(filename, ResourcePath);
        strcat(filename, fname);
    }

    else
    {
        strcpy(filename, ResourcePath);
        strcat(filename, name);
        if (!has_basename_ext(filename))
            strcat(filename, ".DAT");
    }

    strupr(filename);

    FILE *fl = rev_fopen(filename, "rb+");

    if (!fl)
        FatalError(erropen, filename);

    if (fread(&frh, sizeof(FileResHdr), 1, fl) < 1)
        FatalError(errreading, filename);

    if (frh.resmagic != RESMAGIC)
        FatalError(errnotcgs, filename);

    if (frh.version < RESVERSION)
        FatalError(erroldver, filename);

    if (frh.version > RESVERSION)
        FatalError(errnewver, filename);

  // Load in old header
    char *oldheader = (char *)malloc(frh.hdrsize);

    if (!oldheader)
        FatalError(erralloc, name);

    if (fread(oldheader, frh.hdrsize, 1, fl) < 1)
        FatalError(errreading, filename);

  // Check to see if header is different
    if ((int32_t)frh.hdrsize == hdrsize && memcmp(oldheader, header, hdrsize) == 0)
    {
        free(oldheader);
        fclose(fl);
        return true;
    }

    free(oldheader);

  // Read bitmap table
    uint32_t *bitmaptable;
    if (frh.topbm)
    {
        bitmaptable = (uint32_t *)malloc(sizeof(uint32_t) * frh.topbm);
        if (fread(bitmaptable, sizeof(uint32_t), frh.topbm, fl) < frh.topbm)
            FatalError(errreading, filename);
    }
    else
        bitmaptable = nullptr;

  // Read resource
    char *data = (char *)malloc(frh.datasize);
    if (!data)
        FatalError(erralloc, name);

    if (fread(data, 1, frh.datasize, fl) < 1)
        FatalError(errreading, filename);

  // Close and reopen file
    rewind(fl);

  // Write out NEW header block
    frh.hdrsize = hdrsize;

  // Resave resource header
    if (fwrite(&frh, sizeof(FileResHdr), 1, fl) < 1)
        FatalError(errwriting, filename);

    if (fwrite(header, frh.hdrsize, 1, fl) < 1)
        FatalError(errwriting, filename);

  // Write out old bitmap table
    if (frh.topbm > 0)
    {
        if (fwrite(bitmaptable, sizeof(uint32_t), frh.topbm, fl) < frh.topbm)
            FatalError(errwriting, filename);
    }

  // Write out old resource data
    if (fwrite(data, frh.datasize, 1, fl) < 1)
        FatalError(errwriting, filename);

  // Delete allocated memory
    if (data)
        free(data);

    if (bitmaptable)
        free(bitmaptable);

    fclose(fl);

    return true;
}
