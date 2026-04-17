// Decompiled methods and structure for class: cls_0x56f930

/*
/OOAnalyzer/cls_0x56f930
pack(disabled)
Structure cls_0x56f930 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 0056f930

void __thiscall
OOAnalyzer::cls_0x56f930::meth_0x56f930(cls_0x56f930 *this,undefined2 *param_1,uint param_2)

{
  undefined2 uVar1;
  dword dVar2;
  undefined2 *puVar3;
  int iVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  undefined2 *puVar8;
  uint uVar9;
  undefined4 *puVar10;
  undefined2 *puVar11;
  undefined2 *puVar12;
  undefined2 *puVar13;
  undefined2 *in_stack_0000000c;
  
  puVar12 = (undefined2 *)this->mbr_0x8;
  if (param_2 <= (uint)((int)(this->mbr_0xc - (int)puVar12) >> 1)) {
    uVar9 = (int)puVar12 - (int)param_1 >> 1;
    if (uVar9 < param_2) {
      puVar13 = param_1 + param_2;
      if (param_1 != puVar12) {
        puVar8 = puVar13 + -param_2;
        do {
          if (puVar13 != (undefined2 *)0x0) {
            *puVar13 = *puVar8;
          }
          puVar8 = puVar8 + 1;
          puVar13 = puVar13 + 1;
        } while (puVar8 != puVar12);
      }
      puVar13 = puVar12;
      for (iVar4 = param_2 - uVar9; iVar4 != 0; iVar4 = iVar4 + -1) {
        if (puVar13 != (undefined2 *)0x0) {
          *puVar13 = *in_stack_0000000c;
        }
        puVar13 = puVar13 + 1;
      }
      if (param_1 != puVar12) {
        uVar1 = *in_stack_0000000c;
        do {
          *param_1 = uVar1;
          param_1 = param_1 + 1;
        } while (param_1 != puVar12);
      }
      this->mbr_0x8 = (dword)(puVar12 + param_2);
      return;
    }
    if (param_2 != 0) {
      puVar11 = puVar12 + -param_2;
      puVar13 = puVar12;
      for (puVar8 = puVar11; puVar3 = puVar12, puVar8 != puVar12; puVar8 = puVar8 + 1) {
        if (puVar13 != (undefined2 *)0x0) {
          *puVar13 = *puVar8;
        }
        puVar13 = puVar13 + 1;
      }
      for (; param_1 != puVar11; puVar11 = puVar11 + -1) {
        puVar3[-1] = puVar11[-1];
        puVar3 = puVar3 + -1;
      }
      puVar13 = param_1 + param_2;
      if (param_1 != puVar13) {
        uVar1 = *in_stack_0000000c;
        do {
          *param_1 = uVar1;
          param_1 = param_1 + 1;
        } while (param_1 != puVar13);
      }
      this->mbr_0x8 = (dword)(puVar12 + param_2);
    }
    return;
  }
  dVar2 = this->mbr_0x4;
  if ((dVar2 == 0) || (uVar9 = (int)((int)puVar12 - dVar2) >> 1, uVar9 <= param_2)) {
    uVar9 = param_2;
  }
  if (dVar2 == 0) {
    iVar4 = 0;
  }
  else {
    iVar4 = (int)((int)puVar12 - dVar2) >> 1;
  }
  iVar4 = iVar4 + uVar9;
  iVar5 = iVar4;
  if (iVar4 < 0) {
    iVar5 = 0;
  }
  puVar6 = FUN_00482fb0(iVar5 * 2);
  puVar10 = puVar6;
  for (puVar12 = (undefined2 *)this->mbr_0x4; puVar12 != param_1; puVar12 = puVar12 + 1) {
    if (puVar10 != (undefined4 *)0x0) {
      *(undefined2 *)puVar10 = *puVar12;
    }
    puVar10 = (undefined4 *)((int)puVar10 + 2);
  }
  puVar7 = puVar10;
  uVar9 = param_2;
  if (param_2 != 0) {
    do {
      if (puVar7 != (undefined4 *)0x0) {
        *(undefined2 *)puVar7 = *in_stack_0000000c;
      }
      uVar9 = uVar9 - 1;
      puVar7 = (undefined4 *)((int)puVar7 + 2);
    } while (uVar9 != 0);
  }
  puVar13 = (undefined2 *)this->mbr_0x8;
  puVar12 = (undefined2 *)(param_2 * 2 + (int)puVar10);
  if (param_1 != puVar13) {
    puVar8 = (undefined2 *)((int)puVar12 + (param_2 * -2 - (int)puVar10) + (int)param_1);
    do {
      if (puVar12 != (undefined2 *)0x0) {
        *puVar12 = *puVar8;
      }
      puVar8 = puVar8 + 1;
      puVar12 = puVar12 + 1;
    } while (puVar8 != puVar13);
  }
  FUN_004830f0((LPCVOID)this->mbr_0x4);
  this->mbr_0xc = (int)puVar6 + iVar4 * 2;
  dVar2 = this->mbr_0x4;
  if (dVar2 == 0) {
    this->mbr_0x4 = (dword)puVar6;
    this->mbr_0x8 = (int)puVar6 + param_2 * 2;
    return;
  }
  this->mbr_0x4 = (dword)puVar6;
  this->mbr_0x8 = (int)puVar6 + (((int)(this->mbr_0x8 - dVar2) >> 1) + param_2) * 2;
  return;
}



