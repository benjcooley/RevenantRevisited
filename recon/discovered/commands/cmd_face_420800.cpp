// FUN_00420800 @ 00420800 size=60

undefined4 FUN_00420800(int param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = FUN_0047a410(param_2,&DAT_005caf04,&param_2);
  if (iVar1 == 0) {
    return 4;
  }
  if (param_1 != 0) {
    *(char *)(param_1 + 0x36) = (char)param_2;
    *(undefined4 *)(param_1 + 0xb0) = param_2;
  }
  return 1;
}


