// Decompiled methods and structure for class: cls_0x5b30fc

/*
/OOAnalyzer/cls_0x5b30fc
pack(disabled)
Structure cls_0x5b30fc {
   0   cls_0x5b30fc::vftable_5b30fc *   4   vftptr_0x0   "pointer to cls_0x5b30fc::vftable_5b30fc"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 240 Alignment: 1

*/

// Function at 0050f6e0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b30fc::virt_meth_0x50f6e0(cls_0x5b30fc *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0b6e;
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
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x50fa60_005b3164;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 0050f780

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b30fc::virt_meth_0x50f780(cls_0x5b30fc *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0b9e;
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
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x50fc00_005b31cc;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 0050f8c0

cls_0x5b30fc * __thiscall OOAnalyzer::cls_0x5b30fc::virt_meth_0x50f8c0(cls_0x5b30fc *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b30fc(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050f8e0

cls_0x5b30fc * __thiscall OOAnalyzer::cls_0x5b30fc::~cls_0x5b30fc(cls_0x5b30fc *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0bd8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b30fc__vftable_5b30fc_005b30fc;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b30fc *)pcVar1;
}



// Function at 0050f930

cls_0x5a50e8 * __thiscall
OOAnalyzer::cls_0x5b30fc::virt_meth_0x50f930(cls_0x5b30fc *this,undefined4 param_1)

{
  cls_0x5a50e8 *this_00;
  cls_0x5a50e8 *pcVar1;
  int iVar2;
  dword dVar3;
  dword dVar4;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0c03;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a50e8 *)FUN_00482fb0(0xf0);
  local_4 = 0;
  pcVar1 = (cls_0x5a50e8 *)0x0;
  if (this_00 != (cls_0x5a50e8 *)0x0) {
    cls_0x5a50e8::cls_0x5a50e8(this_00);
    this_00->mbr_0x8 = this_00->mbr_0x8 | 1;
    this_00->vftptr_0x0 = (cls_0x5a50e8__vftable_5a50e8 *)&PTR_virt_meth_0x50fa30_005b341c;
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->mbr_0xd8 = this_00->mbr_0x10;
    dVar3 = this_00->mbr_0xd8 + 10;
    *(dword *)&this_00->field_0xdc = this_00->mbr_0x14;
    this_00->mbr_0xe0 = this_00->mbr_0x18;
    this_00->mbr_0xd8 = dVar3;
    iVar2 = *(int *)&this_00->field_0xdc + 10;
    dVar4 = this_00->mbr_0xe0 + 10;
    *(int *)&this_00->field_0xdc = iVar2;
    this_00->mbr_0xe0 = dVar4;
    *(dword *)&this_00->field_0xe4 = dVar3 - this_00->mbr_0x10;
    *(dword *)&this_00->field_0xe8 = iVar2 - this_00->mbr_0x14;
    *(dword *)&this_00->field_0xec = dVar4 - this_00->mbr_0x18;
    cls_0x497c40::meth_0x497c40((cls_0x497c40 *)&DAT_0065def0);
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 0050fa60

cls_0x5b30fc * __thiscall OOAnalyzer::cls_0x5b30fc::virt_meth_0x50fa60(cls_0x5b30fc *this)

{
  byte in_stack_00000004;
  
  cls_0x50fa80::~cls_0x50fa80((cls_0x50fa80 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050fad0

cls_0x5a50e8 * __thiscall
OOAnalyzer::cls_0x5b30fc::virt_meth_0x50fad0(cls_0x5b30fc *this,undefined4 param_1)

{
  cls_0x5a50e8 *this_00;
  cls_0x5a50e8 *pcVar1;
  int iVar2;
  dword dVar3;
  dword dVar4;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0c43;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a50e8 *)FUN_00482fb0(0xf0);
  local_4 = 0;
  pcVar1 = (cls_0x5a50e8 *)0x0;
  if (this_00 != (cls_0x5a50e8 *)0x0) {
    cls_0x5a50e8::cls_0x5a50e8(this_00);
    this_00->mbr_0x8 = this_00->mbr_0x8 | 1;
    this_00->vftptr_0x0 = (cls_0x5a50e8__vftable_5a50e8 *)&PTR_virt_meth_0x50fa30_005b341c;
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->mbr_0xd8 = this_00->mbr_0x10;
    dVar3 = this_00->mbr_0xd8 + 10;
    *(dword *)&this_00->field_0xdc = this_00->mbr_0x14;
    this_00->mbr_0xe0 = this_00->mbr_0x18;
    this_00->mbr_0xd8 = dVar3;
    iVar2 = *(int *)&this_00->field_0xdc + 10;
    dVar4 = this_00->mbr_0xe0 + 10;
    *(int *)&this_00->field_0xdc = iVar2;
    this_00->mbr_0xe0 = dVar4;
    *(dword *)&this_00->field_0xe4 = dVar3 - this_00->mbr_0x10;
    *(dword *)&this_00->field_0xe8 = iVar2 - this_00->mbr_0x14;
    *(dword *)&this_00->field_0xec = dVar4 - this_00->mbr_0x18;
    cls_0x497c40::meth_0x497c40((cls_0x497c40 *)&DAT_0065def0);
    this_00->vftptr_0x0 = (cls_0x5a50e8__vftable_5a50e8 *)&PTR_virt_meth_0x50fbd0_005b360c;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



