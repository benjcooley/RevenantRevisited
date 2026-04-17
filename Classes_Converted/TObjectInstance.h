#pragma once
#include "classes_all.h"

struct TObjectInstance {
    struct cls_0x41c7f0 cls_0x41c7f0;// offset 04 - 24 bytes - Component (member) class.
    dword mbr_0x1c;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x40;// offset 40 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x44;// offset 44 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x48;// offset 48 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x4c;// offset 4c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x50;// offset 50 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x54;// offset 54 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x58;// offset 58 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x5c;// offset 5c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x60;// offset 60 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x64;// offset 64 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x68;// offset 68 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x6c;// offset 6c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    TObjectInstance();
    void meth_0x48f0f0();
    void meth_0x48f180();
    void meth_0x48f340();
    void meth_0x48f450();
    void meth_0x48f560();
    void meth_0x48f680();
    void meth_0x48f760();
    void meth_0x48f950(undefined4 param_1, undefined4 param_2);
    void meth_0x48fcc0(undefined4 param_1);
    virtual void virt_meth_0x48fda0();
    virtual void virt_meth_0x48ff00();
    virtual void virt_meth_0x490030();
    virtual void virt_meth_0x4901e0();
    virtual void virt_meth_0x4903b0();
    virtual void virt_meth_0x490660();
    virtual void virt_meth_0x490760();
    virtual void virt_meth_0x490860();
    virtual void virt_meth_0x490960(int param_1);
    virtual void virt_meth_0x491870();
    virtual TObjectInstance * virt_meth_0x492060();
};
