// FUN_00422000 @ 00422000 size=109

undefined4 FUN_00422000(int *param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 1;
  if (*(int *)(param_2 + 0x10) == 8) {
    uVar2 = *(undefined4 *)(param_2 + 0x14);
    FUN_00479580();
  }
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
    return 4;
  }
  iVar1 = (**(code **)(*param_1 + 0x54))(*(undefined4 *)(param_2 + 0x28),uVar2,0xffffffff);
  if (iVar1 == 0) {
    FUN_0041ee50(s_Unable_to_add__s_005cb2c4,*(undefined4 *)(param_2 + 0x28));
    FUN_004795c0();
    return 0;
  }
  FUN_00479580();
  return 0;
}


