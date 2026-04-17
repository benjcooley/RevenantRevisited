#pragma once
#include "classes_all.h"

struct cls_0x4609f0 {
    dword mbr_0x0;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x4;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x14;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1c;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x20;// offset 20 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x123;// offset 123 - 1 bytes - Unsigned Byte (db)

    undefined4 meth_0x4609f0();
    dword meth_0x460c60();
    uint * meth_0x460ca0(int param_1);
    undefined4 meth_0x460d00(byte *param_1);
    int meth_0x460d60(byte *param_1);
};
