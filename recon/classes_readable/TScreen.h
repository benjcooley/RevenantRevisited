#pragma once
#include "classes_all.h"

struct TScreen {
    dword exclusive;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword complete;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword curexclusive;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword numexclusive;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword nextscreen;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword firstframe;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    undefined4 meth_0x460620();
    undefined4 meth_0x460dc0(char *param_1);
};
