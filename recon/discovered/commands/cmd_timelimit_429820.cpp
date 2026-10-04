// FUN_00429820 @ 00429820 size=45

undefined4 FUN_00429820(undefined4 param_1,int param_2,undefined4 param_3,int param_4)

{
  undefined2 uVar1;
  
  if ((param_4 != 0) && (*(int *)(param_2 + 0x10) == 8)) {
    uVar1 = __ftol();
    *(undefined2 *)(param_4 + 0xc0) = uVar1;
    return 0;
  }
  return 4;
}


