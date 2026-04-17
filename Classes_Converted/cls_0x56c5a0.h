#pragma once
#include "classes_all.h"

struct cls_0x56c5a0 {
    word mbr_0x0;// offset 00 - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword mbr_0x800;// offset 800 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x2800;// offset 2800 - 1 bytes - Unsigned Byte (db)
    word mbr_0x2c00;// offset 2c00 - 2 bytes - Unsigned Word (dw, 2-bytes)
    word mbr_0x3400;// offset 3400 - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword mbr_0x5400;// offset 5400 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x56c5a0();
};
