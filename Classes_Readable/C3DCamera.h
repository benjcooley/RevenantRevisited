#pragma once
#include "classes_all.h"

struct C3DCamera {
    dword m_originx;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword m_originy;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword m_clippx;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword m_clippy;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword pm_videoorigin;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword pm_zbufferorigin;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword m_startx;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword m_endx;// offset 20 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword m_starty;// offset 24 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword m_endy;// offset 28 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword m_screenx;// offset 2c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword m_centerx;// offset 30 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword m_centery;// offset 34 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword pm_video;// offset 38 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword pm_zbuffer;// offset 3c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword pm_savecarrytable;// offset 40 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword pm_addlighttable;// offset 44 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword pm_divtable;// offset 48 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword pm_alphatable;// offset 4c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword m_alphacount;// offset 50 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    C3DCamera(dword *param_1, undefined4 *param_2, undefined4 *param_3, undefined4 *param_4);
    virtual dword virt_meth_0x5750d0();
    virtual undefined4 virt_meth_0x575100(byte *param_1);
    virtual C3DCamera * virt_meth_0x57bad0();
};
