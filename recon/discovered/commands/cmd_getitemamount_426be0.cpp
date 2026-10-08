// FUN_00426be0 @ 00426be0 size=122

undefined4 FUN_00426be0(int *param_1,int param_2)

{
  undefined4 uVar1;
  
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
    return 4;
  }
  if (param_1 == (int *)0x0) {
    FUN_0041ee50(s_Can_t_find_any_object_by_that_na_005cc630);
    FUN_00479580();
    return 0;
  }
  uVar1 = (**(code **)(*param_1 + 0x84))
                    (*(undefined4 *)(param_2 + 0x28),*(undefined4 *)(param_2 + 0x28));
  FUN_0058b100(&stack0xffffffdc,s__d__s_005cc658,uVar1);
  FUN_0041ee50(&stack0xffffffdc);
  FUN_00479580();
  return 0;
}


