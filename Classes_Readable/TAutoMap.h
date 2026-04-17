#pragma once
#include "classes_all.h"

struct TAutoMap {
    dword MapArea;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword ScrollToTarget;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword SavePlayerPixel;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword DraggingMap;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword CurrentMap;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword MapGrfx;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword DisplayMap;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    TAutoMap();
    virtual TAutoMap * virt_meth_0x573dd0();
};
