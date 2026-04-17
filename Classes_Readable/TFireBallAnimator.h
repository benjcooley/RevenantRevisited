#pragma once
#include "classes_all.h"

struct TFireBallAnimator {
    dword trail;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword next_state;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword frame_count;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword glow_frame;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword spark;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword ring;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword fireball_angle;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword explode;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword burst;// offset 20 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword firsttime;// offset 24 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword old_state;// offset 28 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    undefined4 meth_0x41cdd0(int *param_1);
    undefined4 meth_0x41d000();
    undefined4 meth_0x41d1e0();
};
