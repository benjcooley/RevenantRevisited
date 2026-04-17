#pragma once
#include "classes_all.h"

struct TPane {
    byte mbr_0xb4;// offset b4 - 1 bytes - Unsigned Byte (db)
    byte mbr_0xb5;// offset b5 - 1 bytes - Unsigned Byte (db)
    byte mbr_0xb6;// offset b6 - 1 bytes - Unsigned Byte (db)
    dword mbr_0xbc;// offset bc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x492c40();
};
