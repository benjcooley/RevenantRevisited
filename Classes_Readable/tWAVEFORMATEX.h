#pragma once
#include "classes_all.h"

struct tWAVEFORMATEX {
    dword wFormatTag;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword nSamplesPerSec;// offset 184 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword nAvgBytesPerSec;// offset 188 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x18c;// offset 18c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x190;// offset 190 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x194;// offset 194 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x198;// offset 198 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x503540();
};
