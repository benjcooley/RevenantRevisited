// Decompiled methods and structure for class: cls_0x524400

/*
/OOAnalyzer/cls_0x524400
pack(disabled)
Structure cls_0x524400 {
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   220   dword   4   mbr_0xdc   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 260 Alignment: 1

*/

// Function at 00524400

void __thiscall OOAnalyzer::cls_0x524400::meth_0x524400(cls_0x524400 *this)

{
  uint uVar1;
  
  uVar1 = FUN_0059a530((uint)this,(byte *)this->mbr_0x38,(byte *)s_ArrowWallS_005e2f50);
  if (uVar1 == 0) {
    this->mbr_0xfc = 0x80;
  }
  else {
    this->mbr_0xfc = 0x40;
  }
  this->mbr_0xdc = 0xffffffff;
  this->mbr_0xd8 = 0;
  this->mbr_0x100 = 1;
  return;
}



