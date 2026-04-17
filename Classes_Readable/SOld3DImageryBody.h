#pragma once
#include "classes_all.h"

struct SOld3DImageryBody {
    dword flags;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword numverts;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword verts;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword numfaces;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword faces;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword nummaterials;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword material;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword numtextures;// offset 20 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword texturedesc;// offset 48 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword texturebits;// offset 4c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword texturepals;// offset 50 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword textureframes;// offset 54 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword numobjects;// offset 5c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword motion;// offset 17c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword anikeys;// offset 180 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x184;// offset 184 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x188;// offset 188 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x18c;// offset 18c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x190;// offset 190 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x19c;// offset 19c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1a0;// offset 1a0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual undefined4 virt_meth_0x5399f0();
    virtual void virt_meth_0x539ab0();
    SOld3DImageryBody();
    virtual SOld3DImageryBody * virt_meth_0x53a1d0();
};
