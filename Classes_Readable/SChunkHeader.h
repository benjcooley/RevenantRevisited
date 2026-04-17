#pragma once
#include "classes_all.h"

struct SChunkHeader {
    byte type;// offset 00 - 1 bytes - Unsigned Byte (db)
    dword width;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword height;// offset 190 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword block;// offset 194 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1a4;// offset 1a4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    undefined4 meth_0x4a0d20(dword *param_1, uint param_2);
};
