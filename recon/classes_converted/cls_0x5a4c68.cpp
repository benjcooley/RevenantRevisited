#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a4c68



// Function at 00462660

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 cls_0x5a4c68::virt_meth_0x462660()

{
  int iVar1;
  int *piVar2;
  cls_0x46d6b0 *this_00;
  uint *puVar3;
  
  cls_0x5756d0::meth_0x578210((cls_0x5756d0 *)&DAT_00676738);
  this->virt_meth_0x434e40();
  this->mbr_0x17c = 0;
  cls_0x45f7c0::meth_0x48d260((cls_0x45f7c0 *)&DAT_0065a618);
  puVar3 = (uint *)0x14;
  piVar2 = FUN_004a1ec0(0x14,0x14,*(uint *)(PTR_DAT_005d79e0 + 0x38) & 0x3001f,0);
  iVar1 = _DAT_006668d0;
  this->mbr_0x1b4 = (dword)piVar2;
  piVar2[6] = iVar1;
  this_00 = (cls_0x46d6b0 *)
            FUN_0047f670((cls_0x49ead0 *)s_connect_dat_005d2b64,(void *)0xffffffff,0,puVar3);
  if (this_00 == (cls_0x46d6b0 *)0x0) {
    return 0;
  }
  cls_0x5b98b8::meth_0x435c60((cls_0x5b98b8 *)this,this_00);
  cls_0x46d6b0::meth_0x46d710(this_00);
  cls_0x5b98b8::meth_0x435660((cls_0x5b98b8 *)this);
  cls_0x5b98b8::meth_0x435ad0((cls_0x5b98b8 *)this,DAT_0065bb10);
  cls_0x5b98b8::meth_0x4377c0_DefWidget_LoadFile((cls_0x5b98b8 *)this);
  cls_0x5b98b8::meth_0x437620_DefWidget_ParseBuffer((cls_0x5b98b8 *)this);
  return 1;
}



// Function at 00462720

void cls_0x5a4c68::virt_meth_0x462720()

{
  if ((LPCVOID)this->mbr_0x1b4 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x1b4);
    this->mbr_0x1b4 = 0;
  }
  this->virt_meth_0x434f30();
  return;
}



// Function at 00463dc0

void cls_0x5a4c68::virt_meth_0x463dc0()

{
  int iVar1;
  
  this->virt_meth_0x435d70();
  iVar1 = cls_0x5756d0::meth_0x576880((cls_0x5756d0 *)&DAT_00676738);
  if (iVar1 != 0) {
    cls_0x5756d0::meth_0x5769a0((cls_0x5756d0 *)&DAT_00676738);
    cls_0x5a3d44::meth_0x430c50((cls_0x5a3d44 *)this->mbr_0x190);
    (**(code **)(*(int *)this->mbr_0x190 + 0x1c))(((int *)this->mbr_0x190)[5] | 0x20);
  }
  return;
}



// Function at 0046d020

cls_0x5a4c68::cls_0x5a4c68()

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->vftptr_0x0 = (cls_0x5a4c68__vftable_5a4c68 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a4c68__vftable_5a4c68_005a4c68;
  this->mbr_0x180 = 0xffffffff;
  return this;
}



// Function at 0046d080

cls_0x5a4c68 * cls_0x5a4c68::virt_meth_0x46d080()

{
  byte in_stack_00000004;
  
  ~cls_0x5a4c68(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0046d0a0

cls_0x5a4c68 * __thiscall cls_0x5a4c68::~cls_0x5a4c68(cls_0x5a4c68 *this)

{
  cls_0x5a4c68 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d17e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a4c68__vftable_5a4c68 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a4c68 *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



