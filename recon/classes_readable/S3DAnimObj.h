#pragma once
#include "classes_all.h"

struct S3DAnimObj {
    dword flags;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword objnum;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword parent;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword animtrack;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword pos;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword rot;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword scl;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword matrix;// offset 20 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword primtype;// offset 48 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword verttype;// offset 4c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword numverts;// offset 50 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword lverts;// offset 54 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword tlverts;// offset 5c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword numfaces;// offset 17c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword htextures;// offset 180 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword surfaces;// offset 184 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword hmaterial;// offset 188 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual undefined4 virt_meth_0x469370();
    virtual void virt_meth_0x4693e0(int param_1);
    virtual undefined4 virt_meth_0x4697a0(byte *param_1,char *param_2,undefined4 param_3,undefined4 param_4);
    virtual void virt_meth_0x469890();
    S3DAnimObj();
    virtual S3DAnimObj * virt_meth_0x46d2b0();
};
