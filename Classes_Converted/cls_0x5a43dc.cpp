#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a43dc



// Function at 0043fd10

undefined4 cls_0x5a43dc::virt_meth_0x43fd10()

{
  dword dVar1;
  dword dVar2;
  cls_0x5a3c68 *pcVar3;
  undefined4 *puVar4;
  cls_0x5a3c68 *pcVar5;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ce0a;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->virt_meth_0x434e40();
  dVar1 = this->mbr_0x10;
  this->mbr_0x580 = 0xffffffff;
  this->mbr_0x584 = 0xffffffff;
  this->mbr_0x57c = 0;
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 0;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68(pcVar3,&DAT_005cf020,0,0,0x20,dVar1,0xc0,0,0,0,0,1,0,0xffffffff,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  dVar1 = this->mbr_0x10;
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 1;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005cf028,0x20,0,0x10,dVar1,0x58,0,0,0,0,1,0,0xffffffff,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  dVar1 = this->mbr_0x10;
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 2;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005cf02c,0x30,0,0x10,dVar1,0x59,0,0,0,0,1,0,0xffffffff,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  dVar1 = this->mbr_0x10;
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 3;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005cf030,0x40,0,0x10,dVar1,0x48,0,0,0,0,1,0,0xffffffff,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  dVar1 = this->mbr_0x10;
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 4;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005cf034,0x50,0,0x20,dVar1,9,&LAB_0043faa0,0,0,0,0,0,0xffffffff,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  dVar1 = this->mbr_0x10;
  dVar2 = this->mbr_0xc;
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 5;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005cf038,dVar2 - 200,0,0x14,dVar1,0,&LAB_0043fb80,0,0,0,0,0,0xffffffff,0,
               0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  dVar1 = this->mbr_0x10;
  dVar2 = this->mbr_0xc;
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 6;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005cf03c,dVar2 - 0xb4,0,0x14,dVar1,0,&LAB_0043fbb0,0,0,0,0,0,0xffffffff,0
               ,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  dVar1 = this->mbr_0x10;
  dVar2 = this->mbr_0xc;
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 7;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005cf040,dVar2 - 0xa0,0,0x20,dVar1,0x43,&LAB_0043fbf0,0,0,0,0,0,
               0xffffffff,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  dVar1 = this->mbr_0x10;
  dVar2 = this->mbr_0xc;
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 8;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005cf044,dVar2 - 0x80,0,0x20,dVar1,0x5a,&LAB_0043fc50,0,0,0,0,0,
               0xffffffff,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  dVar1 = this->mbr_0x10;
  dVar2 = this->mbr_0xc;
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 9;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005cf04c,dVar2 - 0x60,0,0x20,dVar1,0x44,&LAB_0043fbe0,0,0,0,0,0,
               0xffffffff,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  dVar1 = this->mbr_0x10;
  dVar2 = this->mbr_0xc;
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 10;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005cf050,dVar2 - 0x40,0,0x20,dVar1,0x57,&LAB_0043fca0,0,0,0,1,0,
               0xffffffff,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  dVar1 = this->mbr_0x10;
  dVar2 = this->mbr_0xc;
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 0xb;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005cf058,dVar2 - 0x20,0,0x20,dVar1,0x4c,&LAB_0043fcf0,0,0,0,0,0,
               0xffffffff,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  if (*(int *)&this->field_0x88 == 0) {
    pcVar5 = (cls_0x5a3c68 *)0x0;
  }
  else {
    pcVar5 = **(cls_0x5a3c68 ***)&this->field_0x98;
  }
  cls_0x5a3c68::meth_0x438a50(pcVar5);
  if (*(uint *)&this->field_0x88 < 2) {
    pcVar5 = (cls_0x5a3c68 *)0x0;
  }
  else {
    pcVar5 = *(cls_0x5a3c68 **)(*(int *)&this->field_0x98 + 4);
  }
  cls_0x5a3c68::meth_0x438a50(pcVar5);
  if (*(uint *)&this->field_0x88 < 3) {
    pcVar5 = (cls_0x5a3c68 *)0x0;
  }
  else {
    pcVar5 = *(cls_0x5a3c68 **)(*(int *)&this->field_0x98 + 8);
  }
  cls_0x5a3c68::meth_0x438a50(pcVar5);
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  this->mbr_0x588 = 0;
  puVar4 = FUN_00482fb0(0xf0804);
  puStack_8 = (undefined *)0xc;
  if (puVar4 == (undefined4 *)0x0) {
    DAT_006581bc = (undefined4 *)0x0;
  }
  else {
    DAT_006581bc = FUN_0049d9a0(puVar4);
  }
  ExceptionList = pcVar3;
  return 1;
}



// Function at 00440280

void cls_0x5a43dc::virt_meth_0x440280()

{
  if (this->mbr_0x50 != 0) {
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
    (*this->vftptr_0x0->virt_meth_0x4402b0_84)(this);
    return;
  }
  this->virt_meth_0x435de0();
  return;
}



// Function at 00445160

cls_0x5a43dc::cls_0x5a43dc()

{
  this->mbr_0x8 = 0x154;
  this->mbr_0x18 = 0x154;
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->vftptr_0x0 = (cls_0x5a43dc__vftable_5a43dc *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x10 = 0x10;
  this->mbr_0x20 = 0x10;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->mbr_0x584 = 0;
  this->mbr_0x580 = 0;
  this->mbr_0x57c = 0;
  this->vftptr_0x0 = &cls_0x5a43dc__vftable_5a43dc_005a43dc;
  return this;
}



// Function at 00445270

cls_0x5a43dc * cls_0x5a43dc::virt_meth_0x445270()

{
  byte in_stack_00000004;
  
  ~cls_0x5a43dc(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00445290

cls_0x5a43dc * __thiscall cls_0x5a43dc::~cls_0x5a43dc(cls_0x5a43dc *this)

{
  cls_0x5a43dc *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059ce56;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a43dc__vftable_5a43dc *)&PTR_virt_meth_0x434e40_005a45c8;
  local_4 = 0;
  this->virt_meth_0x434f30();
  this->vftptr_0x0 = (cls_0x5a43dc__vftable_5a43dc *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a43dc *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



