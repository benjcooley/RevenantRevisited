#pragma once
#include "classes_all.h"

struct FileResHdr {
    dword resmagic;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword topbm;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword comptype;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword version;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword datasize;// offset 24 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword objsize;// offset 3c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword hdrsize;// offset 40 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    undefined4 meth_0x49a270();
};
