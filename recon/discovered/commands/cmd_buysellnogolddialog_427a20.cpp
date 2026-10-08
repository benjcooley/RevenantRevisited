// FUN_00427a20 @ 00427a20 size=40

undefined4 FUN_00427a20(undefined4 param_1,int param_2)

{
  if ((*(int *)(param_2 + 0x10) != 2) && (*(int *)(param_2 + 0x10) != 4)) {
    return 4;
  }
  FUN_00533040(*(undefined4 *)(param_2 + 0x28));
  return 0;
}


