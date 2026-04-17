// Decompiled methods and structure for class: cls_0x5b4534

/*
/OOAnalyzer/cls_0x5b4534
pack(disabled)
Structure cls_0x5b4534 {
   0   cls_0x5b4534::vftable_5b4534 *   4   vftptr_0x0   "pointer to cls_0x5b4534::vftable_5b4534"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   224   dword   4   mbr_0xe0   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   264   dword   4   mbr_0x108   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   444   dword   4   mbr_0x1bc   "Unsigned Double-Word (ddw, 4-bytes)"
   448   dword   4   mbr_0x1c0   "Unsigned Double-Word (ddw, 4-bytes)"
   504   dword   4   mbr_0x1f8   "Unsigned Double-Word (ddw, 4-bytes)"
   508   dword   4   mbr_0x1fc   "Unsigned Double-Word (ddw, 4-bytes)"
   564   dword   4   mbr_0x234   "Unsigned Double-Word (ddw, 4-bytes)"
   568   dword   4   mbr_0x238   "Unsigned Double-Word (ddw, 4-bytes)"
   612   dword   4   mbr_0x264   "Unsigned Double-Word (ddw, 4-bytes)"
   616   dword   4   mbr_0x268   "Unsigned Double-Word (ddw, 4-bytes)"
   620   dword   4   mbr_0x26c   "Unsigned Double-Word (ddw, 4-bytes)"
   624   dword   4   mbr_0x270   "Unsigned Double-Word (ddw, 4-bytes)"
   628   dword   4   mbr_0x274   "Unsigned Double-Word (ddw, 4-bytes)"
   632   dword   4   mbr_0x278   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 636 Alignment: 1

*/

// Function at 005123f0

void __thiscall OOAnalyzer::cls_0x5b4534::virt_meth_0x5123f0(cls_0x5b4534 *this)

{
  (*this->vftptr_0x0->virt_meth_0x512490_24)(this);
  this->mbr_0xe0 = 0;
  this->mbr_0x184 = 0x8000;
  this->mbr_0x18c = 1;
  this->mbr_0x188 = 0;
  this->mbr_0x190 = 1;
  return;
}



// Function at 00512430

void __thiscall OOAnalyzer::cls_0x5b4534::virt_meth_0x512430(cls_0x5b4534 *this)

{
  cls_0x5b401c::virt_meth_0x510220((cls_0x5b401c *)this);
  if (*(short *)&this->mbr_0xc == 0) {
    if (this->mbr_0x190 != 0) {
      this->mbr_0x190 = 0;
    }
  }
  else if ((*(short *)&this->mbr_0xc == 1) && (this->mbr_0x80 != 0)) {
    this->mbr_0x184 = 0;
    return;
  }
  return;
}



// Function at 00514dd0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b4534::virt_meth_0x514dd0(cls_0x5b4534 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0dde;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x27c);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b4534__vftable_5b4534_005b4534;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 005154f0

cls_0x5b4534 * __thiscall OOAnalyzer::cls_0x5b4534::virt_meth_0x5154f0(cls_0x5b4534 *this)

{
  byte in_stack_00000004;
  
  cls_0x515510::~cls_0x515510((cls_0x515510 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00515520

cls_0x5b4534 * __thiscall OOAnalyzer::cls_0x5b4534::virt_meth_0x515520(cls_0x5b4534 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b4534(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00515540

cls_0x5b4534 * __thiscall OOAnalyzer::cls_0x5b4534::~cls_0x5b4534(cls_0x5b4534 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0f48;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b4534__vftable_5b4534_005b4534;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b4534 *)pcVar1;
}



