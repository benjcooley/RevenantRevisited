#pragma once
#include "classes_all.h"

struct SControlEntry {
    mode mode;// offset 00 - 276 bytes - Component (member) class.
    dword codes;// offset 120 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword cmdflag;// offset 130 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x14c;// offset 14c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte down;// offset 15c - 1 bytes - Unsigned Byte (db)

    SControlEntry(int param_1);
    virtual SControlEntry * virt_meth_0x4f4ae0();
};
