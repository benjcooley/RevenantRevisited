// Decompiled methods and structure for class: cls_0x5a4a74

/*
/OOAnalyzer/cls_0x5a4a74
pack(disabled)
Structure cls_0x5a4a74 {
   0   cls_0x5a4a74::vftable_5a4a74 *   4   vftptr_0x0   "pointer to cls_0x5a4a74::vftable_5a4a74"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   384   dword   4   mbr_0x180   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 392 Alignment: 1

*/

// Function at 00469a60

bool __thiscall OOAnalyzer::cls_0x5a4a74::virt_meth_0x469a60(cls_0x5a4a74 *this)

{
  int iVar1;
  
  iVar1 = cls_0x5b93c4::virt_meth_0x435150
                    ((cls_0x5b93c4 *)this,s_hostgame_005d3cc8,0x659cbc,0,0,0,0x280,0x1e0,0x1c2,0xa0,
                     s_widgets_005d3cc0);
  return iVar1 != 0;
}



// Function at 0046d390

cls_0x5a4a74 * __thiscall OOAnalyzer::cls_0x5a4a74::virt_meth_0x46d390(cls_0x5a4a74 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a4a74(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0046d3b0

cls_0x5a4a74 * __thiscall OOAnalyzer::cls_0x5a4a74::~cls_0x5a4a74(cls_0x5a4a74 *this)

{
  cls_0x5a4a74 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d1de;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a4a74__vftable_5a4a74 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a4a74 *)FUN_004830f0((LPCVOID)this->mbr_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



