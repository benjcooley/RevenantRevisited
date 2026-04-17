// Decompiled methods and structure for class: cls_0x495ab0

/*
/OOAnalyzer/cls_0x495ab0
pack(disabled)
Structure cls_0x495ab0 {
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 56 Alignment: 1

*/

// Function at 00495ab0

void __thiscall OOAnalyzer::cls_0x495ab0::meth_0x495ab0(cls_0x495ab0 *this,byte *param_1)

{
  char cVar1;
  int *piVar2;
  uint uVar3;
  cls_0x495ab0 *pcVar4;
  cls_0x495ab0 *extraout_ECX;
  uint uVar5;
  int iVar6;
  undefined4 *puVar7;
  char *pcVar8;
  undefined4 *puVar9;
  char *in_stack_00000008;
  
  iVar6 = 0;
  pcVar4 = this;
  if (0 < (int)this->mbr_0x24) {
    while (uVar3 = FUN_0059a530((uint)pcVar4,(byte *)(*(int *)(this->mbr_0x34 + iVar6 * 4) + 8),
                                param_1), uVar3 != 0) {
      iVar6 = iVar6 + 1;
      pcVar4 = extraout_ECX;
      if ((int)this->mbr_0x24 <= iVar6) {
        return;
      }
    }
    piVar2 = *(int **)(this->mbr_0x34 + iVar6 * 4);
    if (*piVar2 == 1) {
      uVar3 = 0xffffffff;
      do {
        pcVar8 = in_stack_00000008;
        if (uVar3 == 0) break;
        uVar3 = uVar3 - 1;
        pcVar8 = in_stack_00000008 + 1;
        cVar1 = *in_stack_00000008;
        in_stack_00000008 = pcVar8;
      } while (cVar1 != '\0');
      uVar3 = ~uVar3;
      puVar7 = (undefined4 *)(pcVar8 + -uVar3);
      puVar9 = (undefined4 *)piVar2[1];
      for (uVar5 = uVar3 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
        *puVar9 = *puVar7;
        puVar7 = puVar7 + 1;
        puVar9 = puVar9 + 1;
      }
      for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
        *(undefined *)puVar9 = *(undefined *)puVar7;
        puVar7 = (undefined4 *)((int)puVar7 + 1);
        puVar9 = (undefined4 *)((int)puVar9 + 1);
      }
    }
  }
  return;
}



