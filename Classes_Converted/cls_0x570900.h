#pragma once
#include "classes_all.h"

struct cls_0x570900 {
    dword mbr_0x0;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xc;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x10;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x14;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x30;// offset 30 - 1 bytes - Unsigned Byte (db)
    dword mbr_0x34;// offset 34 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x570900();
    undefined4 meth_0x570c00();
    void meth_0x5722e0();
};
