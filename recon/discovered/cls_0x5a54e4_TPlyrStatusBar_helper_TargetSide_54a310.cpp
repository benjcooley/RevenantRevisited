// FUN_0054a310 @ 0054a310 size=695

void __fastcall FUN_0054a310(int param_1)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  
  if (((((DAT_00667fcc != (int *)0x0) && (piVar4 = (int *)DAT_00667fcc[0x38], piVar4 != (int *)0x0))
       && ((*piVar4 == 3 || ((piVar4 != (int *)0x0 && (*piVar4 == 0x19)))))) &&
      (piVar4 = (int *)piVar4[0x11], piVar4 != (int *)0x0)) && (piVar4 != DAT_00667fcc)) {
    piVar1 = *(int **)(param_1 + 0x6c);
    (**(code **)(*piVar1 + 100))(0,0,piVar1[1],piVar1[2],0,0xffff,0x7f7f,0x80000000);
    iVar2 = (**(code **)(*piVar4 + 0x130))();
    if ((iVar2 != 0) && (*(int *)(param_1 + 0x60) != 0)) {
      FUN_004bd680(0,0,iVar2,0x80000000,0);
    }
    uVar3 = FUN_0046d710(s_BackPanel_005e5774);
    piVar4 = (int *)FUN_0046d710(&DAT_005e5780);
    uVar5 = FUN_0046d710(s_HealthIcon_005e5788);
    uVar6 = FUN_0046d710(s_ManaIcon_005e5794);
    uVar7 = FUN_0046d710(s_FatigueIcon_005e57a0);
    FUN_004bd680(0,0,uVar3,0x80000000,0);
    if (iVar2 != 0) {
      (**(code **)(**(int **)(param_1 + 0x6c) + 0x5c))
                (&stack0xffffff8c,*(undefined4 *)(param_1 + 0x60),0,0);
    }
    FUN_004bd680(0x62 - (*piVar4 >> 1),0x1f - (piVar4[1] >> 1),piVar4,0x2000,0);
    FUN_004bd680(0x3a,0x20,uVar7,0x2000,0);
    FUN_004bd680(0x3a,0x11,uVar6,0x2000,0);
    FUN_004bd680(0x3a,3,uVar5,0x2000,0);
    (**(code **)(**(int **)(param_1 + 0x74) + 0x5c))
              (&stack0xffffff8c,*(undefined4 *)(param_1 + 0x6c),0,0);
  }
  return;
}


