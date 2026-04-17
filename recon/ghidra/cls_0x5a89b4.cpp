// Decompiled methods and structure for class: cls_0x5a89b4

/*
/OOAnalyzer/cls_0x5a89b4
pack(disabled)
Structure cls_0x5a89b4 {
   0   cls_0x5a89b4::vftable_5a89b4 *   4   vftptr_0x0   "pointer to cls_0x5a89b4::vftable_5a89b4"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   432   dword   4   mbr_0x1b0   "Unsigned Double-Word (ddw, 4-bytes)"
   436   dword   4   mbr_0x1b4   "Unsigned Double-Word (ddw, 4-bytes)"
   440   dword   4   mbr_0x1b8   "Unsigned Double-Word (ddw, 4-bytes)"
   612   dword   4   mbr_0x264   "Unsigned Double-Word (ddw, 4-bytes)"
   616   dword   4   mbr_0x268   "Unsigned Double-Word (ddw, 4-bytes)"
   620   dword   4   mbr_0x26c   "Unsigned Double-Word (ddw, 4-bytes)"
   624   dword   4   mbr_0x270   "Unsigned Double-Word (ddw, 4-bytes)"
   628   dword   4   mbr_0x274   "Unsigned Double-Word (ddw, 4-bytes)"
   632   dword   4   mbr_0x278   "Unsigned Double-Word (ddw, 4-bytes)"
   636   dword   4   mbr_0x27c   "Unsigned Double-Word (ddw, 4-bytes)"
   640   dword   4   mbr_0x280   "Unsigned Double-Word (ddw, 4-bytes)"
   644   dword   4   mbr_0x284   "Unsigned Double-Word (ddw, 4-bytes)"
   648   dword   4   mbr_0x288   "Unsigned Double-Word (ddw, 4-bytes)"
   652   dword   4   mbr_0x28c   "Unsigned Double-Word (ddw, 4-bytes)"
   656   dword   4   mbr_0x290   "Unsigned Double-Word (ddw, 4-bytes)"
   660   dword   4   mbr_0x294   "Unsigned Double-Word (ddw, 4-bytes)"
   664   dword   4   mbr_0x298   "Unsigned Double-Word (ddw, 4-bytes)"
   668   dword   4   mbr_0x29c   "Unsigned Double-Word (ddw, 4-bytes)"
   672   dword   4   mbr_0x2a0   "Unsigned Double-Word (ddw, 4-bytes)"
   676   dword   4   mbr_0x2a4   "Unsigned Double-Word (ddw, 4-bytes)"
   680   dword   4   mbr_0x2a8   "Unsigned Double-Word (ddw, 4-bytes)"
   688   dword   4   mbr_0x2b0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 692 Alignment: 1

*/

// Function at 004df3a0

void __thiscall OOAnalyzer::cls_0x5a89b4::virt_meth_0x4df3a0(cls_0x5a89b4 *this)

{
  dword *pdVar1;
  int iVar2;
  
  cls_0x5a7e38::virt_meth_0x40dd60((cls_0x5a7e38 *)this);
  pdVar1 = &this->mbr_0x1b4;
  iVar2 = 0xf;
  do {
    pdVar1[1] = 0;
    *pdVar1 = 0;
    pdVar1[-1] = 0;
    pdVar1[-0x2c] = 0;
    pdVar1[-0x2d] = 0;
    pdVar1[-0x2e] = 0;
    pdVar1 = pdVar1 + 3;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  cls_0x40b770::meth_0x40b770((cls_0x40b770 *)this->mbr_0x8,0);
  this->mbr_0x26c = 0;
  this->mbr_0x278 = 0;
  this->mbr_0x27c = 0;
  this->mbr_0x280 = 0;
  this->mbr_0x284 = 0;
  this->mbr_0x268 = 0;
  this->mbr_0x270 = 0;
  this->mbr_0x274 = 0;
  this->mbr_0x288 = 0;
  this->mbr_0x28c = 0;
  this->mbr_0x290 = 0;
  this->mbr_0x294 = 0;
  this->mbr_0x298 = 0;
  this->mbr_0x29c = 0;
  this->mbr_0x2a0 = 0;
  this->mbr_0x2a4 = 0;
  this->mbr_0x2a8 = 0;
  this->mbr_0x2b0 = 0;
  cls_0x40b770::meth_0x40b830((cls_0x40b770 *)this->mbr_0x8,0);
  return;
}



// Function at 004f4bc0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5a89b4::virt_meth_0x4f4bc0(cls_0x5a89b4 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ed2e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x7c8);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5a89b4__vftable_5a89b4_005a89b4;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f70f0

cls_0x5a89b4 * __thiscall OOAnalyzer::cls_0x5a89b4::virt_meth_0x4f70f0(cls_0x5a89b4 *this)

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7120

cls_0x5a89b4 * __thiscall OOAnalyzer::cls_0x5a89b4::virt_meth_0x4f7120(cls_0x5a89b4 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a89b4(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7140

cls_0x5a89b4 * __thiscall OOAnalyzer::cls_0x5a89b4::~cls_0x5a89b4(cls_0x5a89b4 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f578;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a89b4__vftable_5a89b4_005a89b4;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5a89b4 *)pcVar1;
}



