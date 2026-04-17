#pragma once
#include "classes_all.h"

struct TMapPane {
    dword sectors;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword newlevel;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword center;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword centeron;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword onobject;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword oldz;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte clicked;// offset 3c - 1 bytes - Unsigned Byte (db)
    dword notifyflags;// offset 9c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword lastkey;// offset ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword numbgrects;// offset d8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword bgrects;// offset ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword queuedrect;// offset 104 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword scrollrect;// offset 108 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword unlitmulti;// offset 10c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword litmulti;// offset 118 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword updatemulti;// offset 13c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword dlight;// offset 140 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x144;// offset 144 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x158;// offset 158 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x15c;// offset 15c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x184;// offset 184 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x188;// offset 188 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x18c;// offset 18c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x190;// offset 190 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x194;// offset 194 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual void virt_meth_0x4e7160();
    virtual void virt_meth_0x4e7210();
    virtual TMapPane * virt_meth_0x4f62e0();
    virtual cls_0x5a47f0 * virt_meth_0x4f6310();
    virtual TMapPane * virt_meth_0x4f63b0();
};
