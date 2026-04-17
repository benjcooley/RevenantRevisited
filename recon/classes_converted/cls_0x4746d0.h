#pragma once
#include "classes_all.h"

struct cls_0x4746d0 {
    dword mbr_0x4;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x8;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    word mbr_0x14;// offset 14 - 2 bytes - Unsigned Word (dw, 2-bytes)
    word mbr_0x1c;// offset 1c - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword mbr_0x24;// offset 24 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x34;// offset 34 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x38;// offset 38 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    int meth_0x4746d0(byte *param_1, byte *param_2, undefined param_3, undefined param_4, undefined4 param_5);
    int meth_0x474bb0(int param_1, uint param_2);
    uint meth_0x475210(byte *param_1);
};
