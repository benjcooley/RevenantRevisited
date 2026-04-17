#pragma once
#include "classes_all.h"

struct cls_0x5b9f28 {
    struct cls_0x5b9f0c cls_0x5b9f0c;// offset 00 - 68 bytes - Component (member) class.
    dword mbr_0x44;// offset 44 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x48;// offset 48 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x4c;// offset 4c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x50;// offset 50 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    int meth_0x5726f0(HANDLE param_1, undefined param_2);
    cls_0x5b9f28(undefined4 param_1, undefined4 param_2, LPCSTR param_3, undefined4 param_4);
    undefined4 meth_0x573190();
    virtual cls_0x5b9f28 * virt_meth_0x573d30();
    dword meth_0x579ce0(int *param_1, undefined4 param_2, cls_0x588410 *param_3, cls_0x588410 *param_4, DWORD param_5);
};
