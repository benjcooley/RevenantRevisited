// FUN_004279f0 @ 004279f0 size=45

undefined4 FUN_004279f0(undefined4 param_1,int param_2)

{
  if ((*(int *)(param_2 + 0x10) != 2) && (*(int *)(param_2 + 0x10) != 4)) {
    return 4;
  }
  FUN_00532fb0(*(undefined4 *)(param_2 + 0x28),param_1);
  return 0;
}


