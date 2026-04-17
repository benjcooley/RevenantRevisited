// Decompiled methods and structure for class: cls_0x495a50

/*
/OOAnalyzer/cls_0x495a50
pack(disabled)
Structure cls_0x495a50 {
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 56 Alignment: 1

*/

// Function at 00495a50

undefined4 __thiscall OOAnalyzer::cls_0x495a50::meth_0x495a50(cls_0x495a50 *this)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  if (0 < (int)this->mbr_0x24) {
    do {
      iVar2 = FUN_0059a530(*(int *)(this->mbr_0x34 + iVar3 * 4) + 8);
      if (iVar2 == 0) {
        piVar1 = *(int **)(this->mbr_0x34 + iVar3 * 4);
        if (*piVar1 != 0) {
          return 0xfeced300;
        }
        return *(undefined4 *)piVar1[1];
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)this->mbr_0x24);
  }
  return 0xfeced300;
}



