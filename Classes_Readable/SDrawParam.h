#pragma once
#include "classes_all.h"

struct SDrawParam {
    dword drawmode;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword func;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword callback;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword originx;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte swidth;// offset 3c - 1 bytes - Unsigned Byte (db)
    dword originy;// offset 9c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword clipx;// offset ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword clipy;// offset ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword clipwidth;// offset fc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword clipheight;// offset 100 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword dx;// offset 104 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword dy;// offset 108 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword dwidth;// offset 10c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword dheight;// offset 184 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword sx;// offset 188 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword sy;// offset 3d4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword sheight;// offset 3d8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword color;// offset 3dc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword intensity;// offset 6a4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x6a8;// offset 6a8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x6ac;// offset 6ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x80c;// offset 80c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual void virt_meth_0x4ec890();
    virtual void virt_meth_0x4ec9d0(int param_1);
    virtual SDrawParam * virt_meth_0x4f7e80();
    virtual cls_0x5a47f0 * virt_meth_0x4f7eb0();
    virtual SDrawParam * virt_meth_0x4f7f50();
    virtual TScreen * virt_meth_0x4f7fc0(int param_1);
};
