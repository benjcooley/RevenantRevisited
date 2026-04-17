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
  uint uVar1;
  cls_0x495b70 *pcVar2;
  cls_0x495b70 *extraout_ECX;
  int iVar3;
  byte *in_stack_00000004;
  
  iVar3 = 0;
  pcVar2 = this;
  if (0 < (int)this->mbr_0x24) {
    do {
      uVar1 = FUN_0059a530((uint)pcVar2,(byte *)(*(int *)(this->mbr_0x34 + iVar3 * 4) + 8),
                           in_stack_00000004);
      if (uVar1 == 0) {
        return **(undefined4 **)(this->mbr_0x34 + iVar3 * 4);
      }
      iVar3 = iVar3 + 1;
      pcVar2 = extraout_ECX;
    } while (iVar3 < (int)this->mbr_0x24);
  }
  return 0xffffffff;
}



