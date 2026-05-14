#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a5d18_TLogoScreen



// Function at 00488e70

cls_0x5a5d18_TLogoScreen::cls_0x5a5d18_TLogoScreen()

{
  TObjectInstance::TObjectInstance(&this->TObjectInstance);
  this->mbr_0x70 = (dword)&cls_0x5a4c30__vftable_5a4c30_005a4c30;
  this->mbr_0x78 = 0;
  this->mbr_0x7c = 0;
  this->mbr_0x80 = 0;
  this->mbr_0x74 = 0;
  (this->TObjectInstance).vftptr_0x0 =
       (TObjectInstance__vftable_5a5ed4 *)&cls_0x5a5d18__vftable_5a5d18_005a5d18;
  return this;
}



// Function at 00488ea0

cls_0x5a5d18_TLogoScreen * cls_0x5a5d18_TLogoScreen::virt_meth_0x488ea0_TLogoScreen_Dtor()

{
  SIZE_T SVar1;
  byte in_stack_00000004;
  
  TObjectInstance::~TObjectInstance(&this->TObjectInstance);
  if ((in_stack_00000004 & 1) != 0) {
    if (this != (cls_0x5a5d18_TLogoScreen *)0x0) {
      SVar1 = FUN_0058cff3(this);
      DAT_0065ba08 = DAT_0065ba08 - SVar1;
    }
    FUN_0058cfab(this);
  }
  return this;
}



// Function at 0053a2c0

undefined4 cls_0x5a5d18_TLogoScreen::virt_meth_0x53a2c0_TLogoScreen_Initialize()

