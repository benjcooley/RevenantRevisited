// Decompiled methods and structure for class: cls_0x5b42f0

/*
/OOAnalyzer/cls_0x5b42f0
pack(disabled)
Structure cls_0x5b42f0 {
   0   cls_0x5b42f0::vftable_5b42f0 *   4   vftptr_0x0   "pointer to cls_0x5b42f0::vftable_5b42f0"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   104   dword   4   mbr_0x68   "Unsigned Double-Word (ddw, 4-bytes)"
   108   dword   4   mbr_0x6c   "Unsigned Double-Word (ddw, 4-bytes)"
   112   dword   4   mbr_0x70   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 132 Alignment: 1

*/

// Function at 00515290

cls_0x5b42f0 * __thiscall OOAnalyzer::cls_0x5b42f0::~cls_0x5b42f0(cls_0x5b42f0 *this)

{
  cls_0x5b42f0 *pcVar1;
  
  this->vftptr_0x0 = &cls_0x5b42f0__vftable_5b42f0_005b42f0;
  pcVar1 = (cls_0x5b42f0 *)FUN_004830f0(this->mbr_0x6c);
  return pcVar1;
}



// Function at 005152b0

cls_0x5b42f0 * __thiscall OOAnalyzer::cls_0x5b42f0::~cls_0x5b42f0(cls_0x5b42f0 *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5b42f0__vftable_5b42f0_005b42f0;
  FUN_004830f0(this->mbr_0x6c);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



