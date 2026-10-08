// FUN_004234e0 @ 004234e0 size=119

undefined4 FUN_004234e0(undefined4 param_1,int param_2)

{
  int iVar1;
  
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
    return 4;
  }
  iVar1 = FUN_00451fe0(*(undefined4 *)(param_2 + 0x28),param_1,1,0);
  if (iVar1 == 0) {
    FUN_0041ee50(s_Can_t_find_any_object_by_that_na_005cb724);
    FUN_00479580();
    return 0;
  }
  iVar1 = FUN_0044eb60(param_1,iVar1);
  if (iVar1 == 0) {
    FUN_0041ee50(s_Hrm___the_objects_are_in_differe_005cb74c);
  }
  FUN_00479580();
  return 0;
}


