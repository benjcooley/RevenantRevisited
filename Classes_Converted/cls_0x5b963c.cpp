#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b963c



// Function at 005399f0

undefined4 cls_0x5b963c::virt_meth_0x5399f0()

{
  int iVar1;
  int *piVar2;
  dword extraout_ECX;
  cls_0x419dd0 cVar3;
  undefined2 uVar4;
  int iVar5;
  
  cls_0x45f7c0::meth_0x48d260((cls_0x45f7c0 *)&DAT_0065a618);
  iVar5 = 0;
  iVar1 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
  piVar2 = FUN_004a1ec0(0xd8,0xa0,(-(uint)(iVar1 != 0xf) & 2) + 2,iVar5);
  this->mbr_0x19c = (dword)piVar2;
  uVar4 = 0;
  cVar3.mbr_0x0 = extraout_ECX;
  cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xfffffff0);
  cls_0x4a22f0::meth_0x4a22f0((cls_0x4a22f0 *)this->mbr_0x19c,cVar3.mbr_0x0,uVar4);
  this->mbr_0x1a0 = 0;
  DAT_0066fb04 = 0xffffffff;
  this->virt_meth_0x435150(s_savegame_005e4330,s_default_005e4328,0x11,0,0,0x280,0x1e0,0x1c2,
             0xa0,s_widgets_005e4320);
  return 1;
}



// Function at 00539ab0

void cls_0x5b963c::virt_meth_0x539ab0()

{
  if ((LPCVOID)this->mbr_0x19c != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x19c);
  }
  this->mbr_0x19c = 0;
  this->virt_meth_0x434f30();
  return;
}



// Function at 0053a0e0

cls_0x5b963c::cls_0x5b963c()

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->vftptr_0x0 = (cls_0x5b963c__vftable_5b963c *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5b963c__vftable_5b963c_005b963c;
  return this;
}



// Function at 0053a140

cls_0x5b963c * __thiscall cls_0x5b963c::~cls_0x5b963c(cls_0x5b963c *this)

{
  cls_0x5b963c *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1b96;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b963c__vftable_5b963c_005b963c;
  local_4 = 0;
  if ((LPCVOID)this->mbr_0x19c != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x19c);
  }
  this->mbr_0x19c = 0;
  this->virt_meth_0x434f30();
  this->vftptr_0x0 = (cls_0x5b963c__vftable_5b963c *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5b963c *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 0053a1d0

cls_0x5b963c * cls_0x5b963c::virt_meth_0x53a1d0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b963c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



