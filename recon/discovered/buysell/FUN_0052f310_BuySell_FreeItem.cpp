// FUN_0052f310 @ 0052f310 size=118

void __fastcall FUN_0052f310(int *param_1)

{
  int iVar1;
  int *piVar2;
  
  if (*param_1 != 0) {
    FUN_004830f0(*param_1);
    *param_1 = 0;
  }
  if (param_1[1] != 0) {
    FUN_004830f0(param_1[1]);
    param_1[1] = 0;
  }
  if (param_1[2] != 0) {
    FUN_004830f0(param_1[2]);
    param_1[2] = 0;
  }
  iVar1 = 0;
  if (0 < param_1[0xd]) {
    piVar2 = param_1 + 3;
    do {
      if (*piVar2 != 0) {
        FUN_004830f0(*piVar2);
        *piVar2 = 0;
      }
      iVar1 = iVar1 + 1;
      piVar2 = piVar2 + 2;
    } while (iVar1 < param_1[0xd]);
    param_1[0xd] = 0;
    return;
  }
  param_1[0xd] = 0;
  return;
}


