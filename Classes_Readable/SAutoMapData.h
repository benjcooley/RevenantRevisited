#pragma once
#include "classes_all.h"

struct SAutoMapData {
    dword ResID;// offset 98 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword Level;// offset ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword Left;// offset c8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword Top;// offset fc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword Right;// offset 10c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword Bottom;// offset 110 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x499500();
};
