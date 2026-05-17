// Decompiled methods and structure for class: cls_0x431750

/*
/OOAnalyzer/cls_0x431750
pack(disabled)
Structure cls_0x431750 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
   88   byte   1   mbr_0x58   "Unsigned Byte (db)"
   89   byte   1   mbr_0x59   "Unsigned Byte (db)"
   90   byte   1   mbr_0x5a   "Unsigned Byte (db)"
   92   byte   1   mbr_0x5c   "Unsigned Byte (db)"
   93   byte   1   mbr_0x5d   "Unsigned Byte (db)"
   94   byte   1   mbr_0x5e   "Unsigned Byte (db)"
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   104   dword   4   mbr_0x68   "Unsigned Double-Word (ddw, 4-bytes)"
   108   dword   4   mbr_0x6c   "Unsigned Double-Word (ddw, 4-bytes)"
   112   dword   4   mbr_0x70   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   132   dword   4   mbr_0x84   "Unsigned Double-Word (ddw, 4-bytes)"
   136   dword   4   mbr_0x88   "Unsigned Double-Word (ddw, 4-bytes)"
   140   dword   4   mbr_0x8c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 144 Alignment: 1

*/

// Function at 00431750

/* WARNING: Removing unreachable block (ram,0x004318ac) */
/* WARNING: Removing unreachable block (ram,0x004318b5) */
/* WARNING: Removing unreachable block (ram,0x004318b7) */

undefined4 __thiscall
OOAnalyzer::cls_0x431750::meth_0x431750_DefWidget_EDIT_attr_parser(cls_0x431750 *this,cls_0x431750 *param_1,int *param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  dword dVar4;
  int iVar5;
  uint uVar6;
  cls_0x431750 *pcVar7;
  undefined4 *puVar8;
  char *pcVar9;
  undefined4 in_stack_0000000c;
  char local_f0 [54];
  char acStack_ba [8];
  char acStack_b2 [8];
  char acStack_aa [8];
  char acStack_a2 [58];
  char acStack_68 [8];
  char acStack_60 [16];
  undefined auStack_50 [80];
  
  if (param_1 == (cls_0x431750 *)0x0) {
    pcVar7 = this;
    for (iVar5 = 0x24; iVar5 != 0; iVar5 = iVar5 + -1) {
      pcVar7->mbr_0x0 = 0;
      pcVar7 = (cls_0x431750 *)&pcVar7->mbr_0x4;
    }
    this->mbr_0x40 = DAT_0065caec;
    this->mbr_0x3c = 0x80000000;
    this->mbr_0x58 = 0xff;
    this->mbr_0x59 = 0xff;
    this->mbr_0x5a = 0xff;
    this->mbr_0x5d = 0xff;
    this->mbr_0x5e = 0xff;
    this->mbr_0x5c = 0;
    this->mbr_0x54 = 1;
    this->mbr_0x60 = 0x80000000;
    this->mbr_0x8c = 0x80000000;
  }
  else if (this != param_1) {
    pcVar7 = this;
    for (iVar5 = 0x24; iVar5 != 0; iVar5 = iVar5 + -1) {
      pcVar7->mbr_0x0 = param_1->mbr_0x0;
      param_1 = (cls_0x431750 *)&param_1->mbr_0x4;
      pcVar7 = (cls_0x431750 *)&pcVar7->mbr_0x4;
    }
  }
  iVar5 = FUN_00479700();
  if (iVar5 == 0) goto LAB_0043196a;
  FUN_00479580();
  iVar5 = FUN_0047a410();
  if (iVar5 == 0) {
    iVar5 = *param_2;
LAB_00431826:
    iVar5 = (**(code **)(iVar5 + 0x98))();
    if (iVar5 == 0) {
      return 0;
    }
  }
  else {
    dVar4 = (**(code **)(*param_2 + 0xa0))();
    this->mbr_0x0 = dVar4;
    if (dVar4 == 0) {
      iVar5 = *param_2;
      goto LAB_00431826;
    }
  }
  iVar5 = FUN_0047a410();
  if (iVar5 == 0) {
    uVar3 = (**(code **)(*param_2 + 0x9c))();
    return uVar3;
  }
  iVar5 = FUN_00479700();
  if (iVar5 != 0) {
    FUN_00479580();
    this->mbr_0x14 = 1;
    if (this == (cls_0x431750 *)0xffffffe8) {
      puVar8 = (undefined4 *)0x0;
      for (iVar5 = 9; iVar5 != 0; iVar5 = iVar5 + -1) {
        *puVar8 = 0;
        puVar8 = puVar8 + 1;
      }
    }
    iVar5 = FUN_0047a410(in_stack_0000000c,s__i__i__i__i_<MARGINS__i__i__i__i_005ccf24,
                         &this->field_0x1c,&this->mbr_0x20,&this->mbr_0x24,&this->mbr_0x28,
                         &this->mbr_0x2c,&this->mbr_0x30,&this->mbr_0x34);
    if ((iVar5 == 0) && (iVar5 = (**(code **)(*param_2 + 0x98))(), iVar5 == 0)) {
      return 0;
    }
  }
  iVar5 = FUN_00479700();
  if (iVar5 != 0) {
    FUN_00479580();
    iVar5 = FUN_0047a410(in_stack_0000000c);
    if (iVar5 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))();
      return uVar3;
    }
  }
