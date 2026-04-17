#pragma once
#include "classes_all.h"

struct TExecuteBuf {
    dword exBuf;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword debDesc;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x24;// offset 24 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    undefined4 meth_0x49d2a0(undefined4 param_1, undefined4 param_2, undefined param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined1 param_7, undefined4 param_8, undefined param_9, undefined1 param_10, undefined param_11, undefined param_12, undefined param_13);
    void meth_0x49d650();
};
