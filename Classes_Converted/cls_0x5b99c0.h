#pragma once
#include "classes_all.h"

struct cls_0x5b99c0 {
    dword mbr_0x4;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x8;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xc;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x10;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x110;// offset 110 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x118;// offset 118 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x11c;// offset 11c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x120;// offset 120 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x124;// offset 124 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x128;// offset 128 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x12c;// offset 12c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x130;// offset 130 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    struct cls_0x41c7f0 cls_0x41c7f0;// offset 13c - 24 bytes - Component (member) class.

    cls_0x5b99c0(dword param_1, dword *param_2, dword param_3, dword *param_4, dword param_5, dword param_6);
    void meth_0x53f560();
    virtual void virt_meth_0x53f800();
    virtual void virt_meth_0x53f860();
    virtual cls_0x5b99c0 * virt_meth_0x540b00();
};
