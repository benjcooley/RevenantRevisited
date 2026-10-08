// FUN_0049ab00 @ 0049ab00 size=45

undefined4 FUN_0049ab00(undefined4 *param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  
  if (param_1 == (undefined4 *)0x0) {
    return 1;
  }
  if (param_2 == (undefined4 *)0x0) {
    return 0xffffffff;
  }
  uVar1 = FUN_0059a530(*(undefined4 *)*param_1,*(undefined4 *)*param_2);
  return uVar1;
}


