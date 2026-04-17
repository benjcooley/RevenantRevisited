#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5acaa8



// Function at 0040ed40

void cls_0x5acaa8::meth_0x40ed40()

{
  cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->field_0x44);
  return;
}



// Function at 004f1590

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5acaa8::virt_meth_0x4f1590()

{
  float fVar1;
  int iVar2;
  float *pfVar3;
  dword *pdVar4;
  int iVar5;
  
  iVar2 = cls_0x5a486c::meth_0x409ca0((cls_0x5a486c *)this->mbr_0x8,0);
  cls_0x5a486c::meth_0x40a0c0((cls_0x5a486c *)this->mbr_0x8,iVar2);
  meth_0x40ed40(this);
  iVar5 = *(int *)(iVar2 + 0xa0);
  if (0 < iVar5) {
    pfVar3 = (float *)(*(int *)(iVar2 + 0xa4) + 0x1c);
    pdVar4 = &this->mbr_0x100;
    do {
      fVar1 = *pfVar3 - _DAT_005a3524;
      iVar5 = iVar5 + -1;
      *pfVar3 = fVar1;
      *pdVar4 = (dword)fVar1;
      pfVar3 = pfVar3 + 8;
      pdVar4 = pdVar4 + 1;
    } while (iVar5 != 0);
  }
  return;
}



// Function at 004f15f0

void cls_0x5acaa8::virt_meth_0x4f15f0()

{
  this->virt_meth_0x40dd60();
  this->mbr_0xfc = 0;
  return;
}



// Function at 004f9210

cls_0x5acaa8 * cls_0x5acaa8::virt_meth_0x4f9210()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9240

cls_0x5a47f0 * cls_0x5acaa8::virt_meth_0x4f9240()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fc8e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x110);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5acaa8__vftable_5acaa8_005acaa8;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f92e0

cls_0x5acaa8 * cls_0x5acaa8::virt_meth_0x4f92e0()

{
  byte in_stack_00000004;
  
  ~cls_0x5acaa8(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9300

cls_0x5acaa8 * __thiscall cls_0x5acaa8::~cls_0x5acaa8(cls_0x5acaa8 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059fca8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5acaa8__vftable_5acaa8_005acaa8;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5acaa8 *)pcVar1;
}



// Function at 004f9350

TScreen * cls_0x5acaa8::virt_meth_0x4f9350(int param_1)

{
  uint uVar1;
  uint uVar2;
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fccb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x19c);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    uVar1 = *(uint *)(param_1 + 4);
    uVar2 = this_00->mbr_0x8;
    *(undefined4 *)&this_00->field_0xe4 = 0;
    this_00[1].mbr_0x38 = 0;
    this_00->mbr_0x8 = uVar2 | uVar1 | 0x48001;
    this_00[1].mbr_0x1c = 0;
    this_00[1].mbr_0xc = 0;
    this_00[1].mbr_0xe = 0;
    *(undefined *)&this_00[1].mbr_0x48 = 0;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4f9520_005acb0c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 0050eec0

void cls_0x5acaa8::virt_meth_0x50eec0()

{
  cls_0x5a486c::meth_0x409ca0((cls_0x5a486c *)this->mbr_0x8,0);
  meth_0x40ed40(this);
  return;
}



// Function at 0050f340

void cls_0x5acaa8::virt_meth_0x50f340()

{
  this->virt_meth_0x40dd60();
  cls_0x497c40::meth_0x497c40((cls_0x497c40 *)&DAT_0065def0);
  return;
}



// Function at 0050fc00

cls_0x5acaa8 * cls_0x5acaa8::virt_meth_0x50fc00()

{
  byte in_stack_00000004;
  
  cls_0x50fc20::~cls_0x50fc20((cls_0x50fc20 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00522af0

void cls_0x5acaa8::virt_meth_0x522af0()

{
  undefined4 uVar1;
  cls_0x50ba90 *pcVar2;
  dword *pdVar3;
  int iVar4;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a141b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a486c::meth_0x409ca0((cls_0x5a486c *)this->mbr_0x8,0);
  meth_0x40ed40(this);
  cls_0x5a486c::meth_0x409ca0((cls_0x5a486c *)this->mbr_0x8,1);
  meth_0x40ed40(this);
  uVar1 = cls_0x5a486c::meth_0x409ca0((cls_0x5a486c *)this->mbr_0x8,0);
  meth_0x40ed40(this);
  pdVar3 = &this->mbr_0x3a8;
  iVar4 = 0x14;
  do {
    pcVar2 = (cls_0x50ba90 *)FUN_00482fb0(0x70);
    local_4 = 0;
    if (pcVar2 == (cls_0x50ba90 *)0x0) {
      pcVar2 = (cls_0x50ba90 *)0x0;
    }
    else {
      pcVar2 = cls_0x50ba90::cls_0x50ba90(pcVar2,uVar1);
    }
    local_4 = 0xffffffff;
    *pdVar3 = (dword)pcVar2;
    cls_0x50ba90::meth_0x50c140(pcVar2,0xf);
    cls_0x50ba90::meth_0x50bd10((cls_0x50ba90 *)*pdVar3,0xbf800000);
    pdVar3 = pdVar3 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  ExceptionList = local_c;
  return;
}



// Function at 00523490

void cls_0x5acaa8::virt_meth_0x523490()

{
  undefined4 *puVar1;
  int iVar2;
  
  puVar1 = FUN_00482fb0(0x3200);
  this->mbr_0x3a4 = (dword)puVar1;
  puVar1 = puVar1 + 7;
  iVar2 = 400;
  do {
    *puVar1 = 0;
    puVar1 = puVar1 + 8;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}



// Function at 00523940

cls_0x5acaa8 * cls_0x5acaa8::virt_meth_0x523940()

{
  byte in_stack_00000004;
  
  cls_0x523960::~cls_0x523960((cls_0x523960 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



