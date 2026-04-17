#pragma once
#include "classes_all.h"

struct SImageryEntry {
    dword status;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword header;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword headersize;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword headerdirty;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte filename;// offset 3c - 1 bytes - Unsigned Byte (db)
    dword body;// offset 9c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword ressize;// offset ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword usecount;// offset ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword imagery;// offset fc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x100;// offset 100 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual SImageryEntry * virt_meth_0x4f9710();
    virtual cls_0x5a47f0 * virt_meth_0x4f9740();
    virtual SImageryEntry * virt_meth_0x4f97e0();
    virtual cls_0x5a47f0 * virt_meth_0x4f9850();
};
