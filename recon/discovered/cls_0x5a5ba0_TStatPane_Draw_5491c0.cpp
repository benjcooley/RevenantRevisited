// FUN_005491c0 @ 005491c0 size=487

void __thiscall FUN_005491c0(int *param_1,int *param_2,undefined4 param_3)

{
  int *piVar1;
  undefined4 uVar2;
  int *piVar3;
  int iVar4;
  int *piVar5;
  undefined4 uVar6;
  
  (**(code **)(*param_2 + 100))(0,0,0xbc,0x132,0,0xffff,0,param_3);
  uVar6 = 0;
  uVar2 = FUN_0046d710(s_Stats_005e5674);
  FUN_004bd680(0,0,uVar2,param_3,uVar6);
  (**(code **)(*param_1 + 0x90))();
  iVar4 = *param_2;
  param_1[0x61] = (int)param_2;
  piVar3 = (int *)(**(code **)(iVar4 + 0xc))();
  piVar1 = param_1 + 0x5f;
  iVar4 = (**(code **)(*piVar3 + 0x44))(piVar3,piVar1);
  if (iVar4 != 0) {
    FUN_004a90d0(iVar4,s_d__revenant_StatPane_cpp_005e5604,0x354);
  }
  iVar4 = FUN_00521c60(DAT_0065abc4);
  SelectObject((HDC)*piVar1,*(HGDIOBJ *)(iVar4 + 0x38));
  SetViewportOrgEx((HDC)*piVar1,*(int *)(param_1[0x61] + 0x14),*(int *)(param_1[0x61] + 0x18),
                   (LPPOINT)0x0);
  piVar5 = (int *)(**(code **)(*(int *)param_1[99] + 0xc))();
  piVar3 = param_1 + 0x60;
  iVar4 = (**(code **)(*piVar5 + 0x44))(piVar5,piVar3);
  if (iVar4 != 0) {
    FUN_004a90d0(iVar4,s_d__revenant_StatPane_cpp_005e563c,0x366);
  }
  iVar4 = FUN_00521c60(DAT_0065abc4);
  SelectObject((HDC)*piVar3,*(HGDIOBJ *)(iVar4 + 0x38));
  SetViewportOrgEx((HDC)*piVar3,*(int *)(param_1[99] + 0x14),*(int *)(param_1[99] + 0x18),
                   (LPPOINT)0x0);
  param_1[0x65] = 0x800;
  iVar4 = FUN_00482fb0(0x800);
  param_1[100] = iVar4;
  FUN_005475e0();
  FUN_004830f0(param_1[100]);
  piVar5 = (int *)(**(code **)(*(int *)param_1[99] + 0xc))();
  iVar4 = (**(code **)(*piVar5 + 0x68))(piVar5,*piVar3);
  if (iVar4 != 0) {
    FUN_004a90d0(iVar4,s_d__revenant_StatPane_cpp_005e5658,0x371);
  }
  piVar3 = (int *)(**(code **)(*(int *)param_1[0x61] + 0xc))();
  iVar4 = (**(code **)(*piVar3 + 0x68))(piVar3,*piVar1);
  if (iVar4 != 0) {
    FUN_004a90d0(iVar4,s_d__revenant_StatPane_cpp_005e5620,0x360);
  }
  (**(code **)(*param_1 + 0x2c))(0);
  FUN_00435cb0(param_2);
  if (param_2 == (int *)PTR_DAT_005d79e0) {
    FUN_004aacb0(param_1[1],param_1[2],param_1[3],param_1[4],6);
  }
  return;
}


