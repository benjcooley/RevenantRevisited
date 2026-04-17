// Decompiled methods and structure for class: cls_0x5a8a20

/*
/OOAnalyzer/cls_0x5a8a20
pack(disabled)
Structure cls_0x5a8a20 {
   0   cls_0x5a8a20::vftable_5a8a20 *   4   vftptr_0x0   "pointer to cls_0x5a8a20::vftable_5a8a20"
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
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
   972   dword   4   mbr_0x3cc   "Unsigned Double-Word (ddw, 4-bytes)"
   976   dword   4   mbr_0x3d0   "Unsigned Double-Word (ddw, 4-bytes)"
   1452   dword   4   mbr_0x5ac   "Unsigned Double-Word (ddw, 4-bytes)"
   1456   dword   4   mbr_0x5b0   "Unsigned Double-Word (ddw, 4-bytes)"
   1692   dword   4   mbr_0x69c   "Unsigned Double-Word (ddw, 4-bytes)"
   1696   dword   4   mbr_0x6a0   "Unsigned Double-Word (ddw, 4-bytes)"
   1700   dword   4   mbr_0x6a4   "Unsigned Double-Word (ddw, 4-bytes)"
   1704   dword   4   mbr_0x6a8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 1708 Alignment: 1

*/

// Function at 004f7190

cls_0x5a8a20 * __thiscall OOAnalyzer::cls_0x5a8a20::virt_meth_0x4f7190(cls_0x5a8a20 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a8a20(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f71b0

cls_0x5a8a20 * __thiscall OOAnalyzer::cls_0x5a8a20::~cls_0x5a8a20(cls_0x5a8a20 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f598;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a8a20__vftable_5a8a20_005a8a20;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5a8a20 *)pcVar1;
}



