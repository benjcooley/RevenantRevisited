// FUN_004932a0 @ 004932a0 size=289

void __thiscall FUN_004932a0(int param_1,char *param_2,undefined4 param_3)

{
  int *piVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 *puVar4;
  
  if (((((DAT_0066829c == 0) || (DAT_0067682c == 0)) ||
       (iVar3 = *(int *)(param_1 + 0xc4), iVar3 == 0)) ||
      ((*(int *)(param_1 + 0xcc) == 0 || (*(short *)(iVar3 + 4) != 0xb)))) ||
     ((iVar3 == DAT_00667fcc ||
      (iVar3 = FUN_0059a530(*(int *)(param_1 + 0xcc),&DAT_005da13c), iVar3 != 0)))) {
    FUN_00535870(param_2,param_3);
    return;
  }
  piVar1 = *(int **)(param_1 + 0xb8);
  if ((piVar1 != (int *)0x0) && (*piVar1 != 1)) {
    FUN_004830f0(piVar1);
    *(undefined4 *)(param_1 + 0xb8) = 0;
  }
  puVar4 = *(undefined4 **)(param_1 + 0xb8);
  if (puVar4 == (undefined4 *)0x0) {
    puVar4 = (undefined4 *)FUN_00482fb0(0x108);
    *(undefined4 **)(param_1 + 0xb8) = puVar4;
    for (iVar3 = 0x42; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    puVar4 = *(undefined4 **)(param_1 + 0xb8);
    *puVar4 = 1;
  }
  iVar3 = puVar4[1];
  if (7 < iVar3) {
    return;
  }
  _strncpy((char *)(puVar4 + iVar3 * 8 + 2),param_2,0x1f);
  uVar2 = *(undefined4 *)(param_1 + 0xc4);
  *(char *)((int)(puVar4 + iVar3 * 8 + 2) + 0x1f) = '\0';
  puVar4[1] = puVar4[1] + 1;
  FUN_00586f80(uVar2,param_2,param_3);
  return;
}


