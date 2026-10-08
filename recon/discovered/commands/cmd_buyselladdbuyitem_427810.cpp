// FUN_00427810 @ 00427810 size=40

undefined4 FUN_00427810(undefined4 param_1,int param_2)

{
  if ((*(int *)(param_2 + 0x10) != 2) && (*(int *)(param_2 + 0x10) != 4)) {
    return 4;
  }
  FUN_00531d70(*(undefined4 *)(param_2 + 0x28));
  return 0;
}


