// FUN_00427500 @ 00427500 size=69

undefined4 FUN_00427500(undefined4 param_1,int param_2)

{
  undefined4 uVar1;
  
  if (*(int *)(param_2 + 0x10) == 8) {
    uVar1 = *(undefined4 *)(param_2 + 0x14);
    FUN_00478a10();
    FUN_00479580();
    FUN_00530670(*(undefined4 *)(param_2 + 0x28),uVar1);
    return 0;
  }
  FUN_00530670(*(undefined4 *)(param_2 + 0x28),1);
  return 0;
}


