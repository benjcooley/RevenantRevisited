#pragma once
#include "classes_all.h"

struct SoftwareRenderer {
    word mbr_0x4;// offset 04 - 2 bytes - Unsigned Word (dw, 2-bytes)
    word mbr_0x6;// offset 06 - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword mbr_0x8;// offset 08 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    word mbr_0xc;// offset 0c - 2 bytes - Unsigned Word (dw, 2-bytes)
    word mbr_0xe;// offset 0e - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword mbr_0x10;// offset 10 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x14;// offset 14 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x18;// offset 18 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1c;// offset 1c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x20;// offset 20 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x24;// offset 24 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x28;// offset 28 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x2c;// offset 2c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x30;// offset 30 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x34;// offset 34 - 1 bytes - Unsigned Byte (db)
    byte mbr_0x35;// offset 35 - 1 bytes - Unsigned Byte (db)
    byte mbr_0x36;// offset 36 - 1 bytes - Unsigned Byte (db)
    byte mbr_0x37;// offset 37 - 1 bytes - Unsigned Byte (db)
    dword mbr_0x38;// offset 38 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x40;// offset 40 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x44;// offset 44 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x48;// offset 48 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x4c;// offset 4c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x50;// offset 50 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x54;// offset 54 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x58;// offset 58 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    word mbr_0x5c;// offset 5c - 2 bytes - Unsigned Word (dw, 2-bytes)
    word mbr_0x5e;// offset 5e - 2 bytes - Unsigned Word (dw, 2-bytes)
    word mbr_0x60;// offset 60 - 2 bytes - Unsigned Word (dw, 2-bytes)
    word mbr_0x62;// offset 62 - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword mbr_0x64;// offset 64 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x68;// offset 68 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x6c;// offset 6c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x70;// offset 70 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x78;// offset 78 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    word mbr_0x7c;// offset 7c - 2 bytes - Unsigned Word (dw, 2-bytes)
    word mbr_0x7e;// offset 7e - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword mbr_0x80;// offset 80 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x84;// offset 84 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x88;// offset 88 - 1 bytes - Unsigned Byte (db)
    byte mbr_0x89;// offset 89 - 1 bytes - Unsigned Byte (db)
    word mbr_0x8a;// offset 8a - 2 bytes - Unsigned Word (dw, 2-bytes)
    dword mbr_0x8c;// offset 8c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x90;// offset 90 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x94;// offset 94 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x98;// offset 98 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x9c;// offset 9c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xa0;// offset a0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xa8;// offset a8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xac;// offset ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xb0;// offset b0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xb4;// offset b4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xb8;// offset b8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xd8;// offset d8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xdc;// offset dc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xe0;// offset e0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xe4;// offset e4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xe8;// offset e8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xec;// offset ec - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xf8;// offset f8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0xfc;// offset fc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x100;// offset 100 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x108;// offset 108 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x10c;// offset 10c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x110;// offset 110 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x120;// offset 120 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x124;// offset 124 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x128;// offset 128 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x12c;// offset 12c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x130;// offset 130 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x134;// offset 134 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x138;// offset 138 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x13c;// offset 13c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x140;// offset 140 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x144;// offset 144 - 1 bytes - Unsigned Byte (db)
    dword mbr_0x148;// offset 148 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x14c;// offset 14c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x15c;// offset 15c - 1 bytes - Unsigned Byte (db)
    dword mbr_0x184;// offset 184 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x188;// offset 188 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x18c;// offset 18c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x190;// offset 190 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x194;// offset 194 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x198;// offset 198 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x19c;// offset 19c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1a0;// offset 1a0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1a4;// offset 1a4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1a8;// offset 1a8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1ac;// offset 1ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1b0;// offset 1b0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1b4;// offset 1b4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x1b8;// offset 1b8 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x238;// offset 238 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x244;// offset 244 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x328;// offset 328 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x364;// offset 364 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x3a4;// offset 3a4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x3ac;// offset 3ac - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x3b0;// offset 3b0 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x3c4;// offset 3c4 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x400;// offset 400 - 1 bytes - Unsigned Byte (db)
    byte mbr_0x401;// offset 401 - 1 bytes - Unsigned Byte (db)
    dword mbr_0x404;// offset 404 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x408;// offset 408 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x40c;// offset 40c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x444;// offset 444 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x44c;// offset 44c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    byte mbr_0x494;// offset 494 - 1 bytes - Unsigned Byte (db)
    dword mbr_0x4fc;// offset 4fc - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x500;// offset 500 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x504;// offset 504 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x508;// offset 508 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x50c;// offset 50c - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x510;// offset 510 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)
    dword mbr_0x544;// offset 544 - 4 bytes - Unsigned Double-Word (ddw, 4-bytes)

