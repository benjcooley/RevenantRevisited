#pragma once
#include "classes_all.h"

struct SLightDef {
    word flags;// offset 0c - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword pos;// offset e0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword color;// offset 184 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword lightindex;// offset 188 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword lightid;// offset 18c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x190;// offset 190 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual SLightDef * virt_meth_0x510180();
    virtual void virt_meth_0x510400();
    virtual void virt_meth_0x510450();
    virtual SControlEntry * virt_meth_0x514950(undefined4 param_1);
    virtual SLightDef * virt_meth_0x515310();
};
