#pragma once
#include "classes_all.h"

struct TArea {
    dword level;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword flags;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword rects;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword lastdaylight;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte scriptfile;// offset 3c - 1 bytes - Unsigned Byte (db)
    dword cdplaynum;// offset 9c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword cdplaylistsize;// offset ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword cdplaylist;// offset ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword cdplaypause;// offset fc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword cdplaystart;// offset 100 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword cdplaywait;// offset 104 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword cdplaylength;// offset 108 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x10c;// offset 10c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual void virt_meth_0x4f3970();
    virtual TArea * virt_meth_0x4f9df0();
    virtual cls_0x5a47f0 * virt_meth_0x4f9e20();
    virtual TArea * virt_meth_0x4f9ec0();
};
