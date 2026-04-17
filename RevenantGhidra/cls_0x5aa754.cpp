// Decompiled methods and structure for class: cls_0x5aa754

/*
/OOAnalyzer/cls_0x5aa754
pack(disabled)
Structure cls_0x5aa754 {
   0   cls_0x5aa754::vftable_5aa754 *   4   vftptr_0x0   "pointer to cls_0x5aa754::vftable_5aa754"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   280   dword   4   mbr_0x118   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 284 Alignment: 1

*/

// Function at 004f6790

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5aa754::virt_meth_0x4f6790(cls_0x5aa754 *this)

{
  cls_0x5a47f0 *this_00;
  undefined4 *puVar1;
  int iVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f3d6;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x124);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4._0_1_ = 2;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x410ca0_005a370c;
    local_4 = CONCAT31(local_4._1_3_,3);
    this_00[4].mbr_0xc = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
    puVar1 = FUN_00482fb0(0x6e0);
    this_00[4].mbr_0x10 = (dword)puVar1;
    puVar1 = puVar1 + 0x15;
    iVar2 = 0x14;
    do {
      *puVar1 = 0;
      puVar1 = puVar1 + 0x16;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    this_00[4].mbr_0x28 = 0x14;
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5aa754__vftable_5aa754_005aa754;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f7880

cls_0x5aa754 * __thiscall OOAnalyzer::cls_0x5aa754::virt_meth_0x4f7880(cls_0x5aa754 *this)

{
  byte in_stack_00000004;
  
  cls_0x4f78a0::~cls_0x4f78a0((cls_0x4f78a0 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f78b0

cls_0x5aa754 * __thiscall OOAnalyzer::cls_0x5aa754::virt_meth_0x4f78b0(cls_0x5aa754 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5aa754(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f78d0

cls_0x5aa754 * __thiscall OOAnalyzer::cls_0x5aa754::~cls_0x5aa754(cls_0x5aa754 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  uint local_4;
  
  puStack_8 = &LAB_0059f756;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5aa754__vftable_5aa754_005aa754;
  local_4 = 1;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = local_4 & 0xffffff00;
  this->mbr_0xfc = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0((LPCVOID)this->mbr_0x100);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5aa754 *)pcVar1;
}



