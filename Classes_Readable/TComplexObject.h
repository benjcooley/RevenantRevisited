#pragma once
#include "classes_all.h"

struct TComplexObject {
    dword desired;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword root;// offset 24 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x34;// offset 34 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x529830();
};
