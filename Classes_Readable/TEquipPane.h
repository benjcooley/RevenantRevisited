#pragma once
#include "classes_all.h"

struct TEquipPane {
    dword equipdata;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword top;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword bottom;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword section;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword grabslot;// offset 13c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword heldslot;// offset 14c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x53f730(int param_1);
};
