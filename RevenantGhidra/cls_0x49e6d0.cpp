// Decompiled methods and structure for class: cls_0x49e6d0

/*
/OOAnalyzer/cls_0x49e6d0
pack(disabled)
Structure cls_0x49e6d0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 48 Alignment: 1

*/

// Function at 0049e6d0

void __thiscall OOAnalyzer::cls_0x49e6d0::meth_0x49e6d0(cls_0x49e6d0 *this)

{
  if (this->mbr_0xc != 0) {
    this->mbr_0x18 = 0;
    this->mbr_0x1c = 0;
    this->mbr_0x28 = 0x10000;
    this->mbr_0x8 = 1;
    this->mbr_0x2c = (int)(this->mbr_0x0 << 0x10) / 0x18;
  }
  return;
}



// Function at 0049e710

void __thiscall OOAnalyzer::cls_0x49e6d0::meth_0x49e710(cls_0x49e6d0 *this)

{
  dword dVar1;
  int iVar2;
  int local_50 [20];
  
  if ((this->mbr_0xc != 0) && (this->mbr_0x8 != 0)) {
    iVar2 = 0;
    if (0 < (int)this->mbr_0x1c) {
      do {
        FUN_0058b100((char *)local_50,(byte *)s_video_03d_bmp_005dacf8);
        FUN_004a2960(*(int **)(*(int *)(this->mbr_0x10 + iVar2 * 4) + 100),local_50,1);
        iVar2 = iVar2 + 1;
      } while (iVar2 < (int)this->mbr_0x1c);
    }
    dVar1 = this->mbr_0x1c;
    this->mbr_0x1c = 0;
    this->mbr_0x18 = this->mbr_0x18 + dVar1;
    this->mbr_0x8 = 0;
  }
  return;
}



