// Decompiled methods and structure for class: cls_0x5b4ecc

/*
/OOAnalyzer/cls_0x5b4ecc
pack(disabled)
Structure cls_0x5b4ecc {
   0   cls_0x5b4ecc::vftable_5b4ecc *   4   vftptr_0x0   "pointer to cls_0x5b4ecc::vftable_5b4ecc"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 176 Alignment: 1

*/

// Function at 005175d0

cls_0x5b4ecc * __thiscall OOAnalyzer::cls_0x5b4ecc::virt_meth_0x5175d0(cls_0x5b4ecc *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b4ecc(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005175f0

cls_0x5b4ecc * __thiscall OOAnalyzer::cls_0x5b4ecc::~cls_0x5b4ecc(cls_0x5b4ecc *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1098;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b4ecc__vftable_5b4ecc_005b4ecc;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b4ecc *)pcVar1;
}



// Function at 00520930

cls_0x5b4f30_TPlayer * __thiscall
OOAnalyzer::cls_0x5b4ecc::virt_meth_0x520930(cls_0x5b4ecc *this,undefined4 param_1)

{
  cls_0x5b4f30_TPlayer *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a127b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  pcVar1 = (cls_0x5b4f30_TPlayer *)FUN_00482fb0(0x674,this);
  local_4 = 0;
  if (pcVar1 != (cls_0x5b4f30_TPlayer *)0x0) {
    pcVar1 = cls_0x5b4f30_TPlayer::cls_0x5b4f30_TPlayer(pcVar1,param_1);
    ExceptionList = local_c;
    return pcVar1;
  }
  ExceptionList = local_c;
  return (cls_0x5b4f30_TPlayer *)0x0;
}



