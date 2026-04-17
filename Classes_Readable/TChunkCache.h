#pragma once
#include "classes_all.h"

struct TChunkCache {
    dword currentcycle;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword currentcycle16;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x20;// offset 20 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    undefined4 meth_0x48e630(int param_1);
    void meth_0x48e670(undefined4 param_1);
};
