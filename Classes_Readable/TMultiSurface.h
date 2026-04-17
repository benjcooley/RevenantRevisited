#pragma once
#include "classes_all.h"

struct TMultiSurface {
    dword zbuffer;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    struct lost lost;// offset 04 - 24 bytes - Component (member) class.
    dword normals;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte deletesurf;// offset 20 - 1 bytes - Unsigned Byte (db)

    TMultiSurface();
};
