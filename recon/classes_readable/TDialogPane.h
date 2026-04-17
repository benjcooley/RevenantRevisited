#pragma once
#include "classes_all.h"

struct TDialogPane {
    dword dialogdata;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword numchoices;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword choice;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword freshresponse;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte character;// offset 30 - 1 bytes - Unsigned Byte (db)
    dword grabslot;// offset 34 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x570900();
    undefined4 meth_0x570c00();
    void meth_0x5722e0();
};
