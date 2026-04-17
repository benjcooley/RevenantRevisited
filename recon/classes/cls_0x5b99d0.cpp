// Decompiled methods and structure for class: cls_0x5b99d0

/*
/OOAnalyzer/cls_0x5b99d0
pack(disabled)
Structure cls_0x5b99d0 {
   0   cls_0x5b99d0::vftable_5b99d0 *   4   vftptr_0x0   "pointer to cls_0x5b99d0::vftable_5b99d0"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 00540990

cls_0x5b99d0 * __thiscall OOAnalyzer::cls_0x5b99d0::cls_0x5b99d0(cls_0x5b99d0 *this)

{
  int iVar1;
  dword in_stack_00000004;
  
  iVar1 = DAT_0067021c;
  this->vftptr_0x0 = &cls_0x5b99d0__vftable_5b99d0_005b99d0;
  if (iVar1 < 0x100) {
    (&DAT_00670220)[iVar1] = this;
    DAT_0067021c = iVar1 + 1;
    this->mbr_0x4 = in_stack_00000004;
    return this;
  }
  this->mbr_0x4 = in_stack_00000004;
  return this;
}



// Function at 00542170

cls_0x5a85ac * __thiscall
OOAnalyzer::cls_0x5b99d0::virt_meth_0x542170(cls_0x5b99d0 *this,undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  cls_0x5a85ac *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1e0b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x184);
  local_4 = 0;
  pcVar1 = (cls_0x5a85ac *)0x0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    (this_00->cls_0x5a50e8).vftptr_0x0 =
         (cls_0x5a50e8__vftable_5a50e8 *)&PTR_virt_meth_0x5421d0_005b99d8;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



