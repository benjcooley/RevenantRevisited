#pragma once
#include "classes_all.h"

struct cls_0x4741b0 {
    word mbr_0x0;// offset 00 - 2 bytes - Unsigned Word (dw, 2-bytes)
    word mbr_0x2;// offset 02 - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword mbr_0x4;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    Alignment * meth_0x4741b0(uint param_1);
    uint meth_0x474210();
    int meth_0x4780c0();
    void meth_0x478170(undefined4 *param_1);
    void meth_0x478520();
};
