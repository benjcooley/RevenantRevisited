#pragma once
#include "classes_all.h"

struct cls_0x5b5560 {
    dword mbr_0x8;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x4c;// offset 4c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xd8;// offset d8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual cls_0x5b5560 * virt_meth_0x520b80();
    virtual undefined4 virt_meth_0x520eb0(int param_1);
    virtual cls_0x5b5560 * virt_meth_0x521cd0();
    virtual TScreen * virt_meth_0x521cf0(undefined4 param_1);
};
