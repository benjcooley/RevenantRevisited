// Decompiled methods and structure for class: cls_0x4959f0

/*
/OOAnalyzer/cls_0x4959f0
pack(disabled)
Structure cls_0x4959f0 {
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 56 Alignment: 1

*/

// Function at 004959f0

void __thiscall OOAnalyzer::cls_0x4959f0::meth_0x4959f0(cls_0x4959f0 *this,undefined4 param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  undefined4 in_stack_00000008;
  
  iVar3 = 0;
  if (0 < (int)this->mbr_0x24) {
    while (iVar2 = FUN_0059a530_stricmp(*(int *)(this->mbr_0x34 + iVar3 * 4) + 8,param_1), iVar2 != 0) {
      iVar3 = iVar3 + 1;
      if ((int)this->mbr_0x24 <= iVar3) {
        return;
      }
    }
    piVar1 = *(int **)(this->mbr_0x34 + iVar3 * 4);
    if (*piVar1 == 0) {
      *(undefined4 *)piVar1[1] = in_stack_00000008;
    }
  }
  return;
}



