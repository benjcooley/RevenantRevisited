#pragma once
#include "classes_all.h"

struct TWeaponSwipe {
    dword obj;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword weaponmat;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword initialized;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword weaponverts;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword numverts;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword charanim;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword maxsegs;// offset 20 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x49e610();
};
