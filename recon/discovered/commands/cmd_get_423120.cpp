// FUN_00423120 @ 00423120 size=138

undefined4 FUN_00423120(int *param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
    return 4;
  }
  piVar1 = (int *)FUN_00451fe0(*(undefined4 *)(param_2 + 0x28),param_1,1,0);
  if (piVar1 == (int *)0x0) {
    FUN_0041ee50(s_Can_t_find_any_object_by_that_na_005cb6c8);
    FUN_00479580();
    return 0;
  }
  iVar2 = (**(code **)(*piVar1 + 0xb4))();
  if (iVar2 == 0) {
    (**(code **)(*piVar1 + 0x90))();
  }
  else {
    (**(code **)(*piVar1 + 0x60))();
  }
  (**(code **)(*param_1 + 0x58))(piVar1,0xffffffff);
  FUN_00479580();
  return 0;
}


