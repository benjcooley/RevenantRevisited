// Decompiled methods and structure for class: cls_0x495b70

/*
/OOAnalyzer/cls_0x495b70
pack(disabled)
Structure cls_0x495b70 {
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 56 Alignment: 1

*/

// Function at 00495b70

undefined4 __thiscall OOAnalyzer::cls_0x495b70::meth_0x495b70(cls_0x495b70 *this)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)this->mbr_0x24) {
    do {
      iVar1 = FUN_0059a530(*(int *)(this->mbr_0x34 + iVar2 * 4) + 8);
      if (iVar1 == 0) {
        return **(undefined4 **)(this->mbr_0x34 + iVar2 * 4);
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x24);
  }
  return 0xffffffff;
}



