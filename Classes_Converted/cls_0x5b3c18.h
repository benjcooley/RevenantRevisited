#pragma once
#include "classes_all.h"

struct cls_0x5b3c18 {
    word mbr_0xc;// offset 0c - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword mbr_0xe0;// offset e0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x184;// offset 184 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x188;// offset 188 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x18c;// offset 18c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x190;// offset 190 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual cls_0x5b3c18 * virt_meth_0x510180();
    virtual void virt_meth_0x510400();
    virtual void virt_meth_0x510450();
    virtual cls_0x5a85ac * virt_meth_0x514950(undefined4 param_1);
    virtual cls_0x5b3c18 * virt_meth_0x515310();
};
