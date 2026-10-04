// FUN_00422d40 @ 00422d40 size=38

undefined4 FUN_00422d40(int *param_1)

{
  uint uVar1;
  
  if (param_1 != (int *)0x0) {
    uVar1 = param_1[2];
    if ((uVar1 & 0x800) != 0) {
      (**(code **)(*param_1 + 0x40))(uVar1 & 0xfffff7ff);
      return 0;
    }
    (**(code **)(*param_1 + 0x40))(uVar1 | 0x800);
  }
  return 0;
}


