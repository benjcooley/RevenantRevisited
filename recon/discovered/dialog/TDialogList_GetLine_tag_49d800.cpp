// FUN_0049d800 @ 0049d800 size=286

undefined * __thiscall FUN_0049d800(undefined4 *param_1,char **param_2)

{
  char *pcVar1;
  int *piVar2;
  int iVar3;
  undefined1 **local_40;
  char *local_3c;
  int local_38;
  char local_34 [39];
  undefined1 local_d;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  pcVar1 = (char *)param_2;
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059dc54;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  _strncpy(local_34,(char *)param_2,0x27);
  local_d = 0;
  FUN_0059be72(local_34);
  local_3c = local_34;
  param_2 = &local_3c;
  local_38 = 0;
  piVar2 = (int *)FUN_0058e8bb(&param_2,param_1[4],*param_1,4,&LAB_0049d190);
  if ((piVar2 == (int *)0x0) || (iVar3 = *piVar2, iVar3 == 0)) {
    local_40 = &local_3c;
    piVar2 = (int *)FUN_0058e8bb(&local_40,param_1[9],param_1[5],4,&LAB_0049d190);
    if (piVar2 == (int *)0x0) {
      iVar3 = 0;
    }
    else {
      iVar3 = *piVar2;
    }
  }
  local_38 = 0;
  local_3c = (char *)0x0;
  if (iVar3 != 0) {
    ExceptionList = local_c;
    return *(undefined **)(iVar3 + 4);
  }
  FUN_0058b100(&DAT_006687c0,&DAT_005dab5c,pcVar1);
  local_4 = 0xffffffff;
  if (local_3c != (char *)0x0) {
    FUN_00482f80(local_3c);
  }
  if (local_38 != 0) {
    FUN_00482f80(local_38);
  }
  ExceptionList = local_c;
  return &DAT_006687c0;
}


