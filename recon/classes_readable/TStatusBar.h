#pragma once
#include "classes_all.h"

struct TStatusBar {
    dword level;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword targetlevel;// offset 12c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte animating;// offset 138 - 1 bytes - Unsigned Byte (db)
    dword mbr_0x144;// offset 144 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x579bb0();
};
