#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x431750



// Function at 00431750

/* WARNING: Removing unreachable block (ram,0x004318ac) */
/* WARNING: Removing unreachable block (ram,0x004318b5) */
/* WARNING: Removing unreachable block (ram,0x004318b7) */

undefined4 cls_0x431750::meth_0x431750(cls_0x431750 *param_1, int *param_2)

{
  char cVar1;
  int iVar2;
  bool bVar3;
  undefined3 extraout_var;
  undefined4 uVar4;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  dword dVar5;
  undefined3 extraout_var_03;
  int iVar6;
  uint uVar7;
  cls_0x431750 *pcVar8;
  undefined4 *puVar9;
  char *pcVar10;
  uint *in_stack_0000000c;
  char local_f0 [54];
  char acStack_ba [8];
  char acStack_b2 [8];
  char acStack_aa [8];
  char acStack_a2 [58];
  char acStack_68 [8];
  char acStack_60 [96];
  
  if (param_1 == (cls_0x431750 *)0x0) {
    pcVar8 = this;
    for (iVar6 = 0x24; iVar6 != 0; iVar6 = iVar6 + -1) {
      pcVar8->mbr_0x0 = 0;
      pcVar8 = (cls_0x431750 *)&pcVar8->mbr_0x4;
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
    pcVar8 = this;
    for (iVar6 = 0x24; iVar6 != 0; iVar6 = iVar6 + -1) {
      pcVar8->mbr_0x0 = param_1->mbr_0x0;
      param_1 = (cls_0x431750 *)&param_1->mbr_0x4;
      pcVar8 = (cls_0x431750 *)&pcVar8->mbr_0x4;
    }
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_BGBITMAP_005cd65c,0);
  if (CONCAT31(extraout_var,bVar3) == 0) goto LAB_0043196a;
  FUN_00479580((byte *)in_stack_0000000c);
  iVar6 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd9ac);
  if (iVar6 == 0) {
    iVar6 = *param_2;
LAB_00431826:
    iVar6 = (**(code **)(iVar6 + 0x98))();
    if (iVar6 == 0) {
      return 0;
    }
  }
  else {
    dVar5 = (**(code **)(*param_2 + 0xa0))();
    this->mbr_0x0 = dVar5;
    if (dVar5 == 0) {
      iVar6 = *param_2;
      goto LAB_00431826;
    }
  }
  iVar6 = FUN_0047a410(in_stack_0000000c,(float **)s_<RECT__d__d__d__d>_005cd668);
  if (iVar6 == 0) {
    uVar4 = (**(code **)(*param_2 + 0x9c))();
    return uVar4;
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_FRAME_005cd688,0);
  if (CONCAT31(extraout_var_00,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x14 = 1;
    if (this == (cls_0x431750 *)0xffffffe8) {
      puVar9 = (undefined4 *)0x0;
      for (iVar6 = 9; iVar6 != 0; iVar6 = iVar6 + -1) {
        *puVar9 = 0;
        puVar9 = puVar9 + 1;
      }
    }
    iVar6 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i__i__i_<MARGINS__i__i__i__i_005ccf24);
    if ((iVar6 == 0) && (iVar6 = (**(code **)(*param_2 + 0x98))(), iVar6 == 0)) {
      return 0;
    }
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DRAWMODE_005cd690,0);
  if (CONCAT31(extraout_var_01,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar6 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd69c);
    if (iVar6 == 0) {
      uVar4 = (**(code **)(*param_2 + 0x9c))();
      return uVar4;
    }
  }
LAB_0043196a:
  bVar3 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd6a8,0);
  if (CONCAT31(extraout_var_02,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar6 = FUN_0047a410(in_stack_0000000c,(float **)s__30s_<RECT__i__i__i__i>_<COLOR___005cd6b0);
    if (iVar6 == 0) {
      uVar4 = (**(code **)(*param_2 + 0x9c))();
      return uVar4;
    }
    dVar5 = cls_0x45f7c0::meth_0x4acb30((cls_0x45f7c0 *)&UNK_0065abf8.field_0x418);
    this->mbr_0x40 = dVar5;
    if (dVar5 == 0) {
      this->mbr_0x40 = DAT_0065caec;
    }
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd710,0);
  if (CONCAT31(extraout_var_03,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar6 = FUN_0047a410(in_stack_0000000c,(float **)s_UP__78s_<RELR>__i__i_DOWN__78s_<_005cd718);
    if (iVar6 == 0) {
      uVar4 = (**(code **)(*param_2 + 0x9c))();
      return uVar4;
    }
    _strncpy(local_f0,acStack_a2 + 2,0x4f);
    uVar7 = 0xffffffff;
    acStack_a2[1] = 0;
    pcVar10 = local_f0;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar10 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar7 - 1);
    _strncpy(local_f0 + (~uVar7 - 1),&DAT_005cd75c,iVar2 + 0x4f);
    iVar6 = *param_2;
    (local_f0 + (~uVar7 - 1))[iVar2 + 0x4f] = '\0';
    dVar5 = (**(code **)(iVar6 + 0xa0))();
    this->mbr_0x64 = dVar5;
    _strncpy(&stack0xffffff08,acStack_aa + 2,0x4f);
    uVar7 = 0xffffffff;
    acStack_aa[1] = 0;
    pcVar10 = &stack0xffffff08;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar10 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar7 - 1);
    _strncpy(local_f0 + (~uVar7 - 9),&DAT_005cd760,iVar2 + 0x4f);
    iVar6 = *param_2;
    (local_f0 + (~uVar7 - 9))[iVar2 + 0x4f] = '\0';
    dVar5 = (**(code **)(iVar6 + 0xa0))(&stack0xffffff08,0xffffffff);
    this->mbr_0x68 = dVar5;
    _strncpy(&stack0xffffff00,acStack_60,0x4f);
    uVar7 = 0xffffffff;
    acStack_b2[1] = 0;
    pcVar10 = &stack0xffffff00;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar10 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar7 - 1);
    _strncpy(local_f0 + (~uVar7 - 0x11),&DAT_005cd764,iVar2 + 0x4f);
    iVar6 = *param_2;
    (local_f0 + (~uVar7 - 0x11))[iVar2 + 0x4f] = '\0';
    dVar5 = (**(code **)(iVar6 + 0xa0))(&stack0xffffff00,0xffffffff);
    this->mbr_0x78 = dVar5;
    _strncpy(&stack0xfffffef8,acStack_68,0x4f);
    uVar7 = 0xffffffff;
    acStack_ba[1] = 0;
    pcVar10 = &stack0xfffffef8;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar10 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar7 - 1);
    _strncpy(local_f0 + (~uVar7 - 0x19),&DAT_005cd768,iVar2 + 0x4f);
    iVar6 = *param_2;
    (local_f0 + (~uVar7 - 0x19))[iVar2 + 0x4f] = '\0';
    dVar5 = (**(code **)(iVar6 + 0xa0))(&stack0xfffffef8,0xffffffff);
    this->mbr_0x7c = dVar5;
    if ((this->mbr_0x8c == 0x80000000) && (this->mbr_0x64 != 0)) {
      this->mbr_0x8c = *(uint *)(this->mbr_0x64 + 0x14) & 0xfffdffff | 0x20;
    }
  }
  return 1;
}



