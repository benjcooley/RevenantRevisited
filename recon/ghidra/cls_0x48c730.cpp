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
  uint uVar1;
  cls_0x48c730 *pcVar2;
  cls_0x48c730 *extraout_ECX;
  int iVar3;
  byte *in_stack_00000004;
  
  iVar3 = 0;
  pcVar2 = this;
  if (0 < (int)this->mbr_0x4) {
    do {
      uVar1 = FUN_0059a530_stricmp((uint)pcVar2,*(byte **)(this->mbr_0x14 + iVar3 * 4),in_stack_00000004);
      if (uVar1 == 0) {
        return *(undefined4 *)(this->mbr_0x14 + iVar3 * 4);
      }
      iVar3 = iVar3 + 1;
      pcVar2 = extraout_ECX;
    } while (iVar3 < (int)this->mbr_0x4);
  }
  return 0;
}



