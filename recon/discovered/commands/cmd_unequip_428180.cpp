// FUN_00428180 @ 00428180 size=54

undefined4 FUN_00428180(int param_1,int param_2)

{
  if ((*(short *)(param_1 + 4) == 0xb) && (*(int *)(param_2 + 0x28) != 0)) {
    FUN_00519ff0(*(int *)(param_2 + 0x28));
    FUN_00478a10();
    FUN_00479580();
    return 0;
  }
  return 4;
}


