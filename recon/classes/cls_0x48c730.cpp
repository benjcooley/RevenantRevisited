// Decompiled methods and structure for class: cls_0x48c730

/*
/OOAnalyzer/cls_0x48c730
pack(disabled)
Structure cls_0x48c730 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 24 Alignment: 1

*/

// Function at 0048c730

undefined4 __thiscall OOAnalyzer::cls_0x48c730::meth_0x48c730(cls_0x48c730 *this)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      iVar1 = FUN_0059a530(*(undefined4 *)(this->mbr_0x14 + iVar2 * 4));
      if (iVar1 == 0) {
        return *(undefined4 *)(this->mbr_0x14 + iVar2 * 4);
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x4);
  }
  return 0;
}



