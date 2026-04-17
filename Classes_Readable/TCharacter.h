#pragma once
#include "classes_all.h"

struct TCharacter {
    dword movetopos;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword movepos;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword chardata;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword waittype;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte autocombat;// offset 3c - 1 bytes - Unsigned Byte (db)
    dword waitticks;// offset 9c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword charflags;// offset ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword exittimestamp;// offset d8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword shovedir;// offset ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword nextattack;// offset fc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword glimpse;// offset 100 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword noise;// offset 104 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword lasthealthrecov;// offset 184 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword lastfatiguerecov;// offset 188 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword lastmanarecov;// offset 1b0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword lastpoisondamage;// offset 1b4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1b8;// offset 1b8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x264;// offset 264 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x268;// offset 268 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x26c;// offset 26c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x270;// offset 270 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x274;// offset 274 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x278;// offset 278 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x27c;// offset 27c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x280;// offset 280 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x284;// offset 284 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x288;// offset 288 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x28c;// offset 28c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x290;// offset 290 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x294;// offset 294 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x298;// offset 298 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x29c;// offset 29c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x2a0;// offset 2a0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x2a4;// offset 2a4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x2a8;// offset 2a8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x2b0;// offset 2b0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual void virt_meth_0x4df3a0();
    virtual cls_0x5a47f0 * virt_meth_0x4f4bc0();
    virtual TCharacter * virt_meth_0x4f70f0();
    virtual TCharacter * virt_meth_0x4f7120();
};
