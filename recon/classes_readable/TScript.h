#pragma once
#include "classes_all.h"

struct TScript {
    dword proto;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword newtrigger;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword trigger;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword priority;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte newtriggerstr;// offset 3c - 1 bytes - Unsigned Byte (db)
    dword lastpriority;// offset 9c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword block;// offset ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword depth;// offset ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword curtrigger;// offset fc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x100;// offset 100 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x104;// offset 104 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual void virt_meth_0x4e4d50();
    virtual undefined4 virt_meth_0x4e4d90();
    virtual cls_0x5a47f0 * virt_meth_0x4f5d70();
    virtual TScript * virt_meth_0x4f76c0();
};
