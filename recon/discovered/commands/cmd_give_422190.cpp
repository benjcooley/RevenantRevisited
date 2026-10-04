// FUN_00422190 @ 00422190 size=152

undefined4 FUN_00422190(int *param_1,int param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar2 = 1;
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
    return 4;
  }
  uVar1 = FUN_00451fe0(*(undefined4 *)(param_2 + 0x28),param_1,0,0);
  FUN_00479580();
  if (*(int *)(param_2 + 0x10) == 8) {
    uVar2 = *(undefined4 *)(param_2 + 0x14);
    FUN_00479580();
  }
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
    return 4;
  }
  uVar2 = (**(code **)(*param_1 + 0x70))(uVar1,*(undefined4 *)(param_2 + 0x28),uVar2);
  FUN_0041ee50(s__d__s_given_005cb330,uVar2,*(undefined4 *)(param_2 + 0x28));
  FUN_00479580();
  return 0;
}


