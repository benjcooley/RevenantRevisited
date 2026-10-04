// FUN_00492ac0 @ 00492ac0 size=55

int __fastcall FUN_00492ac0(int param_1)

{
  int iVar1;
  
  if ((((*(int *)(param_1 + 0xcc) != 0) &&
       (iVar1 = FUN_0059a530(*(int *)(param_1 + 0xcc),&DAT_005da134), iVar1 == 0)) &&
      (iVar1 = *(int *)(param_1 + 0xc4), iVar1 != 0)) && (*(short *)(iVar1 + 4) == 0xb)) {
    return iVar1;
  }
  return DAT_00667fcc;
}


