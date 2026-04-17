#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5aacac



// Function at 004eaae0

undefined4 cls_0x5aacac::virt_meth_0x4eaae0()

{
  cls_0x411eb0::meth_0x4178e0((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),8);
  (**(code **)(this->mbr_0x158 + 0xc))(0,0);
  (**(code **)(this->mbr_0xfc + 0xc))(0,0);
  (**(code **)(this->mbr_0x124 + 0xc))(0,0);
  cls_0x411eb0::meth_0x417b00((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  return 1;
}



// Function at 004f6d50

cls_0x5aacac * cls_0x5aacac::virt_meth_0x4f6d50()

{
  byte in_stack_00000004;
  
  ~cls_0x5aacac(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f6d70

cls_0x5aacac * __thiscall cls_0x5aacac::~cls_0x5aacac(cls_0x5aacac *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f502;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5aacac__vftable_5aacac_005aacac;
  local_4 = 3;
  this->virt_meth_0x40de10();
  local_4._0_1_ = 2;
  this->mbr_0x158 = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0((LPCVOID)this->mbr_0x15c);
  local_4._0_1_ = 1;
  this->mbr_0x124 = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0((LPCVOID)this->mbr_0x128);
  local_4 = (uint)local_4._1_3_ << 8;
  this->mbr_0xfc = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0((LPCVOID)this->mbr_0x100);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5aacac *)pcVar1;
}



