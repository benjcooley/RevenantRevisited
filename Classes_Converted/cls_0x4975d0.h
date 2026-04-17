#pragma once
#include "classes_all.h"

struct cls_0x4975d0 {
    dword mbr_0x0;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x4004;// offset 4004 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x8004;// offset 8004 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x8014;// offset 8014 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    undefined4 meth_0x4975d0();
    void meth_0x497700(uint param_1, undefined4 param_2);
    int meth_0x497800(uint param_1);
    void meth_0x497910(uint param_1, char *param_2);
    int meth_0x497a30(uint param_1);
    int meth_0x497b40(uint param_1);
};
