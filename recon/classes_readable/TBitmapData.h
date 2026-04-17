#pragma once
#include "classes_all.h"

struct TBitmapData {
    dword height;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword regx;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword regy;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword flags;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte palette;// offset 3c - 1 bytes - Unsigned Byte (db)
    dword drawmode;// offset 9c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword keycolor;// offset ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword aliassize;// offset ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword alias;// offset fc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword alphasize;// offset 100 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword alpha;// offset 104 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword zbuffersize;// offset 110 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword zbuffer;// offset 3cc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword normalsize;// offset 3d0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword normal;// offset 5ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword palettesize;// offset 5b0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword datasize;// offset 69c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword data24;// offset 6a0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword data32;// offset 6a4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x6a8;// offset 6a8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual TBitmapData * virt_meth_0x4f7190();
};
