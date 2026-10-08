// FUN_00427a50 @ 00427a50 size=40

undefined4 FUN_00427a50(undefined4 param_1,int param_2)

{
  if ((*(int *)(param_2 + 0x10) != 2) && (*(int *)(param_2 + 0x10) != 4)) {
    return 4;
  }
  FUN_00532fe0(*(undefined4 *)(param_2 + 0x28));
  return 0;
}


