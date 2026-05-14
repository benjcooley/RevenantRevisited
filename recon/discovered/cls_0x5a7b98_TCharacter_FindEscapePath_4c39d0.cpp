// FUN_004c39d0_FindClearPath @ 004c39d0 size=490

uint __thiscall
FUN_004c39d0_FindClearPath(int *param_1,int param_2,undefined4 param_3,byte param_4,int *param_5,
            undefined4 *param_6)

{
  undefined2 uVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  int *piVar10;
  uint *puVar11;
  uint local_14;
  undefined4 local_10;
  int iStack_c;
  int iStack_8;
  int iStack_4;
  
  local_14 = 0;
  piVar5 = param_5;
  if (param_5 == (int *)0x0) {
    piVar5 = (int *)&param_6;
  }
  puVar8 = param_6;
  if (param_6 == (undefined4 *)0x0) {
    puVar8 = &local_10;
  }
  *puVar8 = 0;
  *piVar5 = *(int *)(param_2 + 8);
  if ((param_4 & 8) == 0) {
    puVar11 = &local_14;
    puVar9 = &param_5;
    uVar1 = *(undefined2 *)((int)param_1 + 0xe);
    piVar10 = piVar5;
    uVar4 = (**(code **)(*param_1 + 600))(puVar9,piVar5,puVar11);
    FUN_004530a0_GetWalkHeightRadius(param_3,uVar1,uVar4,puVar9,piVar10,puVar11);
    if (local_14 != 0) {
      return 1;
    }
  }
  else {
    iVar2 = FUN_00452e10_GetWalkHeight(param_3,*(undefined2 *)((int)param_1 + 0xe),0);
    *piVar5 = iVar2;
    param_5 = (int *)0x0;
  }
  if ((int *)param_1[0x15] != (int *)0x0) {
    (**(code **)(*(int *)param_1[0x15] + 0x8c))((short)param_1[3]);
  }
  if (puVar8 == (undefined4 *)0x0) {
    puVar8 = (undefined4 *)&param_4;
  }
  iVar2 = *piVar5;
  *puVar8 = 0;
  iVar7 = *(int *)(param_2 + 8) - iVar2;
  if (iVar7 < 0) {
    iVar7 = iVar2 - *(int *)(param_2 + 8);
  }
  if (((iVar7 < 0x21) && ((int)param_5 < 0x21)) && (iVar2 != 0)) {
    local_14 = 0;
    iVar2 = (**(code **)(*param_1 + 0x1c0))();
    if ((0 < iVar2) &&
       ((*(int *)(param_1[0x36] + 0x48) == 0 ||
        ((*(uint *)(*(int *)(param_1[0x36] + 0x48) + 0x24) & 0x2000000) == 0)))) {
      if ((int *)param_1[0x15] == (int *)0x0) {
        uVar3 = 0;
      }
      else {
        uVar3 = (**(code **)(*(int *)param_1[0x15] + 0x8c))((short)param_1[3]);
      }
      if ((((uVar3 & 0x800) == 0) && ((*(byte *)(param_1 + 2) & 0x80) == 0)) &&
         ((param_1[0x3b] == 0 &&
          (((DAT_0066829c == 0 || ((short)param_1[1] != 0xb)) || (param_1 == DAT_00667fcc)))))) {
        uVar1 = *(undefined2 *)((int)param_1 + 0xe);
        uVar4 = (**(code **)(*param_1 + 600))();
        piVar5 = (int *)FUN_004d4db0_FindCharInLine(param_1,param_3,uVar1,uVar4);
        *puVar8 = piVar5;
        if (piVar5 != (int *)0x0) {
          iStack_c = piVar5[4];
          iStack_8 = piVar5[5];
          iStack_4 = piVar5[6];
          iVar2 = (**(code **)(*piVar5 + 600))();
          iVar7 = (**(code **)(*param_1 + 600))();
          iVar6 = FUN_0046de60_Distance2D(param_2,&iStack_c);
          return (uint)(iVar2 + iVar7 <= iVar6);
        }
      }
    }
    return local_14;
  }
  return 1;
}


