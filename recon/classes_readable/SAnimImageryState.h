#pragma once
#include "classes_all.h"

struct SAnimImageryState {
    dword still;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword anim;// offset 3c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword invitem;// offset 48 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword invanim;// offset 80 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword flags;// offset 8c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
};
