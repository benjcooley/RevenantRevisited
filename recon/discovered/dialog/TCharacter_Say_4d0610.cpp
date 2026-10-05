// FUN_004d0610 @ 004d0610 size=820

undefined4 __thiscall FUN_004d0610(int *param_1,int param_2,int param_3,int param_4,int param_5)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  char *pcVar6;
  undefined4 uStack_118;
  undefined4 uStack_114;
  undefined4 uStack_110;
  char acStack_10c [256];
  void *local_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e8d8;
  local_c = ExceptionList;
  if ((param_2 == 0) ||
     (ExceptionList = &local_c, iVar2 = (**(code **)(*param_1 + 0x1c0))(), iVar2 < 1)) {
    ExceptionList = local_c;
    return 0;
  }
  uVar5 = 0;
  param_1[0x98] = -1;
  if ((param_5 != 0) && (DAT_005d7a60 != 0)) {
    if ((DAT_0066829c == 0) || (iVar2 = (**(code **)(*param_1 + 0x24))(), iVar2 != 0)) {
      iVar2 = FUN_0049c430(param_5);
      param_1[0x98] = iVar2;
      if (iVar2 < 0) {
        uVar5 = 0;
      }
      else {
        iVar4 = FUN_0049b650(iVar2);
        if (iVar4 == 0) {
          uVar5 = 0;
        }
        else {
          iVar2 = FUN_0049b990(iVar2,0x7f,1,0,0x50,700);
          uVar5 = (uint)(iVar2 != 0);
        }
      }
    }
    else {
      iVar2 = FUN_0049c430(param_5);
      param_1[0x98] = iVar2;
      if (-1 < iVar2) {
        FUN_0049b650(iVar2);
        uStack_118 = DAT_00666988;
        uStack_114 = DAT_0066698c;
        uStack_110 = DAT_00666990;
        iVar2 = FUN_0046de60(&uStack_118,param_1 + 4);
        if ((iVar2 < 0x401) && (*(ushort *)((int)param_1 + 0xe) == DAT_00666970)) {
          uVar3 = __ftol();
          uVar5 = FUN_0049b990(param_1[0x98],uVar3,0,param_1 + 4,0x80,0x400);
        }
      }
    }
  }
  FUN_00533dd0(param_2,acStack_10c,0x100);
  if (param_4 == 0) {
    iVar2 = FUN_00482fb0(100);
    uStack_4 = 1;
    if (iVar2 != 0) {
      iVar2 = FUN_004da9f0(&DAT_005e02e4,0x10);
      goto LAB_004d0818;
    }
  }
  else {
    iVar2 = FUN_00482fb0(100);
    uStack_4 = 0;
    if (iVar2 != 0) {
      iVar2 = FUN_004da9f0(param_4,0x10);
      goto LAB_004d0818;
    }
  }
  iVar2 = 0;
LAB_004d0818:
  uStack_4 = 0xffffffff;
  if ((uVar5 == 0) || (DAT_00668188 != 0)) {
    uVar3 = FUN_0059b6bc(acStack_10c);
    *(undefined4 *)(iVar2 + 0x5c) = uVar3;
  }
  else {
    *(undefined4 *)(iVar2 + 0x5c) = 0;
  }
  if (param_3 < 0) {
    if ((param_5 == 0) || (param_1[0x98] == -1)) {
      uVar5 = 0xffffffff;
      pcVar6 = acStack_10c;
      do {
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      *(uint *)(iVar2 + 0x28) = ~uVar5 * 2 + 0x22;
    }
    else {
      iVar4 = FUN_0049c640(param_1[0x98]);
      if (iVar4 == 0) {
        uVar5 = 0xffffffff;
        pcVar6 = acStack_10c;
        do {
          if (uVar5 == 0) break;
          uVar5 = uVar5 - 1;
          cVar1 = *pcVar6;
          pcVar6 = pcVar6 + 1;
        } while (cVar1 != '\0');
        *(uint *)(iVar2 + 0x28) = ~uVar5 * 2 + 0x22;
      }
      else {
        FUN_0049c640(param_1[0x98]);
        iVar4 = __ftol();
        *(int *)(iVar2 + 0x28) = 0xc - iVar4;
      }
    }
  }
  else {
    *(int *)(iVar2 + 0x28) = param_3;
  }
  iVar4 = *param_1;
  *(uint *)(iVar2 + 0x60) = *(uint *)(iVar2 + 0x60) | 0x800;
  (**(code **)(iVar4 + 0x218))(iVar2,0,0);
  if ((DAT_0066829c == 0) || (iVar4 = (**(code **)(*param_1 + 0x24))(), iVar4 != 0)) {
    FUN_00535b90(param_1,acStack_10c,*(undefined4 *)(iVar2 + 0x28));
  }
  ExceptionList = local_c;
  return 1;
}


