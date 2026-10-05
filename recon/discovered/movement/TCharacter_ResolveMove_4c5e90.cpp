// TCharacter_ResolveMove @ 0x004c5e90 (vtable slot 0x31c) -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/COMMAND_SYSTEM.md §6.5)
// Goto arrival: a target and |dx|+|dy|-min(|dx|,|dy|)/2 < 8 -> MoveTo(target), nowaitdone (0x40),
// pick up the item if +0x288; otherwise angle = moveangle = AngleTo(target), then step on.
// FUN_004c5e90 @ 004c5e90 size=1050

undefined4 __thiscall FUN_004c5e90(int *param_1,int param_2,byte param_3)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  char *pcVar6;
  char *pcVar7;
  int iVar8;
  int iVar9;
  char *pcVar10;
  undefined *puVar11;
  undefined4 uVar12;
  void *pvStack_c;
  undefined1 *puStack_8;
  int iStack_4;
  
  iStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e38b;
  pvStack_c = ExceptionList;
  if ((*(uint *)(param_2 + 0x60) & 0x200) != 0) {
    ExceptionList = &pvStack_c;
    param_1[0x2d] = 0;
    iVar2 = FUN_0059a530(param_2 + 4,param_1[0x38] + 4);
    if (iVar2 == 0) {
      uVar4 = *(uint *)(param_2 + 0x2c);
      if (*(byte *)((int)param_1 + 0x36) != uVar4) {
        FUN_004c5ad0(uVar4,uVar4,*(undefined4 *)(param_2 + 0x34));
        ExceptionList = pvStack_c;
        return 0;
      }
    }
    else if (param_1[0x20] == 0) {
      ExceptionList = pvStack_c;
      return 0;
    }
    iVar2 = *param_1;
    uVar3 = FUN_004dadd0(param_1[0x38] + 4,&DAT_005df870,0);
    iVar2 = (**(code **)(iVar2 + 0x1f0))(uVar3);
    if (iVar2 == 0) {
      iVar2 = *param_1;
      uVar3 = FUN_004dadd0(param_1[0x38] + 4,&DAT_005df878,0);
      iVar2 = (**(code **)(iVar2 + 0x1f0))(uVar3);
      if (iVar2 == 0) {
        iVar2 = *param_1;
        uVar3 = FUN_004dadd0(param_1[0x38] + 4,&DAT_005df880,0);
        iVar2 = (**(code **)(iVar2 + 0x1f0))(uVar3);
        if (iVar2 == 0) {
          (**(code **)(*param_1 + 0x208))(param_1[0x38],0);
          ExceptionList = pvStack_c;
          return 0;
        }
        iStack_4 = FUN_00482fb0(100);
        pvStack_c = (void *)0x2;
        if (iStack_4 == 0) goto LAB_004c600e;
        puVar11 = &DAT_005df884;
      }
      else {
        iStack_4 = FUN_00482fb0(100);
        pvStack_c = (void *)0x1;
        if (iStack_4 == 0) {
LAB_004c600e:
          iVar2 = 0;
          goto LAB_004c6010;
        }
        puVar11 = &DAT_005df87c;
      }
    }
    else {
      iStack_4 = FUN_00482fb0(100);
      pvStack_c = (void *)0x0;
      if (iStack_4 == 0) goto LAB_004c600e;
      puVar11 = &DAT_005df874;
    }
    uVar12 = 0;
    uVar3 = FUN_004dadd0(param_1[0x38] + 4,puVar11,0);
    iVar2 = FUN_004daae0(param_1[0x36],uVar3,uVar12);
LAB_004c6010:
    uVar3 = *(undefined4 *)(param_2 + 0x2c);
    iVar9 = *param_1;
    *(uint *)(iVar2 + 0x60) = *(uint *)(iVar2 + 0x60) & 0xfffffdff;
    pvStack_c = (void *)0xffffffff;
    *(undefined4 *)(iVar2 + 0x34) = 8;
    *(undefined4 *)(iVar2 + 0x30) = uVar3;
    *(undefined4 *)(iVar2 + 0x2c) = uVar3;
    (**(code **)(iVar9 + 0x218))(iVar2,0,0);
    ExceptionList = pvStack_c;
    return 0;
  }
  if ((param_3 & 2) == 0) {
    iVar2 = *(int *)(param_2 + 0x38);
    if (((iVar2 != 0) || (*(int *)(param_2 + 0x3c) != 0)) ||
       (ExceptionList = &pvStack_c, *(int *)(param_2 + 0x40) != 0)) {
      iVar9 = param_1[4] - iVar2;
      if (iVar9 < 0) {
        iVar9 = iVar2 - param_1[4];
      }
      iVar2 = param_1[5] - *(int *)(param_2 + 0x3c);
      if (iVar2 < 0) {
        iVar2 = *(int *)(param_2 + 0x3c) - param_1[5];
      }
      iVar8 = iVar9;
      if (iVar2 <= iVar9) {
        iVar8 = iVar2;
      }
      if ((iVar9 - (iVar8 >> 1)) + iVar2 < 8) {
        iVar2 = *param_1;
        ExceptionList = &pvStack_c;
        *(int *)(param_2 + 0x40) = param_1[6];
        (**(code **)(iVar2 + 0xc))(param_2 + 0x38);
        iVar2 = param_1[0xa2];
        *(uint *)(param_2 + 0x60) = *(uint *)(param_2 + 0x60) | 0x40;
        if (iVar2 != 0) {
          FUN_004cfef0(iVar2,0);
          param_1[0xa2] = 0;
        }
        ExceptionList = pvStack_c;
        return 0;
      }
      ExceptionList = &pvStack_c;
      uVar3 = FUN_0046dc60(param_1 + 4,param_2 + 0x38);
      *(undefined4 *)(param_2 + 0x30) = uVar3;
      *(undefined4 *)(param_2 + 0x2c) = uVar3;
    }
    uVar3 = *(undefined4 *)(param_2 + 0x34);
    param_1[0x2c] = (uint)*(byte *)((int)param_1 + 0x36);
    FUN_004c5ad0(*(undefined4 *)(param_2 + 0x2c),*(undefined4 *)(param_2 + 0x2c),uVar3);
    if (param_1[0x20] == 0) {
      ExceptionList = pvStack_c;
      return 2;
    }
    if ((*(uint *)(param_2 + 0x60) & 0x100) != 0) {
      ExceptionList = pvStack_c;
      return 2;
    }
    iVar2 = FUN_00482fb0(100);
    iStack_4 = 3;
    if (iVar2 == 0) {
      pcVar6 = (char *)0x0;
    }
    else {
      pcVar6 = (char *)FUN_004daae0(param_1[0x36],0,0);
    }
    iStack_4 = 0xffffffff;
    iVar2 = FUN_004dac80(param_1[0x38] + 4);
    if (iVar2 == 0) {
      iVar2 = FUN_004dac30(param_1[0x38] + 4);
      if (iVar2 == 0) goto LAB_004c6281;
      iVar2 = param_1[0x38];
      puVar11 = &DAT_005df88c;
    }
    else {
      iVar2 = param_1[0x38];
      puVar11 = &DAT_005df888;
    }
    pcVar7 = (char *)FUN_004dadd0(iVar2 + 4,puVar11);
    uVar4 = 0xffffffff;
    do {
      pcVar10 = pcVar7;
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      pcVar10 = pcVar7 + 1;
      cVar1 = *pcVar7;
      pcVar7 = pcVar10;
    } while (cVar1 != '\0');
    uVar4 = ~uVar4;
    pcVar10 = pcVar10 + -uVar4;
    pcVar7 = pcVar6;
    for (uVar5 = uVar4 >> 2; pcVar7 = pcVar7 + 4, uVar5 != 0; uVar5 = uVar5 - 1) {
      *(undefined4 *)pcVar7 = *(undefined4 *)pcVar10;
      pcVar10 = pcVar10 + 4;
    }
    for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *pcVar7 = *pcVar10;
      pcVar10 = pcVar10 + 1;
      pcVar7 = pcVar7 + 1;
    }
