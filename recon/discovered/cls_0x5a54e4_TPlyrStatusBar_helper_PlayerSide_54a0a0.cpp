// FUN_0054a0a0 @ 0054a0a0 size=623

void __fastcall FUN_0054a0a0(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  
  if (DAT_00667fcc != (int *)0x0) {
    piVar3 = *(int **)(param_1 + 0x6c);
    (**(code **)(*piVar3 + 100))(0,0,piVar3[1],piVar3[2],0,0xffff,0x7f7f,0x80000000);
    iVar1 = (**(code **)(*DAT_00667fcc + 0x130))();
    if (iVar1 != 0) {
      FUN_004bd680(0,0,iVar1,0x80000000,0);
    }
    uVar2 = FUN_0046d710(s_BackPanel_005e573c);
    piVar3 = (int *)FUN_0046d710(&DAT_005e5748);
    uVar4 = FUN_0046d710(s_HealthIcon_005e5750);
    uVar5 = FUN_0046d710(s_ManaIcon_005e575c);
    uVar6 = FUN_0046d710(s_FatigueIcon_005e5768);
    FUN_004bd680(0,0,uVar2,0x80000000,0);
    if (iVar1 != 0) {
      (**(code **)(**(int **)(param_1 + 0x6c) + 0x5c))
                (&stack0xffffff8c,*(undefined4 *)(param_1 + 0x60),0,0);
    }
    FUN_004bd680(0x1a - (*piVar3 >> 1),0x1f - (piVar3[1] >> 1),piVar3,0x2000,0);
    FUN_004bd680(0x2b,0x20,uVar6,0x2000,0);
    FUN_004bd680(0x2b,0x11,uVar5,0x2000,0);
    FUN_004bd680(0x2b,3,uVar4,0x2000,0);
    (**(code **)(**(int **)(param_1 + 0x70) + 0x5c))
              (&stack0xffffff8c,*(undefined4 *)(param_1 + 0x6c),0,0);
  }
  return;
}


