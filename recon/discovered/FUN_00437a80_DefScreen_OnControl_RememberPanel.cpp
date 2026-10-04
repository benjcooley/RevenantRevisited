// FUN_00437a80 @ 00437a80 size=99

undefined4 __thiscall FUN_00437a80(int param_1,int param_2,int param_3)

{
  int iVar1;
  
  if ((param_3 == 3000) && (param_2 != 0)) {
    iVar1 = FUN_0059a600(param_2 + 0x18,s_Panel_005cdc20,5);
    if (iVar1 == 0) {
      _strncpy((char *)(param_1 + 0x158),(char *)(param_2 + 0x1d),0x1f);
      *(undefined1 *)(param_1 + 0x177) = 0;
      *(undefined4 *)(param_1 + 0x150) = 1;
      return 1;
    }
  }
  return 1;
}