    virtual bool virt_meth_0x46e8b0();
    virtual void virt_meth_0x46e900();
    virtual int virt_meth_0x46e930();
    virtual void virt_meth_0x46e970(int param_1);
    virtual int virt_meth_0x46ea20();
    virtual undefined4 virt_meth_0x46f160();
    virtual int virt_meth_0x46f190();
    virtual void virt_meth_0x46f1e0();
    virtual undefined4 virt_meth_0x46f250();
    virtual bool virt_meth_0x46f3a0();
    virtual undefined4 virt_meth_0x46f3d0();
    virtual void virt_meth_0x46faf0();
    virtual int virt_meth_0x46fc40(int *param_1,int *param_2);
    virtual int virt_meth_0x46fd30(undefined4 param_1,undefined4 param_2);
    virtual uint virt_meth_0x46fea0();
    virtual undefined4 virt_meth_0x46fee0();
    virtual undefined4 virt_meth_0x4705f0(cls_0x5b4f30 *param_1);
    virtual void virt_meth_0x4708e0();
    virtual uint virt_meth_0x470920();
    virtual void virt_meth_0x470ca0();
    virtual void virt_meth_0x470cc0();
    virtual void virt_meth_0x470e40();
    virtual void virt_meth_0x470f00();
    void meth_0x471260();
    virtual void virt_meth_0x471900(undefined4 param_1,undefined4 param_2);
    virtual void virt_meth_0x471b50();
    virtual void virt_meth_0x471ba0(int param_1);
    virtual void virt_meth_0x472310(int *param_1,int param_2);
    virtual void virt_meth_0x472380();
    virtual void virt_meth_0x472430(int param_1);
    virtual void virt_meth_0x472980();
    virtual void virt_meth_0x472e90();
    virtual undefined4 virt_meth_0x472f80(byte *param_1,char *param_2);
    virtual undefined4 virt_meth_0x473460(byte *param_1);
    virtual uint virt_meth_0x473600();
    virtual void virt_meth_0x4736f0(byte *param_1);
    virtual void virt_meth_0x4778c0();
    virtual undefined4 virt_meth_0x477990();
    virtual undefined4 virt_meth_0x477a60();
    virtual undefined4 virt_meth_0x477aa0();
    virtual void virt_meth_0x4bf730();
    virtual void virt_meth_0x4bfcb0();
    virtual int * virt_meth_0x4bfda0();
    virtual void virt_meth_0x4bff60();
    virtual undefined4 virt_meth_0x4c0020();
    virtual void virt_meth_0x4c0880();
    virtual TScreen * virt_meth_0x4c08e0(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x4c09e0();
    virtual TScreen * virt_meth_0x4c0a10(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x4c0a90();
    virtual TScreen * virt_meth_0x4c0ac0(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x4c0b40();
    virtual TScreen * virt_meth_0x4c0b70(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x4c0bf0();
    virtual SoftwareRenderer * virt_meth_0x4c0e10();
    virtual TScreen * virt_meth_0x4c10b0(undefined4 param_1);
    virtual undefined4 virt_meth_0x4dd370();
    virtual void virt_meth_0x4dd3e0(int param_1,int param_2);
    virtual void virt_meth_0x4dd470();
    virtual SoftwareRenderer * virt_meth_0x4ddd40();
    virtual SoftwareRenderer * virt_meth_0x4de060();
    virtual SoftwareRenderer * virt_meth_0x4de120();
    virtual void virt_meth_0x4de800();
    virtual void virt_meth_0x4def30();
    void meth_0x4defe0();
    dword meth_0x4df070();
    virtual void virt_meth_0x4df0f0(int param_1);
    virtual void virt_meth_0x4e9cc0(int param_1);
    virtual TScreen * virt_meth_0x4f4a30(int param_1);
    virtual SoftwareRenderer * virt_meth_0x4f4ec0();
    virtual SoftwareRenderer * virt_meth_0x4f6110();
    virtual cls_0x5a47f0 * virt_meth_0x4f6140();
    virtual SoftwareRenderer * virt_meth_0x4f6bc0();
    virtual SoftwareRenderer * virt_meth_0x4f79b0();
    virtual SoftwareRenderer * virt_meth_0x4f7d20();
    virtual SoftwareRenderer * virt_meth_0x4fa390();
    virtual cls_0x5a47f0 * virt_meth_0x4fa440();
    dword meth_0x4fa540();
    virtual void virt_meth_0x4fb560(int param_1);
    virtual void virt_meth_0x4fd8c0();
    void meth_0x4fe800();
    virtual void virt_meth_0x4ffbf0();
    virtual SoftwareRenderer * virt_meth_0x500020();
    virtual SoftwareRenderer * virt_meth_0x5001c0();
    virtual cls_0x5a47f0 * virt_meth_0x5001f0();
    virtual SoftwareRenderer * virt_meth_0x500350();
    virtual cls_0x5a47f0 * virt_meth_0x500380();
    virtual SoftwareRenderer * virt_meth_0x5004e0();
    virtual cls_0x5a47f0 * virt_meth_0x500510();
    virtual SoftwareRenderer * virt_meth_0x500660();
    virtual cls_0x5a47f0 * virt_meth_0x500690();
    virtual SoftwareRenderer * virt_meth_0x500800();
    virtual cls_0x5a47f0 * virt_meth_0x500830();
    virtual SoftwareRenderer * virt_meth_0x500990();
    virtual cls_0x5a47f0 * virt_meth_0x5009c0();
    virtual SoftwareRenderer * virt_meth_0x500b20();
    virtual cls_0x5a47f0 * virt_meth_0x500b50();
    virtual SoftwareRenderer * virt_meth_0x500c80();
    virtual cls_0x5a85ac * virt_meth_0x500cb0(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x500d10();
    virtual cls_0x5a85ac * virt_meth_0x500d40(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x500da0();
    virtual cls_0x5a85ac * virt_meth_0x500dd0(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x500e30();
    virtual cls_0x5a85ac * virt_meth_0x500e60(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x500ec0();
    virtual cls_0x5a85ac * virt_meth_0x500ef0(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x500f50();
    virtual cls_0x5a85ac * virt_meth_0x500f80(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x500fe0();
    virtual cls_0x5a47f0 * virt_meth_0x501010();
    virtual SoftwareRenderer * virt_meth_0x501650();
    virtual cls_0x5a47f0 * virt_meth_0x501680();
    virtual void virt_meth_0x501790();
    virtual void virt_meth_0x502470();
    virtual void virt_meth_0x504920();
    virtual void virt_meth_0x505290();
    void meth_0x507330();
    virtual SoftwareRenderer * virt_meth_0x508ff0();
    virtual cls_0x5a47f0 * virt_meth_0x509020();
    virtual SoftwareRenderer * virt_meth_0x5091c0();
    virtual cls_0x5a47f0 * virt_meth_0x5091f0();
    virtual SoftwareRenderer * virt_meth_0x509550();
    virtual cls_0x5a47f0 * virt_meth_0x509580();
    virtual SoftwareRenderer * virt_meth_0x509a00();
    virtual SoftwareRenderer * virt_meth_0x509b90();
    virtual cls_0x5a47f0 * virt_meth_0x509bc0();
    virtual SoftwareRenderer * virt_meth_0x509d20();
    virtual cls_0x5a85ac * virt_meth_0x509d50(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x509df0();
    virtual SoftwareRenderer * virt_meth_0x50a1f0();
    virtual cls_0x5a47f0 * virt_meth_0x50a220();
    virtual SoftwareRenderer * virt_meth_0x50a390();
    virtual cls_0x5a47f0 * virt_meth_0x50a3c0();
    virtual void virt_meth_0x50a490();
    virtual SoftwareRenderer * virt_meth_0x50a4b0();
    virtual undefined4 virt_meth_0x50d1a0();
    virtual int virt_meth_0x50d370();
    virtual undefined4 virt_meth_0x50d3a0();
    virtual void virt_meth_0x50d510();
    virtual void virt_meth_0x50d640();
    virtual void virt_meth_0x50d9c0();
    virtual SoftwareRenderer * virt_meth_0x50df50();
    virtual TScreen * virt_meth_0x50df80(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x50e400();
    virtual SoftwareRenderer * virt_meth_0x50e4e0();
    virtual undefined4 virt_meth_0x50ea80();
    virtual TScreen * virt_meth_0x50eb80(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x50ed80();
    virtual TScreen * virt_meth_0x50edb0(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x50ee10();
    virtual cls_0x5a47f0 * virt_meth_0x50f640();
    virtual TScreen * virt_meth_0x50f820(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x50f890();
    virtual SoftwareRenderer * virt_meth_0x50fa30();
    virtual SoftwareRenderer * virt_meth_0x50fbd0();
    virtual TScreen * virt_meth_0x50fd30(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x50fdf0();
    virtual TScreen * virt_meth_0x50ffd0(undefined4 param_1);
    undefined4 meth_0x517050(char *param_1, uint param_2);
    virtual SoftwareRenderer * virt_meth_0x522010();
    virtual cls_0x5a85ac * virt_meth_0x523530(undefined4 param_1);
    virtual cls_0x5a85ac * virt_meth_0x5235e0(undefined4 param_1);
    virtual cls_0x5a47f0 * virt_meth_0x5236b0();
    virtual SoftwareRenderer * virt_meth_0x523880();
    virtual SoftwareRenderer * virt_meth_0x5238b0();
    virtual void virt_meth_0x523a70();
    virtual SoftwareRenderer * virt_meth_0x523b90();
    virtual undefined4 virt_meth_0x523da0();
    virtual TScreen * virt_meth_0x523de0(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x523fd0();
    virtual TScreen * virt_meth_0x5240f0(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x5241b0();
    virtual void virt_meth_0x524ce0();
    virtual void virt_meth_0x5267d0();
    virtual void virt_meth_0x526e20();
    void meth_0x526f00(int param_1, int param_2);
    virtual void virt_meth_0x5273d0(int param_1);
    virtual TScreen * virt_meth_0x527460(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x527550();
    virtual TScreen * virt_meth_0x527580(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x527660();
    virtual TScreen * virt_meth_0x527690(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x527750();
    virtual TScreen * virt_meth_0x527780(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x527840();
    virtual TScreen * virt_meth_0x527870(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x527920();
    virtual TScreen * virt_meth_0x527950(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x527a00();
    virtual TScreen * virt_meth_0x527a30(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x527b20();
    virtual TScreen * virt_meth_0x527b50(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x527ef0();
    virtual TScreen * virt_meth_0x527f20(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x527fd0();
    virtual TScreen * virt_meth_0x528000(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x5280a0();
    virtual TScreen * virt_meth_0x5280d0(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x528170();
    virtual TScreen * virt_meth_0x5281a0(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x528240();
    virtual TScreen * virt_meth_0x528270(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x528310();
    virtual TScreen * virt_meth_0x528340(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x5283e0();
    virtual void virt_meth_0x528430();
    virtual TScreen * virt_meth_0x528600(undefined4 param_1);
    virtual void virt_meth_0x5286e0();
    virtual SoftwareRenderer * virt_meth_0x528770();
    virtual cls_0x5a47f0 * virt_meth_0x528980();
    virtual TScreen * virt_meth_0x528a90(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x528b00();
    virtual void virt_meth_0x528ea0();
    virtual TScreen * virt_meth_0x528ee0(undefined4 param_1);
    virtual SoftwareRenderer * virt_meth_0x529120();
    virtual SoftwareRenderer * virt_meth_0x5421d0();
    virtual cls_0x5b99c0 * virt_meth_0x542200(undefined4 param_1,undefined4 param_2,undefined4 param_3,
          undefined4 param_4,undefined4 param_5,undefined4 param_6);
    virtual cls_0x5b99c0 * virt_meth_0x542290(undefined4 param_1,undefined4 param_2,undefined4 param_3,
          undefined4 param_4,undefined4 param_5,undefined4 param_6);
};
