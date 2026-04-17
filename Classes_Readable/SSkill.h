#pragma once
#include "classes_all.h"

struct SSkill {
    byte children;// offset 54 - 1 bytes - Unsigned Byte (db)
    dword ancestor;// offset a0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword difficulty;// offset a4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    undefined4 meth_0x5877a0();
    void meth_0x587880();
};
