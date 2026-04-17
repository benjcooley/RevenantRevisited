#pragma once
#include "classes_all.h"

struct cls_0x531b70 {
    byte mbr_0x17c;// offset 17c - 1 bytes - Unsigned Byte (db)
    word mbr_0x194;// offset 194 - 2 bytes - Unsigned Word (dw, 2-bytes)
    word mbr_0x196;// offset 196 - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword mbr_0x198;// offset 198 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1b4;// offset 1b4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x531b70();
};
