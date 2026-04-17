#pragma once
#include "classes_all.h"

struct cls_0x5ac0cc {
    dword mbr_0x4;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x8;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xc;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x14;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x3c;// offset 3c - 1 bytes - Unsigned Byte (db)
    dword mbr_0x9c;// offset 9c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xac;// offset ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xec;// offset ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xfc;// offset fc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x100;// offset 100 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x104;// offset 104 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x108;// offset 108 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual void virt_meth_0x4f0130();
    virtual void virt_meth_0x4f0160();
    virtual cls_0x5ac0cc * virt_meth_0x4f8980();
    virtual cls_0x5a47f0 * virt_meth_0x4f89b0();
    virtual cls_0x5ac0cc * virt_meth_0x4f8a50();
    virtual TScreen * virt_meth_0x4f8ac0(int param_1);
};
