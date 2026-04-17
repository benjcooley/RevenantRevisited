#pragma once
#include "classes_all.h"

struct TButtonPane {
    int32_t clicked;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x4;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x8;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xc;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x10;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x14;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1c;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x20;// offset 20 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x24;// offset 24 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x28;// offset 28 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x2c;// offset 2c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x30;// offset 30 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x34;// offset 34 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x38;// offset 38 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x3c;// offset 3c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x40;// offset 40 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x44;// offset 44 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x48;// offset 48 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x4c;// offset 4c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x5c;// offset 5c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x6c;// offset 6c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x7c;// offset 7c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x8c;// offset 8c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x90;// offset 90 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x94;// offset 94 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x98;// offset 98 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x9c;// offset 9c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xa0;// offset a0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xa4;// offset a4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xa8;// offset a8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    undefined4 meth_0x42bd90(TButtonPane *param_1, int *param_2);
};
