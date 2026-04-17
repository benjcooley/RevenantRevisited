// Decompiled methods and structure for class: cls_0x5b8e34

/*
/OOAnalyzer/cls_0x5b8e34
pack(disabled)
Structure cls_0x5b8e34 {
   0   cls_0x5b8e34::vftable_5b8e34 *   4   vftptr_0x0   "pointer to cls_0x5b8e34::vftable_5b8e34"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 176 Alignment: 1

*/

// Function at 00528a20

cls_0x5b8e34 * __thiscall OOAnalyzer::cls_0x5b8e34::virt_meth_0x528a20(cls_0x5b8e34 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b8e34(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00528a40

cls_0x5b8e34 * __thiscall OOAnalyzer::cls_0x5b8e34::~cls_0x5b8e34(cls_0x5b8e34 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1758;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b8e34__vftable_5b8e34_005b8e34;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b8e34 *)pcVar1;
}



