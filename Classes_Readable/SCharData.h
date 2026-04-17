#pragma once
#include "classes_all.h"

struct SCharData {
    dword name;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword groups;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword objtype;// offset 24 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword flags;// offset 28 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword damagemods;// offset 2c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword maxattackrange;// offset 30 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword swipecolor;// offset 34 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword weapontype;// offset 38 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword weapondamage;// offset 3c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword armorvalue;// offset 40 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword defensemod;// offset 44 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword attackmod;// offset 48 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword arrowpos;// offset 4c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword arrowspeed;// offset 50 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword bowwait;// offset 5c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x60;// offset 60 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x4d7030();
    SCharData(char *param_1);
};
