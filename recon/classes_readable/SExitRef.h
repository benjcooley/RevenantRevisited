#pragma once
#include "classes_all.h"

struct SExitRef {
    dword target;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword level;// offset 28 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mapindex;// offset 2c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword ambient;// offset 3c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword ambcolor;// offset 40 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword next;// offset 50 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    int meth_0x48cb50(undefined4 param_1);
};
