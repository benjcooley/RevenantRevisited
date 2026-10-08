// FUN_00426c60 @ 00426c60 size=110

undefined4 FUN_00426c60(int *param_1)

{
  int iVar1;
  undefined1 auStack_20 [32];
  
  if (param_1 == (int *)0x0) {
    FUN_0058b100(auStack_20,s_Command_requires_a_context__005cc68c);
  }
  else {
    iVar1 = (**(code **)(*param_1 + 0x88))();
    if (iVar1 == 0) {
      FUN_0058b100(auStack_20,s_Empty_Slot_is_FALSE_005cc674);
    }
    else {
      FUN_0058b100(auStack_20,s_Empty_Slot_is_TRUE_005cc660);
    }
  }
  FUN_0041ee50(auStack_20);
  FUN_00479580();
  return 0;
}