LAB_0043196a:
  iVar5 = FUN_00479700();
  if (iVar5 != 0) {
    FUN_00479580();
    iVar5 = FUN_0047a410(in_stack_0000000c,s__30s_<RECT__i__i__i__i>_<COLOR___005cd6b0,
                         acStack_a2 + 2,&this->mbr_0x44,&this->mbr_0x48,&this->mbr_0x4c,
                         &this->mbr_0x50,&this->mbr_0x5a,&this->mbr_0x59,&this->mbr_0x58,
                         &this->mbr_0x5e,&this->mbr_0x5d,&this->mbr_0x5c);
    if (iVar5 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))();
      return uVar3;
    }
    dVar4 = cls_0x45f7c0::meth_0x4acb30((cls_0x45f7c0 *)&UNK_0065abf8.field_0x418);
    this->mbr_0x40 = dVar4;
    if (dVar4 == 0) {
      this->mbr_0x40 = DAT_0065caec;
    }
  }
  iVar5 = FUN_00479700();
  if (iVar5 != 0) {
    FUN_00479580();
    iVar5 = FUN_0047a410(in_stack_0000000c,s_UP__78s_<RELR>__i__i_DOWN__78s_<_005cd718,
                         acStack_a2 + 2,&this->mbr_0x74,&this->mbr_0x6c,&this->mbr_0x70,auStack_50,
                         &this->mbr_0x88,&this->mbr_0x80);
    if (iVar5 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))();
      return uVar3;
    }
    _strncpy(local_f0,acStack_a2 + 2,0x4f);
    uVar6 = 0xffffffff;
    acStack_a2[1] = 0;
    pcVar9 = local_f0;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar6 - 1);
    _strncpy(local_f0 + (~uVar6 - 1),&DAT_005cd75c,iVar2 + 0x4f);
    iVar5 = *param_2;
    (local_f0 + (~uVar6 - 1))[iVar2 + 0x4f] = '\0';
    dVar4 = (**(code **)(iVar5 + 0xa0))();
    this->mbr_0x64 = dVar4;
    _strncpy(&stack0xffffff08,acStack_aa + 2,0x4f);
    uVar6 = 0xffffffff;
    acStack_aa[1] = 0;
    pcVar9 = &stack0xffffff08;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar6 - 1);
    _strncpy(local_f0 + (~uVar6 - 9),&DAT_005cd760,iVar2 + 0x4f);
    iVar5 = *param_2;
    (local_f0 + (~uVar6 - 9))[iVar2 + 0x4f] = '\0';
    dVar4 = (**(code **)(iVar5 + 0xa0))(&stack0xffffff08,0xffffffff);
    this->mbr_0x68 = dVar4;
    _strncpy(&stack0xffffff00,acStack_60,0x4f);
    uVar6 = 0xffffffff;
    acStack_b2[1] = 0;
    pcVar9 = &stack0xffffff00;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar6 - 1);
    _strncpy(local_f0 + (~uVar6 - 0x11),&DAT_005cd764,iVar2 + 0x4f);
    iVar5 = *param_2;
    (local_f0 + (~uVar6 - 0x11))[iVar2 + 0x4f] = '\0';
    dVar4 = (**(code **)(iVar5 + 0xa0))(&stack0xffffff00,0xffffffff);
    this->mbr_0x78 = dVar4;
    _strncpy(&stack0xfffffef8,acStack_68,0x4f);
    uVar6 = 0xffffffff;
    acStack_ba[1] = 0;
    pcVar9 = &stack0xfffffef8;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar6 - 1);
    _strncpy(local_f0 + (~uVar6 - 0x19),&DAT_005cd768,iVar2 + 0x4f);
    iVar5 = *param_2;
    (local_f0 + (~uVar6 - 0x19))[iVar2 + 0x4f] = '\0';
    dVar4 = (**(code **)(iVar5 + 0xa0))(&stack0xfffffef8,0xffffffff);
    this->mbr_0x7c = dVar4;
    if ((this->mbr_0x8c == 0x80000000) && (this->mbr_0x64 != 0)) {
      this->mbr_0x8c = *(uint *)(this->mbr_0x64 + 0x14) & 0xfffdffff | 0x20;
    }
  }
  return 1;
}



