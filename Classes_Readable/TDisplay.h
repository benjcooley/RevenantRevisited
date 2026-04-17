#pragma once
#include "classes_all.h"

struct TDisplay {
    dword default_pass;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword depth_pass;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword tile_pipeline;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword tile_pip_desc;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword color_target;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword depth_target;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword normal_target;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword model;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword light_params;// offset 20 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x429ac0(cls_0x5a68d8 *param_1, int *param_2, int param_3, int param_4, int param_5, int param_6);
};
