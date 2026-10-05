// FUN_0049c430 @ 0049c430 size=79

int __thiscall FUN_0049c430(int param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 local_1c [7];
  
  local_1c[0] = param_2;
  param_2 = local_1c;
  iVar1 = FUN_0058e8bb(&param_2,*(undefined4 *)(param_1 + 0x2c),*(undefined4 *)(param_1 + 0x1c),4,
                       &LAB_0049ab00);
  if (iVar1 == 0) {
    return -1;
  }
  return iVar1 - *(int *)(param_1 + 0x2c) >> 2;
}


