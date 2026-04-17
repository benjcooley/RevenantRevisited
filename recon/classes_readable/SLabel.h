#pragma once
#include "classes_all.h"

struct SLabel {
    byte used;// offset b4 - 1 bytes - Unsigned Byte (db)
    byte isgroup;// offset b5 - 1 bytes - Unsigned Byte (db)
    byte label;// offset b6 - 1 bytes - Unsigned Byte (db)
    dword index;// offset bc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x492c40();
};
