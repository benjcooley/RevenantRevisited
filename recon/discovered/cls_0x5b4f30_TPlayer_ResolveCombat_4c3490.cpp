// FUN_004c3490_ResolveCombat @ 004c3490 size=543

int __thiscall FUN_004c3490_ResolveCombat(int *param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  
  iVar2 = FUN_004db2b0_TComplexObject_ResolveAction(param_2);
  if (iVar2 != 0) {
    return iVar2;
  }
  piVar3 = (int *)param_1[0x36];
  if ((*piVar3 == 2) ||
     ((((int *)param_1[0x38] != (int *)0x0 &&
       ((((*(int *)param_1[0x38] == 3 &&
          ((iVar2 = FUN_004dab80_TActionBlock_Is(s_combatrun_005e0618), iVar2 != 0 ||
           (iVar2 = FUN_004dab80_TActionBlock_Is(s_handrun_005e0610), iVar2 != 0)))) ||
         (((int *)param_1[0x38] != (int *)0x0 &&
          ((*(int *)param_1[0x38] == 0x19 && (iVar2 = FUN_004dab80_TActionBlock_Is(s_bowrun_005e0608), iVar2 != 0)))
          ))) || (iVar2 = FUN_004dab80_TActionBlock_Is(&DAT_005e0604), iVar2 != 0)))) &&
      ((piVar3 = (int *)param_1[0x36], piVar3 != (int *)0x0 &&
       (((iVar2 = *piVar3, iVar2 == 2 || (iVar2 == 4)) || (iVar2 == 0x1a)))))))) {
    iVar2 = (**(code **)(*param_1 + 0x31c))(piVar3,param_2);
  }
  else {
    piVar3 = (int *)param_1[0x36];
    iVar1 = *piVar3;
    if (iVar1 == 7) {
      iVar2 = (**(code **)(*param_1 + 800))(piVar3,param_2);
      return iVar2;
    }
    if (((iVar1 == 0xc) || (iVar1 == 0xe)) || (iVar1 == 0xd)) {
      iVar2 = (**(code **)(*param_1 + 0x338))(piVar3,param_2);
      return iVar2;
    }
    if (iVar1 == 8) {
      iVar2 = (**(code **)(*param_1 + 0x334))(piVar3,param_2);
      return iVar2;
    }
    if ((iVar1 == 4) || (iVar1 == 0x1a)) {
      iVar2 = (**(code **)(*param_1 + 0x324))(piVar3,param_2);
      return iVar2;
    }
    if ((iVar1 == 3) || (iVar1 == 0x19)) {
      iVar2 = (**(code **)(*param_1 + 0x328))(piVar3,param_2);
      return iVar2;
    }
    if (iVar1 == 5) {
      iVar2 = (**(code **)(*param_1 + 0x348))(piVar3,param_2);
      return iVar2;
    }
    if (iVar1 == 0x1b) {
      iVar2 = (**(code **)(*param_1 + 0x32c))(piVar3,param_2);
      return iVar2;
    }
    if (iVar1 == 0x1c) {
      iVar2 = (**(code **)(*param_1 + 0x330))(piVar3,param_2);
      return iVar2;
    }
    if (iVar1 == 0x10) {
      iVar2 = (**(code **)(*param_1 + 0x340))(piVar3,param_2);
      return iVar2;
    }
    if (iVar1 == 0x11) {
      iVar2 = (**(code **)(*param_1 + 0x344))(piVar3,param_2);
      return iVar2;
    }
    iVar2 = 0;
    if (iVar1 == 0x13) {
      iVar2 = (**(code **)(*param_1 + 0x33c))(piVar3,param_2);
      return iVar2;
    }
  }
  return iVar2;
}


