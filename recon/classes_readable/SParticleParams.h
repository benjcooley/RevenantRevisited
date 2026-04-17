#pragma once
#include "classes_all.h"

struct SParticleParams {
    dword particles;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword gravity;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword trails;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword objflags;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte bounce;// offset 34 - 1 bytes - Unsigned Byte (db)
    byte killobj;// offset 35 - 1 bytes - Unsigned Byte (db)
    dword numtargets;// offset 38 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword targetpos;// offset 3c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword turnang;// offset 40 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword seekspeed;// offset 54 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword autorange;// offset ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword hitrange;// offset ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual undefined4 virt_meth_0x40f5b0();
    virtual SParticleParams * virt_meth_0x410df0();
    virtual byte virt_meth_0x418830();
    virtual void virt_meth_0x418890();
    virtual void virt_meth_0x418c60();
    virtual undefined4 virt_meth_0x418fc0();
    virtual cls_0x5a47f0 * virt_meth_0x419040();
    virtual undefined4 virt_meth_0x419590();
    virtual void virt_meth_0x419d80();
    virtual undefined4 virt_meth_0x419fb0();
    virtual SParticleParams * virt_meth_0x419ff0();
    virtual SParticleParams * virt_meth_0x41a020();
    virtual void virt_meth_0x445b20();
};
