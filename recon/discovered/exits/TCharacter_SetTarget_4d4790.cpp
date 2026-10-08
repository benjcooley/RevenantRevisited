// TCharacter_SetTarget @ 0x004d4790 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// called with 0 by Activate/pos/level: clears the combat target (inferred name)
// FUN_004d4790 @ 004d4790 size=375

undefined4 __thiscall FUN_004d4790(int *param_1,int *param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  
  if (((DAT_0066829c != 0) && (iVar3 = (**(code **)(*param_1 + 0x178))(), iVar3 < 1)) &&
     (DAT_00676e5c == '\0')) {
    return 0;
  }
  iVar3 = (**(code **)(*param_1 + 0x1c0))();
  if (0 < iVar3) {
    if ((param_1[0x44] & 0x80000U) == 0) {
      iVar3 = *(int *)(param_1[0x36] + 0x48);
      if ((iVar3 != 0) && ((*(uint *)(iVar3 + 0x24) & 0x2000000) != 0)) {
        return 0;
      }
      iVar3 = *(int *)(param_1[0x36] + 0x4c);
      if ((iVar3 != 0) && ((*(byte *)(iVar3 + 0x24) & 0x80) != 0)) {
        return 0;
      }
    }
    if (param_2 != param_1) {
      if ((param_2 != (int *)0x0) && (iVar3 = (**(code **)(*param_2 + 0x1c0))(), iVar3 < 1)) {
        return 0;
      }
      piVar1 = (int *)param_1[0x38];
      if (((piVar1 == (int *)0x0) ||
          ((*piVar1 != 3 && ((piVar1 == (int *)0x0 || (*piVar1 != 0x19)))))) &&
         (param_2 != (int *)0x0)) {
        uVar4 = FUN_004d3b90(param_2,3);
        return uVar4;
      }
      if (*(int **)(param_1[0x36] + 0x44) != param_2) {
        if (param_2 == (int *)0x0) {
          iVar3 = 0;
        }
        else {
          iVar3 = param_2[0x10];
        }
        FUN_00583f60(param_1,0x21,iVar3,0,0);
        iVar3 = param_1[0x37];
        iVar2 = param_1[0x38];
        *(int **)(param_1[0x36] + 0x44) = param_2;
        *(int **)(iVar3 + 0x44) = param_2;
        *(int **)(iVar2 + 0x44) = param_2;
        if ((param_2 != (int *)0x0) && (uVar4 = FUN_0046ea90(param_2), param_1[0x95] == 0)) {
          iVar3 = param_1[0x37];
          *(undefined4 *)(param_1[0x36] + 0x2c) = uVar4;
          iVar2 = param_1[0x38];
          *(undefined4 *)(iVar3 + 0x2c) = uVar4;
          *(undefined4 *)(iVar2 + 0x2c) = uVar4;
        }
        if ((short)param_1[1] == 0xb) {
          param_1[0xa3] = -1;
          param_1[0xa4] = 0;
        }
        return 1;
      }
      return 1;
    }
  }
  return 0;
}


