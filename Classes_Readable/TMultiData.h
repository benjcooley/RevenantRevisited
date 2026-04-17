#pragma once
#include "classes_all.h"

struct TMultiData {
    dword names;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword offsets;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x34;// offset 34 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
};
