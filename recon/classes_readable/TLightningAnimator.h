#pragma once
#include "classes_all.h"

struct TLightningAnimator {
    dword impact_spark;// offset 00 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword glow_scale;// offset 04 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword total_offset;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword end_p;// offset 0c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword numpoints;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte damage_flag;// offset 14 - 1 bytes - Unsigned Byte (db)
    dword maxpoints;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword showangle;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword rotdegree;// offset 20 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword morrotdegree;// offset 24 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword maxsize;// offset 28 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte firsttime;// offset 31 - 1 bytes - Unsigned Byte (db)
    dword down;// offset 34 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    void meth_0x588480();
    void meth_0x5725f0();
    void meth_0x57bbb0();
    void meth_0x57bc30();
    void meth_0x57c960();
    undefined meth_0x57c9c0(int param_1);
    undefined4 meth_0x57ca40();
    undefined4 meth_0x57d200();
    undefined4 meth_0x57d390();
    undefined4 meth_0x588150();
    int meth_0x588170();
    void meth_0x588180();
    void meth_0x5881a0();
    undefined meth_0x5881e0();
    undefined meth_0x588200();
    undefined2 meth_0x588220();
    undefined2 meth_0x588250();
    undefined4 meth_0x588280();
    undefined4 meth_0x5882c0();
    TLightningAnimator();
    TLightningAnimator(dword param_1);
    int meth_0x5884a0();
    void meth_0x588570();
    void meth_0x5885b0();
    void meth_0x588600();
    void meth_0x588660();
    void meth_0x5886d0(undefined4 *param_1);
    void meth_0x588740(char *param_1);
    int meth_0x588a50();
    undefined4 meth_0x588a90(int param_1);
    undefined4 meth_0x588c00(int *param_1);
    void meth_0x588d70();
    void meth_0x588db0();
    void meth_0x588df0();
    void meth_0x588e80();
    void meth_0x5891c0(undefined4 *param_1);
    undefined4 meth_0x589290();
    char * meth_0x5892e0();
    void meth_0x589310();
    void meth_0x589350(char *param_1);
    int meth_0x5895c0();
    void meth_0x5896c0(int *param_1);
    void meth_0x589810();
};
