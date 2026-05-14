// Decompiled methods and structure for class: cls_0x5253e0

/*
/OOAnalyzer/cls_0x5253e0
pack(disabled)
Structure cls_0x5253e0 {
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   220   dword   4   mbr_0xdc   "Unsigned Double-Word (ddw, 4-bytes)"
   224   dword   4   mbr_0xe0   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 256 Alignment: 1

*/

// Function at 005253e0

void __thiscall OOAnalyzer::cls_0x5253e0::meth_0x5253e0(cls_0x5253e0 *this)

{
  uint uVar1;
  
  uVar1 = FUN_0059a530_stricmp((uint)this,(byte *)this->mbr_0x38,(byte *)s_DunFireWallS_005e2fc4);
  if (uVar1 == 0) {
    this->mbr_0xfc = 0x80;
  }
  else {
    this->mbr_0xfc = 0x40;
  }
  this->mbr_0xdc = 0xffffffff;
  this->mbr_0xd8 = 0;
  this->mbr_0xe0 = 0;
  this->mbr_0x8 = this->mbr_0x8 | 0x40000;
  return;
}



