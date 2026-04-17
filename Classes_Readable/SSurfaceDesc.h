#pragma once
#include "classes_all.h"

struct SSurfaceDesc {
    dword mbr_0x4;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x8;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xc;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x10;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x14;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x18;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1c;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x20;// offset 20 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x48;// offset 48 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x4c;// offset 4c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x50;// offset 50 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x54;// offset 54 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x5c;// offset 5c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x17c;// offset 17c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x180;// offset 180 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x184;// offset 184 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x188;// offset 188 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual undefined4 virt_meth_0x469370();
    virtual void virt_meth_0x4693e0(int param_1);
    virtual undefined4 virt_meth_0x4697a0(byte *param_1,char *param_2,undefined4 param_3,undefined4 param_4);
    virtual void virt_meth_0x469890();
    SSurfaceDesc();
    virtual SSurfaceDesc * virt_meth_0x46d2b0();
};
