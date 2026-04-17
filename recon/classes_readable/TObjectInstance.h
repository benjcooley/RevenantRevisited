#pragma once
#include "classes_all.h"

struct TObjectInstance {
    dword notifyflags;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mapindex;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword sector;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword cl;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword inf;// offset 110 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword shadow;// offset 190 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword imagery;// offset 194 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword animator;// offset 198 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword owner;// offset 19c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword script;// offset 1a4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword lightdef;// offset 1bc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword stats;// offset 1c0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword moveangle;// offset 1c4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword movedist;// offset 1cc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword movevert;// offset 1d4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1d8;// offset 1d8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1dc;// offset 1dc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1e4;// offset 1e4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1e8;// offset 1e8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1ec;// offset 1ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1f0;// offset 1f0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
};
