// Decompiled methods and structure for class: cls_0x49a270

/*
/OOAnalyzer/cls_0x49a270
pack(disabled)
Structure cls_0x49a270 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 68 Alignment: 1

*/

// Function at 0049a270

undefined4 __thiscall OOAnalyzer::cls_0x49a270::meth_0x49a270(cls_0x49a270 *this)

{
  dword dVar1;
  undefined4 uVar2;
  
  if (DAT_00668114 != 0) {
    return 1;
  }
  uVar2 = 0;
  if (this->mbr_0x0 == 0) {
    dVar1 = _AIL_redbook_open_4(0);
    this->mbr_0x0 = dVar1;
    if (dVar1 == 0) goto LAB_0049a2b1;
  }
  uVar2 = 1;
  dVar1 = _AIL_redbook_volume_4(this->mbr_0x0);
  this->mbr_0xc = dVar1;
  this->mbr_0x4 = 0x60;
LAB_0049a2b1:
  this->mbr_0x40 = 0;
  this->mbr_0x3c = 0xffffffff;
  this->mbr_0x24 = 0;
  this->mbr_0x8 = 0;
  return uVar2;
}



