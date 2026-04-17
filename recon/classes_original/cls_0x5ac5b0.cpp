// Decompiled methods and structure for class: cls_0x5ac5b0

/*
/OOAnalyzer/cls_0x5ac5b0
pack(disabled)
Structure cls_0x5ac5b0 {
   0   cls_0x5ac5b0::vftable_5ac5b0 *   4   vftptr_0x0   "pointer to cls_0x5ac5b0::vftable_5ac5b0"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   264   dword   4   mbr_0x108   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
   276   dword   4   mbr_0x114   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 392 Alignment: 1

*/

// Function at 004f0780

void __thiscall OOAnalyzer::cls_0x5ac5b0::virt_meth_0x4f0780(cls_0x5ac5b0 *this)

{
  int iVar1;
  dword dVar2;
  undefined4 uVar3;
  
  cls_0x5a7e38::virt_meth_0x40dd60((cls_0x5a7e38 *)this);
  this->mbr_0x104 = 0;
  this->mbr_0x100 = 0;
  iVar1 = *(int *)this->mbr_0x4;
  this->mbr_0x10c = 0x3f000000;
  dVar2 = (**(code **)(iVar1 + 0x200))();
  this->mbr_0xfc = dVar2;
  uVar3 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  cls_0x5a486c::meth_0x40a0c0((cls_0x5a486c *)this->mbr_0x8,uVar3);
  this->mbr_0x110 = 0;
  this->mbr_0x108 = 0;
  return;
}



// Function at 004f8dc0

cls_0x5ac5b0 * __thiscall OOAnalyzer::cls_0x5ac5b0::virt_meth_0x4f8dc0(cls_0x5ac5b0 *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8df0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5ac5b0::virt_meth_0x4f8df0(cls_0x5ac5b0 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fbae;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x118);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ac5b0__vftable_5ac5b0_005ac5b0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f8e90

cls_0x5ac5b0 * __thiscall OOAnalyzer::cls_0x5ac5b0::virt_meth_0x4f8e90(cls_0x5ac5b0 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5ac5b0(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8eb0

cls_0x5ac5b0 * __thiscall OOAnalyzer::cls_0x5ac5b0::~cls_0x5ac5b0(cls_0x5ac5b0 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059fbc8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ac5b0__vftable_5ac5b0_005ac5b0;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ac5b0 *)pcVar1;
}



