#pragma once
#include "classes_all.h"

struct cls_0x5b30fc {
    dword mbr_0x4;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x8;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xc;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x14;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x3c;// offset 3c - 1 bytes - Unsigned Byte (db)
    dword mbr_0x9c;// offset 9c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xac;// offset ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xec;// offset ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual cls_0x5a47f0 * virt_meth_0x50f6e0();
    virtual cls_0x5a47f0 * virt_meth_0x50f780();
    virtual cls_0x5b30fc * virt_meth_0x50f8c0();
    virtual TScreen * virt_meth_0x50f930(undefined4 param_1);
    virtual cls_0x5b30fc * virt_meth_0x50fa60();
    virtual TScreen * virt_meth_0x50fad0(undefined4 param_1);
};
