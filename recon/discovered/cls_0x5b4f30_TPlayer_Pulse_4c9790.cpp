// FUN_004c9790_Pulse @ 004c9790 size=856

void __fastcall FUN_004c9790_Pulse(int *param_1)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  if (param_1[0x93] != 0) {
    param_1[0x93] = param_1[0x93] + -1;
    return;
  }
  if (((param_1[0x38] != 0) &&
      ((((iVar2 = FUN_0045f770(3), iVar2 != 0 &&
         ((iVar2 = FUN_004dab80_TActionBlock_Is(s_combatrun_005e0618), iVar2 != 0 ||
          (iVar2 = FUN_004dab80_TActionBlock_Is(s_handrun_005e0610), iVar2 != 0)))) ||
        (((int *)param_1[0x38] != (int *)0x0 &&
         ((*(int *)param_1[0x38] == 0x19 && (iVar2 = FUN_004dab80_TActionBlock_Is(s_bowrun_005e0608), iVar2 != 0))))
        )) || (iVar2 = FUN_004dab80_TActionBlock_Is(&DAT_005e0604), iVar2 != 0)))) &&
     (((iVar2 = FUN_0045f770(3), iVar2 != 0 ||
       (((int *)param_1[0x38] != (int *)0x0 && (*(int *)param_1[0x38] == 0x19)))) &&
      (*(int *)(param_1[0x38] + 0x44) != 0)))) {
    FUN_004cf000_SetWalkMode();
    iVar2 = FUN_0045f770(3);
    if ((iVar2 == 0) && (((int *)param_1[0x38] == (int *)0x0 || (*(int *)param_1[0x38] != 0x19)))) {
      uVar3 = 0;
    }
    else {
      uVar3 = *(undefined4 *)(param_1[0x38] + 0x44);
    }
    FUN_004d3b90_BeginFighting(uVar3,3);
  }
  piVar1 = (int *)param_1[0x38];
  if ((piVar1 == (int *)0x0) || (*piVar1 != 3)) {
    if (param_1[0x94] == 0) {
      iVar2 = (**(code **)(*param_1 + 0x1e0))();
      iVar4 = (**(code **)(*param_1 + 0x1c8))();
      if ((iVar4 == iVar2) && (iVar2 = FUN_00483300_RandomRange(1,100), iVar2 == 100)) {
        iVar2 = FUN_00483300_RandomRange(100,800);
        param_1[0x94] = iVar2;
        FUN_004cf490_SetRunMode();
      }
    }
    else {
      param_1[0x94] = param_1[0x94] + -1;
      iVar2 = (**(code **)(*param_1 + 0x1e0))();
      iVar4 = (**(code **)(*param_1 + 0x1c8))();
      if (((iVar4 < iVar2 / 2) && (iVar2 = FUN_00483300_RandomRange(1,100), 0x32 < iVar2)) ||
         (((iVar2 = FUN_0045f770(3), iVar2 != 0 ||
           (((int *)param_1[0x38] != (int *)0x0 && (*(int *)param_1[0x38] == 0x19)))) &&
          ((*(int *)(param_1[0x38] + 0x44) != 0 && (iVar2 = FUN_00483300_RandomRange(1,100), 0xf < iVar2)))))) {
        param_1[0x94] = 0;
      }
      if (param_1[0x94] == 0) {
        FUN_004cf000_SetWalkMode();
      }
    }
    iVar2 = param_1[0x8d];
    if (iVar2 == 0) {
      iVar2 = FUN_004c95e0_FindWaypoint();
      param_1[0x8d] = iVar2;
      if (iVar2 == 0) {
        param_1[0x93] = 1000;
        return;
      }
      FUN_004cedb0(*(undefined4 *)(iVar2 + 0x10),*(undefined4 *)(iVar2 + 0x14),0);
      return;
    }
    FUN_004cedb0(*(undefined4 *)(iVar2 + 0x10),*(undefined4 *)(iVar2 + 0x14),0);
    iVar2 = param_1[0x8d];
    iVar4 = param_1[4] - *(int *)(iVar2 + 0x10);
    if (iVar4 < 0) {
      iVar4 = *(int *)(iVar2 + 0x10) - param_1[4];
    }
    iVar5 = param_1[5] - *(int *)(iVar2 + 0x14);
    if (iVar5 < 0) {
      iVar5 = *(int *)(iVar2 + 0x14) - param_1[5];
    }
    iVar6 = iVar4;
    if (iVar5 <= iVar4) {
      iVar6 = iVar5;
    }
    iVar2 = FUN_0045f730(0,param_1[6],(iVar5 - (iVar6 >> 1)) + iVar4,*(undefined4 *)(iVar2 + 0x18));
    if (iVar2 < 0x19) {
      iVar2 = param_1[0x8d];
      iVar4 = FUN_004c95e0_FindWaypoint();
      param_1[0x8e] = iVar2;
      param_1[0x8d] = iVar4;
      return;
    }
  }
  else {
    if (piVar1[0x11] == 0) {
      FUN_004d3fd0_EndFighting();
      param_1[0x93] = 100;
      return;
    }
    if (param_1[0x95] != 0) {
      if (param_1[0x94] == 0) {
        iVar2 = FUN_00483300_RandomRange(1,100);
        if (0x1e < iVar2) {
          iVar2 = FUN_00483300_RandomRange(800,2000);
          param_1[0x94] = iVar2;
          FUN_004cf490_SetRunMode();
          return;
        }
      }
      else {
        iVar2 = param_1[0x94] + -1;
        param_1[0x94] = iVar2;
        if (iVar2 == 0) {
          FUN_004cf000_SetWalkMode();
        }
      }
    }
  }
  return;
}


