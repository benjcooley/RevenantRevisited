// FUN_0049a100 @ 0049a100 size=143

void __fastcall FUN_0049a100(int *param_1)

{
  int iVar1;
  void **ppvVar2;
  int iVar3;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059dba8;
  local_c = ExceptionList;
  iVar3 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < *param_1) {
    do {
      if ((-1 < iVar3) && (iVar1 = *(int *)(param_1[4] + iVar3 * 4), iVar1 != 0)) {
        FUN_004830f0(iVar1);
      }
      FUN_0041cb40(iVar3);
      iVar3 = iVar3 + 1;
      ppvVar2 = ExceptionList;
    } while (iVar3 < *param_1);
  }
  ExceptionList = ppvVar2;
  *param_1 = 0;
  param_1[1] = 0;
  FUN_004830f0(param_1[5]);
  local_4 = 0xffffffff;
  FUN_004830f0(param_1[4]);
  ExceptionList = local_c;
  return;
}


