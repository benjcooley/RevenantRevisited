// FUN_0049d6d0 @ 0049d6d0 size=168

int __thiscall FUN_0049d6d0(int *param_1,char **param_2)

{
  int iVar1;
  char *local_30;
  undefined4 local_2c;
  char local_28 [39];
  undefined1 local_1;
  
  _strncpy(local_28,(char *)param_2,0x27);
  local_1 = 0;
  FUN_0059be72(local_28);
  local_30 = local_28;
  param_2 = &local_30;
  local_2c = 0;
  iVar1 = FUN_0058e8bb(&param_2,param_1[4],*param_1,4,&LAB_0049d190);
  if ((iVar1 == 0) || (iVar1 = iVar1 - param_1[4] >> 2, iVar1 < 0)) {
    param_2 = &local_30;
    iVar1 = FUN_0058e8bb(&param_2,param_1[9],param_1[5],4,&LAB_0049d190);
    if (iVar1 == 0) {
      return -1;
    }
    iVar1 = iVar1 - param_1[9] >> 2;
    if (-1 < iVar1) {
      iVar1 = iVar1 + *param_1;
    }
  }
  return iVar1;
}


