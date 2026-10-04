// FUN_0048eea0 @ 0048eea0 size=131

undefined4 __thiscall FUN_0048eea0(int *param_1,int param_2,uint param_3)

{
  undefined4 uVar1;
  
  if ((*(int *)(param_1[5] + param_2 * 4) == 0) || (3 < param_1[7])) {
    uVar1 = 0;
  }
  else {
    param_1[param_1[7] + 8] = param_2;
    param_1[param_1[7] + 0xc] = param_3;
    param_1[7] = param_1[7] + 1;
    (**(code **)(*param_1 + 0x40))(0x101,*(undefined4 *)(param_1[5] + param_2 * 4));
    if ((param_3 & 0x100) != 0) {
      FUN_004aacb0(0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4),*(undefined4 *)(PTR_DAT_005d79e0 + 8),6
                  );
    }
    uVar1 = 1;
    if (param_1[7] == 1) {
      DAT_00668504 = param_1[0x12];
      return uVar1;
    }
  }
  return uVar1;
}


