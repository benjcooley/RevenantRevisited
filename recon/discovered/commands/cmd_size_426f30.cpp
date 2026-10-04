// FUN_00426f30 @ 00426f30 size=136

undefined4 FUN_00426f30(int *param_1,undefined4 param_2)

{
  int iVar1;
  undefined1 auStack_18 [4];
  undefined1 auStack_14 [4];
  undefined1 auStack_10 [4];
  undefined1 auStack_c [4];
  undefined1 auStack_8 [4];
  undefined1 auStack_4 [4];
  
  if (param_1 == (int *)0x0) {
    FUN_0041ee50(s_Cant_find_any_object_by_that_nam_005cc6ec);
    FUN_00479580();
    return 0;
  }
  iVar1 = FUN_0047a410(param_2,s__d__d__d__d__d__d_005cc710,auStack_18,auStack_14,auStack_10,
                       auStack_c,auStack_8,auStack_4);
  if (iVar1 != 0) {
    (**(code **)(*param_1 + 8))(auStack_18,0xffffffff,0);
    FUN_0050f2d0(auStack_c);
  }
  FUN_00479580();
  return 0;
}


