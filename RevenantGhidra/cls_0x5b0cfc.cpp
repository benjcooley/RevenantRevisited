// Decompiled methods and structure for class: cls_0x5b0cfc

/*
/OOAnalyzer/cls_0x5b0cfc
pack(disabled)
Structure cls_0x5b0cfc {
   0   cls_0x5b0cfc::vftable_5b0cfc *   4   vftptr_0x0   "pointer to cls_0x5b0cfc::vftable_5b0cfc"
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
   280   dword   4   mbr_0x118   "Unsigned Double-Word (ddw, 4-bytes)"
   284   dword   4   mbr_0x11c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 288 Alignment: 1

*/

// Function at 00503740

void __thiscall OOAnalyzer::cls_0x5b0cfc::virt_meth_0x503740(cls_0x5b0cfc *this)

{
  undefined4 uVar1;
  
  cls_0x5a7e38::virt_meth_0x40dd60((cls_0x5a7e38 *)this);
  uVar1 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  cls_0x5a486c::meth_0x40a0c0((cls_0x5a486c *)this->mbr_0x8,uVar1);
  this->mbr_0x114 = 0x42c80000;
  this->mbr_0x118 = 0;
  this->mbr_0x11c = 0;
  return;
}



// Function at 00509620

cls_0x5b0cfc * __thiscall OOAnalyzer::cls_0x5b0cfc::virt_meth_0x509620(cls_0x5b0cfc *this)

{
  byte in_stack_00000004;
  
  cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509650

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b0cfc::virt_meth_0x509650(cls_0x5b0cfc *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a06de;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x104);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b0cfc__vftable_5b0cfc_005b0cfc;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 005096f0

cls_0x5b0cfc * __thiscall OOAnalyzer::cls_0x5b0cfc::virt_meth_0x5096f0(cls_0x5b0cfc *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b0cfc(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509710

cls_0x5b0cfc * __thiscall OOAnalyzer::cls_0x5b0cfc::~cls_0x5b0cfc(cls_0x5b0cfc *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a06f8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b0cfc__vftable_5b0cfc_005b0cfc;
  DAT_0066d198 = DAT_0066d198 + -1;
  local_4 = 0;
  if (DAT_0066d198 < 1) {
    FUN_004830f0(DAT_0066d170);
    DAT_0066d170 = (LPCVOID)0x0;
    FUN_004830f0(DAT_0066d174);
    DAT_0066d174 = (LPCVOID)0x0;
    FUN_004830f0(DAT_0066d1a4);
    DAT_0066d1a4 = (LPCVOID)0x0;
    DAT_0066d198 = 0;
    DAT_0066d19c = 0;
    DAT_0066d1a0 = 0;
    DAT_0066d1a8 = 0;
  }
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b0cfc *)pcVar1;
}



