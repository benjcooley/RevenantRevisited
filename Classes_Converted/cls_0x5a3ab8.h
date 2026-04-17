#pragma once
#include "classes_all.h"

struct cls_0x5a3ab8 {
    dword mbr_0x8;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xc;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x10;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x14;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x18;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x37;// offset 37 - 1 bytes - Unsigned Byte (db)
    dword mbr_0x38;// offset 38 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x57;// offset 57 - 1 bytes - Unsigned Byte (db)
    dword mbr_0x58;// offset 58 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x5c;// offset 5c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x60;// offset 60 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x64;// offset 64 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x68;// offset 68 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x6c;// offset 6c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    word mbr_0x70;// offset 70 - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword mbr_0x74;// offset 74 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x78;// offset 78 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x80;// offset 80 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x84;// offset 84 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x88;// offset 88 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x8c;// offset 8c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    cls_0x5a3ab8(dword param_1, dword param_2, char *param_3, char *param_4, dword param_5, char *param_6, dword param_7, dword param_8, dword param_9, dword param_10, undefined2 param_11, undefined4 param_12, dword param_13, dword param_14, dword param_15, dword param_16, dword param_17);
    cls_0x5a3ab8(int *param_1, uint param_2);
    virtual cls_0x5a3b24 * virt_meth_0x437e00(undefined4 param_1);
};
