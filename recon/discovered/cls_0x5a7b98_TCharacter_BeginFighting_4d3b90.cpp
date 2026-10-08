// FUN_004d3b90 @ 004d3b90 size=1082

undefined4 __thiscall FUN_004d3b90(int *param_1,int *param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  int *piVar7;
  undefined4 unaff_retaddr;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e995;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  if (((param_1[0x38] != 0) &&
      ((((ExceptionList = &local_c, iVar2 = FUN_0045f770(3), iVar2 != 0 &&
         ((iVar2 = FUN_004dab80(s_combatrun_005e0618), iVar2 != 0 ||
          (iVar2 = FUN_004dab80(s_handrun_005e0610), iVar2 != 0)))) ||
        (((int *)param_1[0x38] != (int *)0x0 &&
         ((*(int *)param_1[0x38] == 0x19 && (iVar2 = FUN_004dab80(s_bowrun_005e0608), iVar2 != 0))))
        )) || (iVar2 = FUN_004dab80(&DAT_005e0604), iVar2 != 0)))) ||
     ((((param_1[0x38] != 0 && (iVar2 = FUN_004dab80(s_sneak_005c618c), iVar2 != 0)) ||
       ((param_1 != (int *)0x0 &&
        (((int *)param_1[0x36] != (int *)0x0 && (*(int *)param_1[0x36] == 0xb)))))) ||
      ((DAT_0066829c != 0 &&
       ((iVar2 = (**(code **)(*param_1 + 0x178))(), iVar2 < 1 && (DAT_00676e5c == '\0')))))))) {
    ExceptionList = local_c;
    return 0;
  }
  piVar7 = param_2;
  piVar3 = param_1 + 0x72;
  iVar2 = 8;
  do {
    *piVar3 = 1;
    piVar3 = piVar3 + 3;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  if ((param_2 != (int *)0x0) &&
     ((iVar2 = (**(code **)(*param_2 + 0x1c0))(), iVar2 < 1 || (piVar7[0x69] != 0)))) {
    piVar7 = (int *)0x0;
  }
  iVar2 = param_3;
  if (((param_3 == 0x19) && ((short)param_1[1] == 0xb)) && (param_1[0xaf] == 0)) {
    ExceptionList = local_c;
    return 0;
  }
  if (piVar7 == (int *)0x0) {
    iVar4 = FUN_004cd690(&param_2,1,0xffffffff,*(undefined1 *)((int)param_1 + 0x36),0x20,7);
    piVar7 = (int *)((iVar4 < 1) - 1 & (uint)param_2);
  }
  if (DAT_0066829c == 0) {
    FUN_0054d390();
  }
  if (*(int *)param_1[0x38] == iVar2) {
    uVar5 = FUN_004d4790(piVar7);
    ExceptionList = local_c;
    return uVar5;
  }
  FUN_005840b0(param_1,piVar7,iVar2);
  if ((param_1[0x44] & 0x4000U) == 0) {
    if (iVar2 == 0x19) {
      unaff_retaddr = (**(code **)(*param_1 + 0x308))(0);
    }
    else if (iVar2 == 3) {
      unaff_retaddr = (**(code **)(*param_1 + 0x304))(0);
    }
    else {
      FUN_00481d10(s_Invalid_fighting_action_005e042c,0);
    }
  }
  else {
    unaff_retaddr = (**(code **)(*param_1 + 0x30c))(0);
  }
  iVar4 = (**(code **)(*param_1 + 0x138))(unaff_retaddr,0xffffffff);
  if (iVar4 < 0) {
    ExceptionList = local_c;
    return 0;
  }
  if (((int *)param_1[0x36] != (int *)0x0) &&
     (((iVar4 = *(int *)param_1[0x36], iVar4 == 2 || (iVar4 == 4)) || (iVar4 == 0x1a)))) {
    FUN_004cee70(0);
  }
  param_3 = FUN_00482fb0(100);
  uStack_4 = 0;
  if (param_3 == 0) {
    iVar4 = 0;
  }
  else {
    iVar4 = FUN_004da9f0(unaff_retaddr,iVar2);
  }
  iVar6 = param_1[0x36];
  iVar1 = *param_1;
  uStack_4 = 0xffffffff;
  *(int **)(iVar4 + 0x44) = piVar7;
  iVar6 = (**(code **)(iVar1 + 0x13c))(iVar6 + 4,iVar4 + 4,0xffffffff);
  *(uint *)(iVar4 + 0x60) = (-1 < iVar6 | 2) << 4 | *(uint *)(iVar4 + 0x60) & 0xffffffef;
  uVar5 = (**(code **)(*param_1 + 0x30c))(0);
  iVar6 = FUN_004dab80(uVar5);
  if (iVar6 == 0) {
    if (iVar2 == 0x19) {
      if ((*(int *)param_1[0x36] != 3) && (*(int *)param_1[0x36] != 0xc)) goto LAB_004d3e9e;
    }
    else if ((iVar2 != 3) || (*(int *)param_1[0x36] != 0x19)) goto LAB_004d3e9e;
  }
  *(uint *)(param_1[0x36] + 0x60) = *(uint *)(param_1[0x36] + 0x60) & 0xffffffef;
LAB_004d3e9e:
  if ((piVar7 == (int *)0x0) || (param_1[0x95] != 0)) {
    *(uint *)(iVar4 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
  }
  else {
    uVar5 = FUN_0046ea90(piVar7);
    *(undefined4 *)(iVar4 + 0x2c) = uVar5;
  }
  if (((int *)param_1[0x36] == (int *)0x0) ||
     (((iVar2 = *(int *)param_1[0x36], iVar2 != 2 && (iVar2 != 4)) && (iVar2 != 0x1a)))) {
    *(undefined4 *)(iVar4 + 0x30) = *(undefined4 *)(iVar4 + 0x2c);
  }
  else {
    *(int *)(iVar4 + 0x30) = param_1[0x2c];
  }
  iVar2 = (**(code **)(*param_1 + 0x208))(iVar4,0);
  if ((iVar2 == 0) && (iVar4 != 0)) {
    if (*(int *)(iVar4 + 0x5c) != 0) {
      FUN_00482f80(*(int *)(iVar4 + 0x5c));
    }
    FUN_004830f0(iVar4);
  }
  if (param_1[0x21] != 0) {
    piVar3 = (int *)param_1[0x38];
    if ((piVar3 == (int *)0x0) || ((*piVar3 != 3 && ((piVar3 == (int *)0x0 || (*piVar3 != 0x19))))))
    {
      iVar2 = 0;
    }
    else {
      iVar2 = piVar3[0x11];
    }
    if ((piVar3 == (int *)0x0) || ((*piVar3 != 3 && ((piVar3 == (int *)0x0 || (*piVar3 != 0x19))))))
    {
      iVar4 = 0;
    }
    else {
      iVar4 = piVar3[0x11];
    }
    iVar2 = FUN_00492640(10,0,0,iVar4,&DAT_005e044c,iVar2,s_enemy_005e0444);
    if (iVar2 != 0) {
      param_1[0x44] = param_1[0x44] | 2;
    }
  }
  param_1[0x48] = -1;
  ExceptionList = local_c;
  return 1;
}


