#pragma once
#include "classes_all.h"

struct TSpell {
    dword effect;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword targetnum;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword targets;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword timer;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword frame;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword master;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword spell;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword variant;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword wait;// offset 20 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword source;// offset 24 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword magic_defense;// offset 28 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword magic_offense;// offset 2c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    undefined4 meth_0x49e490(int param_1);
};
