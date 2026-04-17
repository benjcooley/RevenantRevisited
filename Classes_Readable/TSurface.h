#pragma once
#include "classes_all.h"

struct TSurface {
    dword width;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword height;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword bitsperpixel;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword stride;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword originx;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte needs_restore;// offset 37 - 1 bytes - Unsigned Byte (db)
    dword originy;// offset 38 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x57;// offset 57 - 1 bytes - Unsigned Byte (db)
    dword clipmode;// offset 58 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword clipx;// offset 5c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword clipy;// offset 60 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword clipwidth;// offset 64 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword clipheight;// offset 68 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword keycolor;// offset 6c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    word mbr_0x70;// offset 70 - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword locked;// offset 74 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword cpu_buffer;// offset 78 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword buffer_size;// offset 80 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword image;// offset 84 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword img_desc;// offset 88 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x8c;// offset 8c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    TSurface(dword param_1, dword param_2, char *param_3, char *param_4, dword param_5, char *param_6, dword param_7, dword param_8, dword param_9, dword param_10, undefined2 param_11, undefined4 param_12, dword param_13, dword param_14, dword param_15, dword param_16, dword param_17);
    TSurface(int *param_1, uint param_2);
    virtual cls_0x5a3b24 * virt_meth_0x437e00(undefined4 param_1);
};
