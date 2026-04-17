#pragma once
#include "classes_all.h"

struct TStatPane {
    dword startline;// offset 40 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte expanded;// offset 44 - 1 bytes - Unsigned Byte (db)
    dword mbr_0x48;// offset 48 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    undefined4 meth_0x41b0b0(undefined4 *param_1);
};
