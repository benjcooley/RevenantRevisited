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
  uint uVar2;
  cls_0x495a50 *pcVar3;
  cls_0x495a50 *extraout_ECX;
  int iVar4;
  byte *in_stack_00000004;
  
  iVar4 = 0;
  pcVar3 = this;
  if (0 < (int)this->mbr_0x24) {
    do {
      uVar2 = FUN_0059a530_stricmp((uint)pcVar3,(byte *)(*(int *)(this->mbr_0x34 + iVar4 * 4) + 8),
                           in_stack_00000004);
      if (uVar2 == 0) {
        piVar1 = *(int **)(this->mbr_0x34 + iVar4 * 4);
        if (*piVar1 != 0) {
          return 0xfeced300;
        }
        return *(undefined4 *)piVar1[1];
      }
      iVar4 = iVar4 + 1;
      pcVar3 = extraout_ECX;
    } while (iVar4 < (int)this->mbr_0x24);
  }
  return 0xfeced300;
}



