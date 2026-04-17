// Decompiled methods and structure for class: cls_0x5a47f0

/*
/OOAnalyzer/cls_0x5a47f0
pack(disabled)
Structure cls_0x5a47f0 {
   0   cls_0x5a47f0::vftable_5a47f0 *   4   vftptr_0x0   "pointer to cls_0x5a47f0::vftable_5a47f0"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   byte   1   mbr_0x34   "Unsigned Byte (db)"
   53   byte   1   mbr_0x35   "Unsigned Byte (db)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 60 Alignment: 1

*/

// Function at 00445940

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5a47f0::cls_0x5a47f0(cls_0x5a47f0 *this)

{
  dword dVar1;
  TPlayScreen *in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5a47f0__vftable_5a47f0_005a47f0;
  this->mbr_0x4 = (dword)in_stack_00000004;
  dVar1 = TPlayScreen::meth_0x46e8a0(in_stack_00000004);
  this->mbr_0x8 = dVar1;
  this->mbr_0x10 = 0xffffffff;
  this->mbr_0x18 = 0xffffffff;
  this->mbr_0xc = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x28 = 0;
  this->mbr_0x24 = 0;
  this->mbr_0x20 = 0;
  this->mbr_0x30 = 0;
  this->mbr_0x35 = 0;
  this->mbr_0x34 = 0;
  this->mbr_0x38 = 0;
  this->mbr_0x1c = 1;
  this->mbr_0x2c = 1;
  return this;
}



// Function at 00445990

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5a47f0::~cls_0x5a47f0(cls_0x5a47f0 *this)

{
  cls_0x5a47f0 *pcVar1;
  
  pcVar1 = (cls_0x5a47f0 *)this->mbr_0x38;
  this->vftptr_0x0 = &cls_0x5a47f0__vftable_5a47f0_005a47f0;
  if (pcVar1 != (cls_0x5a47f0 *)0x0) {
    pcVar1 = (cls_0x5a47f0 *)FUN_004830f0(pcVar1);
    this->mbr_0x38 = 0;
  }
  return pcVar1;
}



// Function at 00448380

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5a47f0::~cls_0x5a47f0(cls_0x5a47f0 *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5a47f0__vftable_5a47f0_005a47f0;
  if ((LPCVOID)this->mbr_0x38 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x38);
    this->mbr_0x38 = 0;
  }
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



