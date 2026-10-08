// FUN_00422070 @ 00422070 size=128

undefined4 FUN_00422070(int *param_1)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  char *pcVar6;
  
  if (param_1 == (int *)0x0) {
    return 0;
  }
  iVar4 = 0;
  iVar5 = 0;
  iVar1 = (**(code **)(*param_1 + 0x80))();
  if (0 < iVar1) {
    do {
      piVar2 = (int *)(**(code **)(*param_1 + 0x7c))(iVar5);
      if (piVar2 != (int *)0x0) {
        uVar3 = (**(code **)(*piVar2 + 0x198))(piVar2[0xe]);
        FUN_0041ee50(s___d___s___005cb2d8,uVar3);
        iVar4 = iVar4 + 1;
      }
      iVar5 = iVar5 + 1;
      iVar1 = (**(code **)(*param_1 + 0x80))();
    } while (iVar5 < iVar1);
    if (0 < iVar4) {
      pcVar6 = &DAT_005cb2e8;
      goto LAB_004220d9;
    }
  }
  pcVar6 = s_No_items_found__005cb2ec;
LAB_004220d9:
  FUN_0041ee50(pcVar6);
  FUN_00479580();
  return 0;
}


