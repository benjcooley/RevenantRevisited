#pragma once
#include "classes_all.h"

struct cls_0x5b45ac {
    word mbr_0xc;// offset 0c - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword mbr_0x10;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x14;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x18;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xe0;// offset e0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x184;// offset 184 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x188;// offset 188 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x18c;// offset 18c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x190;// offset 190 - 1 bytes - Unsigned Byte (db)

    virtual void virt_meth_0x512bf0();
    virtual cls_0x5a47f0 * virt_meth_0x514fb0();
    virtual cls_0x5b45ac * virt_meth_0x515590();
};
