// FUN_00420050 @ 00420050 size=172

undefined4 FUN_00420050(int *param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = (int *)0x0;
  iVar1 = 0;
  if ((*(int *)(param_2 + 0x10) == 4) || (*(int *)(param_2 + 0x10) == 2)) {
    piVar2 = (int *)FUN_00451fe0(*(undefined4 *)(param_2 + 0x28),param_1,0,0);
    FUN_00479580();
  }
  if ((*(int *)(param_2 + 0x10) == 4) || (*(int *)(param_2 + 0x10) == 2)) {
    iVar1 = FUN_00451fe0(*(undefined4 *)(param_2 + 0x28),param_1,0,0);
    FUN_00479580();
  }
  if ((param_1 != (int *)0x0) && (piVar2 != (int *)0x0)) {
    if (iVar1 != 0) {
      (**(code **)(*piVar2 + 0xbc))(param_1,*(undefined4 *)(iVar1 + 0x40));
      return 0;
    }
    (**(code **)(*piVar2 + 0xbc))(param_1,0xffffffff);
    return 0;
  }
  (**(code **)(*param_1 + 0xbc))(param_1,0xffffffff);
  return 0;
}