{
  uint *puVar1;
  dword dVar2;
  dword dVar3;
  cls_0x5a4494__vftable_5a4494 *pcVar4;
  cls_0x5b98b8 *pcVar5;
  undefined *puVar6;
  int iVar7;
  cls_0x5a4494 *this_00;
  cls_0x5a3c68 *pcVar8;
  void *unaff_EBX;
  uint *unaff_EDI;
  cls_0x419dd0 local_14;
  undefined *local_10;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_005a1be2;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  (this->TObjectInstance).mbr_0x54 = 0;
  cls_0x45f7c0::meth_0x51eda0_TPlayerManager_Clear((cls_0x45f7c0 *)&DAT_0065a890);
  FUN_00446cb0();
  DAT_0066fcac = FUN_0047f670((cls_0x49ead0 *)s_menus_dat_005e43dc,(void *)0xffffffff,0,unaff_EDI);
  iVar7 = cls_0x46d6b0::meth_0x46d710(DAT_0065abc0);
  FUN_0043a020(iVar7);
  dVar2 = this->mbr_0x70;
  cls_0x419dd0::cls_0x419dd0(&local_14);
  local_10 = &stack0xffffffd4;
  cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffffd4);
  (**(code **)(dVar2 + 0x30))(8);
  (this->TObjectInstance).mbr_0x40 = (dword)&this->mbr_0x70;
  (this->TObjectInstance).mbr_0x44 = (dword)&this->mbr_0x70;
  this_00 = (cls_0x5a4494 *)FUN_00482fb0(0x17c);
  puVar6 = PTR_DAT_005d79e0;
  local_10 = (undefined *)0x0;
  if (this_00 == (cls_0x5a4494 *)0x0) {
    this_00 = (cls_0x5a4494 *)0x0;
  }
  else {
    this_00->vftptr_0x0 = &cls_0x5a4494__vftable_5a4494_005a4494;
    dVar2 = *(dword *)(puVar6 + 8);
    dVar3 = *(dword *)(puVar6 + 4);
    this_00->mbr_0xc = dVar3;
    this_00->mbr_0x1c = dVar3;
    this_00->mbr_0x4 = 0;
    this_00->mbr_0x14 = 0;
    this_00->mbr_0x8 = 0;
    this_00->mbr_0x18 = 0;
    this_00->mbr_0x10 = dVar2;
    this_00->mbr_0x20 = dVar2;
    this_00->mbr_0x48 = 0;
    this_00->mbr_0x4c = 0;
    this_00->mbr_0x50 = 0;
    this_00->mbr_0x54 = 0;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00->field_0x88,0x10);
    this_00->vftptr_0x0 = (cls_0x5a4494__vftable_5a4494 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  }
  pcVar4 = this_00->vftptr_0x0;
  local_10 = (undefined *)0xffffffff;
  this->mbr_0x88 = (dword)this_00;
  (*pcVar4->virt_meth_0x491900_0)(this_00);
  puVar1 = (uint *)(this->mbr_0x88 + 0x60);
  *puVar1 = *puVar1 | 2;
  pcVar8 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_10 = (undefined *)0x1;
  if (pcVar8 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar8,DAT_0066fcac,s_MenuNewGame_005e43f0,0,0,0,0,0xffffffff,0x10,0xffffffff);
  }
  local_10 = (undefined *)0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this->mbr_0x88);
  pcVar8 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_10 = (undefined *)0x2;
  if (pcVar8 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar8,DAT_0066fcac,s_MenuLoadGame_005e43fc,0,0,0,0,0xffffffff,0x10,0xffffffff);
  }
  local_10 = (undefined *)0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this->mbr_0x88);
  pcVar8 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_10 = (undefined *)0x3;
  if (pcVar8 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar8,DAT_0066fcac,s_MenuMulti_005e440c,0,0,0,0,0xffffffff,0x10,0xffffffff);
  }
  local_10 = (undefined *)0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this->mbr_0x88);
  pcVar8 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_10 = (undefined *)0x4;
  if (pcVar8 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar8,DAT_0066fcac,s_MenuOptions_005e4418,0,0,0,0,0xffffffff,0x10,0xffffffff);
  }
  local_10 = (undefined *)0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this->mbr_0x88);
  pcVar8 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_10 = (undefined *)0x5;
  if (pcVar8 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar8,DAT_0066fcac,s_MenuExit_005e4424,0,0,0,0,0xffffffff,0x10,0xffffffff);
  }
  local_10 = (undefined *)0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this->mbr_0x88);
  iVar7 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this->mbr_0x88);
  pcVar5 = (cls_0x5b98b8 *)this->mbr_0x88;
  *(undefined **)(iVar7 + 0x80) = &LAB_0053a1f0;
  *(uint *)(iVar7 + 0x14) = *(uint *)(iVar7 + 0x14) | 0x40;
  iVar7 = cls_0x5b98b8::meth_0x436900(pcVar5);
  *(undefined **)(iVar7 + 0x80) = &LAB_0053a220;
  *(uint *)(iVar7 + 0x14) = *(uint *)(iVar7 + 0x14) | 0x40;
  iVar7 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this->mbr_0x88);
  pcVar5 = (cls_0x5b98b8 *)this->mbr_0x88;
  *(undefined **)(iVar7 + 0x80) = &LAB_0053a240;
  *(uint *)(iVar7 + 0x14) = *(uint *)(iVar7 + 0x14) | 0x40;
  iVar7 = cls_0x5b98b8::meth_0x436900(pcVar5);
  *(undefined **)(iVar7 + 0x80) = &LAB_0053a260;
  *(uint *)(iVar7 + 0x14) = *(uint *)(iVar7 + 0x14) | 0x40;
  iVar7 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this->mbr_0x88);
  *(uint *)(iVar7 + 0x14) = *(uint *)(iVar7 + 0x14) | 0x40;
  *(undefined **)(iVar7 + 0x80) = &LAB_0053a2a0;
  cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
  ExceptionList = unaff_EBX;
  return 1;
}



// Function at 0053a680

void cls_0x5a5d18_TLogoScreen::virt_meth_0x53a680_TLogoScreen_Close()

{
  cls_0x5a4f28::meth_0x48ee10((cls_0x5a4f28 *)this);
  (**(code **)(*(int *)this->mbr_0x88 + 4))();
  if ((int *)this->mbr_0x88 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x88 + 0x7c))(1);
  }
  FUN_00482f80(DAT_0066fcac);
  DAT_0066fcac = (LPCVOID)0x0;
  FUN_0043a020(0);
  return;
}



