// Decompiled methods and structure for class: cls_0x53ede0

/*
/OOAnalyzer/cls_0x53ede0
pack(disabled)
Structure cls_0x53ede0 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 24 Alignment: 1

*/

// Function at 0053ede0

undefined4 __thiscall OOAnalyzer::cls_0x53ede0::meth_0x53ede0(cls_0x53ede0 *this)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      iVar1 = FUN_0059a530(*(int *)(this->mbr_0x14 + iVar4 * 4) + 0x1c);
      if (iVar1 == 0) {
        return *(undefined4 *)(this->mbr_0x14 + iVar4 * 4);
      }
      iVar1 = 0;
      piVar2 = *(int **)(this->mbr_0x14 + iVar4 * 4);
      if (0 < *piVar2) {
        do {
          iVar3 = *(int *)(piVar2[4] + iVar1 * 4);
          if (iVar3 == 0) {
            iVar3 = piVar2[5];
          }
          iVar3 = FUN_0059a530(iVar3 + 4);
          if (iVar3 == 0) {
            return *(undefined4 *)(this->mbr_0x14 + iVar4 * 4);
          }
          iVar1 = iVar1 + 1;
          piVar2 = *(int **)(this->mbr_0x14 + iVar4 * 4);
        } while (iVar1 < *piVar2);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)this->mbr_0x4);
  }
  return 0;
}



