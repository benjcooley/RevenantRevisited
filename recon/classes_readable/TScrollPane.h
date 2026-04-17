#pragma once
#include "classes_all.h"

struct TScrollPane {
    dword scrolldata;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword scrollfont;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword line;// offset d8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword numlines;// offset e0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    undefined4 meth_0x4cefb0();
};
