// FUN_004220f0 @ 004220f0 size=92

undefined4 FUN_004220f0(int *param_1,int param_2)

{
  undefined4 uVar1;
  
  uVar1 = 1;
  if (*(int *)(param_2 + 0x10) == 8) {
    uVar1 = *(undefined4 *)(param_2 + 0x14);
    FUN_00479580();
  }
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
    return 4;
  }
  uVar1 = (**(code **)(*param_1 + 0x78))(*(undefined4 *)(param_2 + 0x28),uVar1);
  FUN_0041ee50(s__d__s_deleted_005cb300,uVar1,*(undefined4 *)(param_2 + 0x28));
  FUN_00479580();
  return 0;
}


