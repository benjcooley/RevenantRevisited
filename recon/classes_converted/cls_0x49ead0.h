#pragma once
#include "classes_all.h"

struct cls_0x49ead0 {
    dword mbr_0x0;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x4;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x8;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xc;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x110;// offset 110 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x18f;// offset 18f - 1 bytes - Unsigned Byte (db)
    dword mbr_0x190;// offset 190 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    struct cls_0x41c7f0 cls_0x41c7f0;// offset 194 - 24 bytes - Component (member) class.
    struct cls_0x41c7f0 cls_0x41c7f0;// offset 1bc - 24 bytes - Component (member) class.
    struct cls_0x41c7f0 cls_0x41c7f0;// offset 1d4 - 24 bytes - Component (member) class.
    dword mbr_0x1ec;// offset 1ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1f0;// offset 1f0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    cls_0x49ead0(undefined4 param_1);
    void meth_0x49ec40();
    undefined4 meth_0x49ee20(cls_0x49ead0 *param_1);
    void meth_0x49eff0();
    void meth_0x49f180();
    int meth_0x49f1b0();
    undefined4 meth_0x49f3d0(undefined4 *param_1);
    undefined4 * meth_0x49f470(byte *param_1);
    undefined4 meth_0x49fb80(int param_1, char *param_2);
    undefined4 meth_0x49ff40();
    int meth_0x4a0180(cls_0x49ead0 *param_1);
    int meth_0x4a0380(byte *param_1);
    dword * meth_0x4a06a0();
    dword * meth_0x4a0890();
    undefined4 meth_0x4a0e70();
};
