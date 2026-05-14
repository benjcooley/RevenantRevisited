#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b79ac



// Function at 00525fc0

void cls_0x5b79ac::virt_meth_0x525fc0(int param_1)

{
  dword dVar1;
  dword in_stack_00000008;
  
  this->virt_meth_0x471ba0_Notify(param_1);
  if (param_1 == 1) {
    if (this->mbr_0x100 != in_stack_00000008) {
      return;
    }
  }
  else {
    if (param_1 != 2) {
      return;
    }
    dVar1 = this->mbr_0x100;
    if (dVar1 == 0) {
      return;
    }
    if (*(dword *)(dVar1 + 0x44) != in_stack_00000008) {
      return;
    }
    if ((*(uint *)(dVar1 + 8) & 0x80000) != 0) {
      return;
    }
  }
  this->mbr_0x100 = 0;
  return;
}



// Function at 00526230

void cls_0x5b79ac::virt_meth_0x526230()

{
  int *piVar1;
  int iVar2;
  
  iVar2 = *(int *)(this->mbr_0x4 + 0xd8);
  if ((((iVar2 != 0) && (piVar1 = *(int **)(*(int *)(iVar2 + 4) + 0xe0), piVar1 != (int *)0x0)) &&
      ((*piVar1 == 3 || ((piVar1 != (int *)0x0 && (*piVar1 == 0x19)))))) &&
     ((int *)piVar1[0x11] != (int *)0x0)) {
    (**(code **)(*(int *)piVar1[0x11] + 0x1b4))(1);
  }
  this->virt_meth_0x40dd60();
  iVar2 = 0;
  do {
    cls_0x5a7e38::meth_0x526040((cls_0x5a7e38 *)this);
    iVar2 = iVar2 + 1;
  } while (iVar2 < 100);
  if (DAT_005d79e4 < 3) {
    SoftwareRenderer::meth_0x4defe0((SoftwareRenderer *)this->mbr_0x4);
  }
  this->mbr_0xfc = 0;
  return;
}



// Function at 00527c30

cls_0x5b79ac * cls_0x5b79ac::virt_meth_0x527c30()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00527c60

cls_0x5a47f0 * cls_0x5b79ac::virt_meth_0x527c60()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a15ce;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x1ec0);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b79ac__vftable_5b79ac_005b79ac;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00527d00

TScreen * cls_0x5b79ac::virt_meth_0x527d00(undefined4 param_1)

{
  TScreen *this_00;
  uint uVar1;
  dword dVar2;
  uint extraout_ECX;
  char *pcVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a15f3;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xfc);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527de0_005b7a10;
    uVar1 = FUN_0059a530_stricmp(extraout_ECX,(byte *)this_00->mbr_0x38,(byte *)s_CavFGeyser_005e3044);
    if (uVar1 == 0) {
      pcVar3 = s_fgeyser_005e3050;
    }
    else {
      pcVar3 = s_sgeyser_005e3058;
    }
    dVar2 = cls_0x4746d0::meth_0x475210((cls_0x4746d0 *)&UNK_0066cc1c.field_0x4c,pcVar3);
    this_00->mbr_0xf8 = dVar2;
    this_00->mbr_0xe0 = 0;
    this_00->mbr_0xd8 = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00527de0

cls_0x5b79ac * cls_0x5b79ac::virt_meth_0x527de0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00527e10

TScreen * cls_0x5b79ac::virt_meth_0x527e10(undefined4 param_1)

{
  TScreen *this_00;
  uint uVar1;
  dword dVar2;
  uint extraout_ECX;
  char *pcVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1613;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xfc);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527de0_005b7a10;
    uVar1 = FUN_0059a530_stricmp(extraout_ECX,(byte *)this_00->mbr_0x38,(byte *)s_CavFGeyser_005e3044);
    if (uVar1 == 0) {
      pcVar3 = s_fgeyser_005e3050;
    }
    else {
      pcVar3 = s_sgeyser_005e3058;
    }
    dVar2 = cls_0x4746d0::meth_0x475210((cls_0x4746d0 *)&UNK_0066cc1c.field_0x4c,pcVar3);
    this_00->mbr_0xf8 = dVar2;
    this_00->mbr_0xe0 = 0;
    this_00->mbr_0xd8 = 0;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527ef0_005b7c50;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00528790

cls_0x5b79ac * cls_0x5b79ac::virt_meth_0x528790()

{
  byte in_stack_00000004;
  
  ~cls_0x5b79ac(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005287b0

cls_0x5b79ac * __thiscall cls_0x5b79ac::~cls_0x5b79ac(cls_0x5b79ac *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1708;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b79ac__vftable_5b79ac_005b79ac;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b79ac *)pcVar1;
}



