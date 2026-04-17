#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a9e50



// Function at 004e4ec0

void cls_0x5a9e50::virt_meth_0x4e4ec0()

{
  undefined4 uVar1;
  
  this->virt_meth_0x40dd60();
  uVar1 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  cls_0x5a486c::meth_0x40a0c0((cls_0x5a486c *)this->mbr_0x8,uVar1);
  this->mbr_0xfc = 0;
  return;
}



// Function at 004e4ef0

void cls_0x5a9e50::virt_meth_0x4e4ef0()

{
  dword dVar1;
  
  this->virt_meth_0x40e2e0();
  dVar1 = this->mbr_0xfc + 1;
  this->mbr_0xfc = dVar1;
  if (0x11 < (int)dVar1) {
    this->mbr_0xfc = 0;
  }
  return;
}



// Function at 004e5260

undefined4 cls_0x5a9e50::virt_meth_0x4e5260()

{
  cls_0x5a486c *this_00;
  undefined4 *puVar1;
  float *pfVar2;
  int iVar3;
  
  cls_0x411eb0::meth_0x4178e0((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),2);
  puVar1 = (undefined4 *)cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  FUN_0040c960();
  iVar3 = puVar1[0x28];
  if (0 < iVar3) {
    pfVar2 = (float *)(puVar1[0x29] + 0x18);
    do {
      iVar3 = iVar3 + -1;
      *pfVar2 = (float)this->mbr_0xfc + *pfVar2;
      pfVar2 = pfVar2 + 8;
    } while (iVar3 != 0);
  }
  puVar1[0x11] = 0x3fb33333;
  puVar1[0x10] = 0x3fb33333;
  puVar1[0x12] = this->mbr_0x100;
  this_00 = (cls_0x5a486c *)this->mbr_0x8;
  *puVar1 = 0x2040;
  cls_0x5a486c::meth_0x40a8f0(this_00);
  FUN_0040c9c0(&this->mbr_0x9c);
  if ((this->mbr_0x3c & 1) != 0) {
    FUN_0040ca20((int *)&this->mbr_0x9c);
  }
  this->mbr_0xec = 1;
  if ((this->mbr_0x3c & 2) != 0) {
    cls_0x5a486c::meth_0x40ca90((cls_0x5a486c *)this->mbr_0x8,this->mbr_0x4,this->mbr_0xc);
  }
  this->mbr_0xec = 1;
  cls_0x411eb0::meth_0x417b00((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  return 1;
}



// Function at 004f5e10

cls_0x5a47f0 * cls_0x5a9e50::virt_meth_0x4f5e10()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f16e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x10c);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5a9e50__vftable_5a9e50_005a9e50;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f7730

cls_0x5a9e50 * cls_0x5a9e50::virt_meth_0x4f7730()

{
  byte in_stack_00000004;
  
  cls_0x5a9d88::~cls_0x5a9d88((cls_0x5a9d88 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f77a0

cls_0x5a9e50 * cls_0x5a9e50::virt_meth_0x4f77a0()

{
  byte in_stack_00000004;
  
  ~cls_0x5a9e50(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f77c0

cls_0x5a9e50 * __thiscall cls_0x5a9e50::~cls_0x5a9e50(cls_0x5a9e50 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f708;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a9e50__vftable_5a9e50_005a9e50;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5a9e50 *)pcVar1;
}



