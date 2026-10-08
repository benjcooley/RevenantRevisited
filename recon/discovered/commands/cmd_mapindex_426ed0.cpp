// FUN_00426ed0 @ 00426ed0 size=86

undefined4 FUN_00426ed0(int param_1)

{
  undefined1 auStack_20 [32];
  
  if (param_1 == 0) {
    FUN_0041ee50(s_Can_t_find_any_object_by_that_na_005cc6bc);
    FUN_00479580();
    return 0;
  }
  FUN_0058b100(auStack_20,&DAT_005cc6e4,*(undefined4 *)(param_1 + 0x40));
  FUN_0041ee50(auStack_20);
  FUN_00479580();
  return 0;
}


