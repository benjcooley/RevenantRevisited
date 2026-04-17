#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a4b2c



// Function at 0046a660

undefined4 cls_0x5a4b2c::virt_meth_0x46a660()

{
  int iVar1;
  dword dVar2;
  
  iVar1 = this->virt_meth_0x435150(s_joingame_005d3ed0,0x659cc4,0,0,0,0x280,0x1e0,0x1c2,0xa0,
                     s_widgets_005d3ec8);
  if (iVar1 == 0) {
    return 0;
  }
  dVar2 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x17c = dVar2;
  dVar2 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x180 = dVar2;
  dVar2 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x184 = dVar2;
  dVar2 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x188 = dVar2;
  dVar2 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x18c = dVar2;
  return 1;
}



// Function at 0046d470

cls_0x5a4b2c * cls_0x5a4b2c::virt_meth_0x46d470()

{
  byte in_stack_00000004;
  
  ~cls_0x5a4b2c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0046d490

cls_0x5a4b2c * __thiscall cls_0x5a4b2c::~cls_0x5a4b2c(cls_0x5a4b2c *this)

{
  cls_0x5a4b2c *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d1fe;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a4b2c__vftable_5a4b2c *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a4b2c *)FUN_004830f0((LPCVOID)this->mbr_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



