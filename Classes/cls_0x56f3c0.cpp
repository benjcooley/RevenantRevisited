// Decompiled methods and structure for class: cls_0x56f3c0

/*
/OOAnalyzer/cls_0x56f3c0
pack(disabled)
Structure cls_0x56f3c0 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 0056f3c0

dword __thiscall OOAnalyzer::cls_0x56f3c0::meth_0x56f3c0(cls_0x56f3c0 *this)

{
  dword dVar1;
  
  dVar1 = this->mbr_0x4;
  if (dVar1 == 0) {
    return dVar1;
  }
  return (int)(this->mbr_0x8 - dVar1) / 0x24;
}



// Function at 0056f3f0

cls_0x56f3c0 * __thiscall
OOAnalyzer::cls_0x56f3c0::meth_0x56f3f0(cls_0x56f3c0 *this,cls_0x56f3c0 *param_1,uint param_2)

{
  cls_0x56f3c0 *pcVar1;
  dword dVar2;
  dword dVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  cls_0x56f3c0 *pcVar8;
  cls_0x56f3c0 *pcVar9;
  int iVar10;
  uint uVar11;
  cls_0x56f3c0 *pcVar12;
  undefined4 *puVar13;
  cls_0x56f3c0 *pcVar14;
  dword *pdVar15;
  undefined4 *puVar16;
  cls_0x56f3c0 *pcVar17;
  dword *pdVar18;
  undefined4 *in_stack_0000000c;
  
  pcVar1 = (cls_0x56f3c0 *)this->mbr_0x8;
  if (param_2 <= (uint)((int)(this->mbr_0xc - (int)pcVar1) / 0x24)) {
    uVar11 = ((int)pcVar1 - (int)param_1) / 0x24;
    if (uVar11 < param_2) {
      puVar7 = (undefined4 *)(param_2 * 0x24 + (int)param_1);
      if (param_1 != pcVar1) {
        puVar5 = puVar7 + param_2 * -9;
        do {
          if (puVar7 != (undefined4 *)0x0) {
            puVar6 = puVar5;
            puVar13 = puVar7;
            for (iVar10 = 9; iVar10 != 0; iVar10 = iVar10 + -1) {
              *puVar13 = *puVar6;
              puVar6 = puVar6 + 1;
              puVar13 = puVar13 + 1;
            }
          }
          puVar5 = puVar5 + 9;
          puVar7 = puVar7 + 9;
        } while ((cls_0x56f3c0 *)puVar5 != pcVar1);
      }
      pcVar9 = pcVar1;
      for (iVar10 = param_2 - uVar11; iVar10 != 0; iVar10 = iVar10 + -1) {
        if (pcVar9 != (cls_0x56f3c0 *)0x0) {
          puVar7 = in_stack_0000000c;
          pcVar8 = pcVar9;
          for (iVar4 = 9; iVar4 != 0; iVar4 = iVar4 + -1) {
            *(undefined4 *)pcVar8 = *puVar7;
            puVar7 = puVar7 + 1;
            pcVar8 = (cls_0x56f3c0 *)&pcVar8->mbr_0x4;
          }
        }
        pcVar9 = (cls_0x56f3c0 *)&pcVar9[2].mbr_0x4;
      }
      if (param_1 != pcVar1) {
        do {
          pcVar9 = param_1 + 2;
          puVar7 = in_stack_0000000c;
          for (iVar10 = 9; iVar10 != 0; iVar10 = iVar10 + -1) {
            *(undefined4 *)param_1 = *puVar7;
            puVar7 = puVar7 + 1;
            param_1 = (cls_0x56f3c0 *)&param_1->mbr_0x4;
          }
          param_1 = (cls_0x56f3c0 *)&pcVar9->mbr_0x4;
        } while ((cls_0x56f3c0 *)&pcVar9->mbr_0x4 != pcVar1);
      }
      this->mbr_0x8 = (dword)(param_2 * 0x24 + (int)pcVar1);
      return this;
    }
    pcVar9 = (cls_0x56f3c0 *)(((int)pcVar1 - (int)param_1) * 0x38e38e39);
    if (param_2 != 0) {
      pcVar12 = (cls_0x56f3c0 *)((int)pcVar1 + param_2 * -0x24);
      pcVar9 = pcVar1;
      for (pcVar8 = pcVar12; pcVar14 = pcVar1, pcVar8 != pcVar1;
          pcVar8 = (cls_0x56f3c0 *)&pcVar8[2].mbr_0x4) {
        if (pcVar9 != (cls_0x56f3c0 *)0x0) {
          pcVar14 = pcVar8;
          pcVar17 = pcVar9;
          for (iVar10 = 9; iVar10 != 0; iVar10 = iVar10 + -1) {
            *(undefined4 *)pcVar17 = *(undefined4 *)pcVar14;
            pcVar14 = (cls_0x56f3c0 *)&pcVar14->mbr_0x4;
            pcVar17 = (cls_0x56f3c0 *)&pcVar17->mbr_0x4;
          }
        }
        pcVar9 = (cls_0x56f3c0 *)&pcVar9[2].mbr_0x4;
      }
      while (param_1 != pcVar12) {
        pcVar12 = (cls_0x56f3c0 *)&pcVar12[-3].mbr_0xc;
        pcVar14 = (cls_0x56f3c0 *)&pcVar14[-3].mbr_0xc;
        pdVar15 = (dword *)pcVar12;
        pdVar18 = (dword *)pcVar14;
        for (iVar10 = 9; iVar10 != 0; iVar10 = iVar10 + -1) {
          *pdVar18 = *pdVar15;
          pdVar15 = pdVar15 + 1;
          pdVar18 = pdVar18 + 1;
        }
      }
      pcVar9 = (cls_0x56f3c0 *)(param_2 * 0x24 + (int)param_1);
      pcVar8 = param_1;
      if (param_1 != pcVar9) {
        do {
          param_1 = (cls_0x56f3c0 *)&pcVar8[2].mbr_0x4;
          puVar7 = in_stack_0000000c;
          for (iVar10 = 9; iVar10 != 0; iVar10 = iVar10 + -1) {
            *(undefined4 *)pcVar8 = *puVar7;
            puVar7 = puVar7 + 1;
            pcVar8 = (cls_0x56f3c0 *)&pcVar8->mbr_0x4;
          }
          pcVar8 = param_1;
        } while (param_1 != pcVar9);
      }
      this->mbr_0x8 = (dword)(param_2 * 0x24 + (int)pcVar1);
      pcVar9 = param_1;
    }
    return pcVar9;
  }
  dVar2 = this->mbr_0x4;
  if ((dVar2 == 0) || (uVar11 = (int)((int)pcVar1 - dVar2) / 0x24, uVar11 <= param_2)) {
    uVar11 = param_2;
  }
  if (dVar2 == 0) {
    iVar10 = 0;
  }
  else {
    iVar10 = (int)((int)pcVar1 - dVar2) / 0x24;
  }
  iVar10 = iVar10 + uVar11;
  iVar4 = iVar10;
  if (iVar10 < 0) {
    iVar4 = 0;
  }
  puVar5 = (undefined4 *)FUN_00482fb0(iVar4 * 0x24);
  pcVar1 = (cls_0x56f3c0 *)this->mbr_0x4;
  puVar7 = puVar5;
  for (pcVar9 = pcVar1; pcVar9 != param_1; pcVar9 = (cls_0x56f3c0 *)&pcVar9[2].mbr_0x4) {
    if (puVar7 != (undefined4 *)0x0) {
      pcVar8 = pcVar9;
      puVar6 = puVar7;
      for (iVar4 = 9; iVar4 != 0; iVar4 = iVar4 + -1) {
        *puVar6 = *(undefined4 *)pcVar8;
        pcVar8 = (cls_0x56f3c0 *)&pcVar8->mbr_0x4;
        puVar6 = puVar6 + 1;
      }
    }
    puVar7 = puVar7 + 9;
  }
  puVar6 = puVar7;
  uVar11 = param_2;
  if (param_2 != 0) {
    do {
      if (puVar6 != (undefined4 *)0x0) {
        puVar13 = in_stack_0000000c;
        puVar16 = puVar6;
        for (iVar4 = 9; iVar4 != 0; iVar4 = iVar4 + -1) {
          *puVar16 = *puVar13;
          puVar13 = puVar13 + 1;
          puVar16 = puVar16 + 1;
        }
      }
      uVar11 = uVar11 - 1;
      puVar6 = puVar6 + 9;
    } while (uVar11 != 0);
  }
  pcVar9 = (cls_0x56f3c0 *)this->mbr_0x8;
  puVar6 = puVar7 + param_2 * 9;
  if (param_1 != pcVar9) {
    puVar7 = (undefined4 *)((int)puVar6 + (param_2 * -0x24 - (int)puVar7) + (int)param_1);
    do {
      if (puVar6 != (undefined4 *)0x0) {
        puVar13 = puVar7;
        puVar16 = puVar6;
        for (iVar4 = 9; iVar4 != 0; iVar4 = iVar4 + -1) {
          *puVar16 = *puVar13;
          puVar13 = puVar13 + 1;
          puVar16 = puVar16 + 1;
        }
      }
      puVar7 = puVar7 + 9;
      puVar6 = puVar6 + 9;
    } while ((cls_0x56f3c0 *)puVar7 != pcVar9);
  }
  FUN_004830f0(pcVar1);
  this->mbr_0xc = (dword)(puVar5 + iVar10 * 9);
  dVar2 = this->mbr_0x4;
  if (dVar2 == 0) {
    this->mbr_0x4 = (dword)puVar5;
    this->mbr_0x8 = (dword)(cls_0x56f3c0 *)(puVar5 + param_2 * 9);
    return (cls_0x56f3c0 *)(puVar5 + param_2 * 9);
  }
  dVar3 = this->mbr_0x8;
  this->mbr_0x4 = (dword)puVar5;
  this->mbr_0x8 = (dword)(cls_0x56f3c0 *)(puVar5 + (param_2 + (int)(dVar3 - dVar2) / 0x24) * 9);
  return (cls_0x56f3c0 *)(puVar5 + (param_2 + (int)(dVar3 - dVar2) / 0x24) * 9);
}



// Function at 0056f6e0

undefined4 * __thiscall
OOAnalyzer::cls_0x56f3c0::meth_0x56f6e0(cls_0x56f3c0 *this,undefined4 *param_1)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 *in_stack_00000008;
  
  puVar1 = (undefined4 *)this->mbr_0x8;
  puVar5 = param_1;
  puVar4 = param_1;
  if (in_stack_00000008 != puVar1) {
    do {
      puVar3 = in_stack_00000008 + 9;
      puVar5 = puVar4 + 9;
      for (iVar2 = 9; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar4 = *in_stack_00000008;
        in_stack_00000008 = in_stack_00000008 + 1;
        puVar4 = puVar4 + 1;
      }
      in_stack_00000008 = puVar3;
      puVar4 = puVar5;
    } while (puVar3 != puVar1);
  }
  this->mbr_0x8 = (dword)puVar5;
  return param_1;
}



