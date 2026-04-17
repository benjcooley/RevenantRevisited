// Decompiled methods and structure for class: cls_0x5b45ac

/*
/OOAnalyzer/cls_0x5b45ac
pack(disabled)
Structure cls_0x5b45ac {
   0   cls_0x5b45ac::vftable_5b45ac *   4   vftptr_0x0   "pointer to cls_0x5b45ac::vftable_5b45ac"
   12   word   2   mbr_0xc   "Unsigned Word (dw, 2-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   224   dword   4   mbr_0xe0   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   byte   1   mbr_0x190   "Unsigned Byte (db)"
}
Length: 401 Alignment: 1

*/

// Function at 00512bf0

void __thiscall OOAnalyzer::cls_0x5b45ac::virt_meth_0x512bf0(cls_0x5b45ac *this)

{
  undefined uVar1;
  undefined uVar2;
  undefined4 uVar3;
  
  cls_0x5b401c::virt_meth_0x510220((cls_0x5b401c *)this);
  if ((this->mbr_0xc == 1) && (this->mbr_0x190 != 0)) {
    uVar3 = 1;
    uVar2 = 0x7f;
    this->mbr_0x190 = 0;
    uVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
    cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,uVar1,uVar2,uVar3);
  }
  return;
}



// Function at 00514fb0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b45ac::virt_meth_0x514fb0(cls_0x5b45ac *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0e3e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x1cc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x515620_005b47b0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00515590

cls_0x5b45ac * __thiscall OOAnalyzer::cls_0x5b45ac::virt_meth_0x515590(cls_0x5b45ac *this)

{
  byte in_stack_00000004;
  
  cls_0x5155b0::~cls_0x5155b0((cls_0x5155b0 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00515640

cls_0x5b45ac * __thiscall OOAnalyzer::cls_0x5b45ac::~cls_0x5b45ac(cls_0x5b45ac *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0f88;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5b45ac__vftable_5b45ac *)&PTR_virt_meth_0x515620_005b47b0;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b45ac *)pcVar1;
}



