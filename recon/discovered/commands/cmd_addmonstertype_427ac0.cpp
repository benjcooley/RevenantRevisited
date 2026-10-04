// FUN_00427ac0 @ 00427ac0 size=152

undefined4 FUN_00427ac0(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iStack_40;
  undefined1 auStack_3c [60];
  
  iVar1 = FUN_0047a410(param_2,s__s__d__d_005cc94c,auStack_3c,&iStack_40,&param_2);
  if (iVar1 == 0) {
    return 4;
  }
  iVar1 = FUN_00517050(auStack_3c,iStack_40,param_2);
  if (iVar1 != 0) {
    FUN_0041ee50(s__s__freq___ds___tt___d___added_s_005cc958,auStack_3c,iStack_40 / 0x18,param_2);
    return 0;
  }
  FUN_0041ee50(s_Could_not_add___s__to_the_monste_005cc988,auStack_3c);
  return 0;
}


