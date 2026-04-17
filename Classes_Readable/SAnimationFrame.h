#pragma once
#include "classes_all.h"

struct SAnimationFrame {
    dword dx;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword dy;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword regx;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword regy;// offset 38 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword x;// offset d8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword y;// offset dc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword width;// offset e0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword height;// offset e4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword decbufsize;// offset e8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword bitmap;// offset ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
};
