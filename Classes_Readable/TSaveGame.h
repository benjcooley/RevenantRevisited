#pragma once
#include "classes_all.h"

struct TSaveGame {
    word mbr_0xe;// offset 0e - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword gametime;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword version;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword pane;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte loading;// offset 36 - 1 bytes - Unsigned Byte (db)
};
