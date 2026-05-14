// Decompiled methods and structure for class: cls_0x5b2764

/*
/OOAnalyzer/cls_0x5b2764
pack(disabled)
Structure cls_0x5b2764 {
   0   cls_0x5b2764::vftable_5b2764 *   4   vftptr_0x0   "pointer to cls_0x5b2764::vftable_5b2764"
   12   word   2   mbr_0xc   "Unsigned Word (dw, 2-bytes)"
}
Length: 14 Alignment: 1

*/

// Function at 0050d230

void __thiscall OOAnalyzer::cls_0x5b2764::virt_meth_0x50d230(cls_0x5b2764 *this)

{
  word wVar1;
  int iVar2;
  
  wVar1 = this->mbr_0xc;
  if ((wVar1 == 1) || (wVar1 == 0)) {
    iVar2 = cls_0x5a7b98_TCharacter::meth_0x50d2b0((cls_0x5a7b98_TCharacter *)this);
    if (iVar2 != 0) {
      cls_0x5a7b98_TCharacter::meth_0x50d530((cls_0x5a7b98_TCharacter *)this);
      return;
    }
    cls_0x5a7b98_TCharacter::meth_0x50d530((cls_0x5a7b98_TCharacter *)this);
  }
  else if ((wVar1 == 3) || (wVar1 == 2)) {
    iVar2 = cls_0x5a7b98_TCharacter::meth_0x50d2b0((cls_0x5a7b98_TCharacter *)this);
    if (iVar2 != 0) {
      cls_0x5a7b98_TCharacter::meth_0x50d530((cls_0x5a7b98_TCharacter *)this);
      return;
    }
    cls_0x5a7b98_TCharacter::meth_0x50d530((cls_0x5a7b98_TCharacter *)this);
    return;
  }
  return;
}



// Function at 0050e030

cls_0x5b2764 * __thiscall OOAnalyzer::cls_0x5b2764::virt_meth_0x50e030(cls_0x5b2764 *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050e060

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b2764::virt_meth_0x50e060(cls_0x5b2764 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0a6e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0xfc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b2764__vftable_5b2764_005b2764;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 0050e100

cls_0x5b2764 * __thiscall OOAnalyzer::cls_0x5b2764::virt_meth_0x50e100(cls_0x5b2764 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b2764(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050e120

cls_0x5b2764 * __thiscall OOAnalyzer::cls_0x5b2764::~cls_0x5b2764(cls_0x5b2764 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0a88;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b2764__vftable_5b2764_005b2764;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b2764 *)pcVar1;
}



// Function at 0050e430

cls_0x5a50e8 * __thiscall
OOAnalyzer::cls_0x5b2764::virt_meth_0x50e430(cls_0x5b2764 *this,undefined4 param_1)

{
  cls_0x5a50e8 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0acb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a50e8 *)FUN_00482fb0(0x108);
  local_4 = 0;
  if (this_00 != (cls_0x5a50e8 *)0x0) {
    cls_0x5a50e8::cls_0x5a50e8(this_00);
    *(undefined4 *)&this_00->field_0xdc = 0;
    this_00->mbr_0xe0 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    *(undefined4 *)&this_00->field_0xe4 = 0;
    *(undefined4 *)&this_00->field_0xe8 = 0;
    this_00->vftptr_0x0 = (cls_0x5a50e8__vftable_5a50e8 *)&PTR_virt_meth_0x50e4e0_005b2a4c;
    this_00->mbr_0x104 = 3;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a50e8 *)0x0;
}