LAB_004c6281:
    (**(code **)(*param_1 + 0x218))(pcVar6,0,0);
    ExceptionList = pvStack_c;
    return 2;
  }
  iVar2 = *(int *)(param_2 + 0x2c);
  if (iVar2 < 0x7f) {
    if (0x5f < iVar2) goto LAB_004c60ef;
    if ((0x1f < iVar2) && (ExceptionList = &pvStack_c, iVar2 < 0x41)) goto LAB_004c6086;
LAB_004c6080:
    iVar2 = iVar2 + 0x20;
  }
  else {
    if (iVar2 < 0xa1) goto LAB_004c6080;
    if ((iVar2 < 0xe1) && (ExceptionList = &pvStack_c, 0xbe < iVar2)) goto LAB_004c6086;
LAB_004c60ef:
    iVar2 = iVar2 + -0x20;
  }
  ExceptionList = &pvStack_c;
  *(int *)(param_2 + 0x2c) = iVar2;
LAB_004c6086:
  iVar2 = *param_1;
  uVar4 = *(int *)(param_2 + 0x2c) + 0x10;
  uVar5 = uVar4 & 0xff;
  *(uint *)(param_2 + 0x2c) = uVar5;
  *(char *)((int)param_1 + 0x36) = (char)uVar4;
  param_1[0x2c] = uVar5;
  (**(code **)(iVar2 + 0x218))(param_1[0x38],0,0);
  iVar2 = FUN_0057d9d0(param_1,0x1d,1,0,0);
  if (iVar2 != 0) {
    FUN_0057dc70();
  }
  ExceptionList = pvStack_c;
  return 0;
}


