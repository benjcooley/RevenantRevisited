// Decompiled methods and structure for class: cls_0x49e610

/*
/OOAnalyzer/cls_0x49e610
pack(disabled)
Structure cls_0x49e610 {
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 36 Alignment: 1

*/

// Function at 0049e610

void __thiscall OOAnalyzer::cls_0x49e610::meth_0x49e610(cls_0x49e610 *this)

{
  dword dVar1;
  undefined4 *puVar2;
  int iVar3;
  int local_50 [20];
  
  if (this->mbr_0xc != 0) {
    if (this->mbr_0x8 != 0) {
      iVar3 = 0;
      if (0 < (int)this->mbr_0x1c) {
        do {
          FUN_0058b100((char *)local_50,(byte *)s_video_03d_bmp_005dacf8);
          FUN_004a2960(*(int **)(*(int *)(this->mbr_0x10 + iVar3 * 4) + 100),local_50,1);
          iVar3 = iVar3 + 1;
        } while (iVar3 < (int)this->mbr_0x1c);
      }
      dVar1 = this->mbr_0x1c;
      this->mbr_0x1c = 0;
      this->mbr_0x18 = this->mbr_0x18 + dVar1;
      this->mbr_0x8 = 0;
    }
    VirtualUnlock((LPVOID)this->mbr_0xc,this->mbr_0x20);
    FUN_00482f80((LPCVOID)this->mbr_0xc);
    iVar3 = 0;
    this->mbr_0xc = 0;
    if (0 < (int)this->mbr_0x14) {
      do {
        puVar2 = *(undefined4 **)(this->mbr_0x10 + iVar3 * 4);
        if (puVar2 != (undefined4 *)0x0) {
          (**(code **)*puVar2)(1);
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 < (int)this->mbr_0x14);
    }
    FUN_004830f0((LPCVOID)this->mbr_0x10);
    this->mbr_0x10 = 0;
  }
  return;
}



