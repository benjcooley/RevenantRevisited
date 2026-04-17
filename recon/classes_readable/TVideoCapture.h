#pragma once
#include "classes_all.h"

struct TVideoCapture {
    dword saving;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword totframes;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword frame;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword bufframe;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword bufsize;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword framesize;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword frameaccum;// offset 40 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x49a380();
};
