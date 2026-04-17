#pragma once
#include "classes_all.h"

struct TConsolePane {
    dword head;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword chained;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword tail;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword oldbuflen;// offset 30 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword box;// offset 38 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword axis;// offset 44 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword cmdthreadhandle;// offset 48 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    uint meth_0x4a22f0(uint param_1, undefined2 param_2);
    uint meth_0x4a2510(int param_1, int param_2, undefined4 *param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, uint param_8, undefined4 param_9, undefined2 param_10, undefined4 param_11, undefined4 param_12);
};
