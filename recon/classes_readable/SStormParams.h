#pragma once
#include "classes_all.h"

struct SStormParams {
    dword particles;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword tex_u;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword tex_v;// offset 50 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword particle_u;// offset 60 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword particle_v;// offset 64 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword particle_begin;// offset 68 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword particle_end;// offset 6c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword impact_u;// offset 70 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword impact_v;// offset 74 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword impact_begin;// offset 78 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword impact_end;// offset 7c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword gravity;// offset 80 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword velocity;// offset 84 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword pos;// offset 88 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword pos_spread;// offset 8c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword impact_frame_inc;// offset 90 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword particle_frame_inc;// offset 94 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword particle_scale;// offset 98 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword impact_scale;// offset 9c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual undefined4 virt_meth_0x43da00();
    virtual void virt_meth_0x43dad0();
    virtual void virt_meth_0x43db00();
    virtual void virt_meth_0x43db30(cls_0x5a68d8 *param_1);
    virtual void virt_meth_0x43dfa0(undefined4 param_1);
    virtual void virt_meth_0x43e2b0(int param_1);
};
