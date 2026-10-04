// FUN_00420c70 @ 00420c70 size=46

undefined4 FUN_00420c70(int param_1,int param_2)

{
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 3)) {
    return 4;
  }
  if (param_1 != 0) {
    FUN_00471290(*(undefined4 *)(param_2 + 0x28));
  }
  return 0x2000;
}


