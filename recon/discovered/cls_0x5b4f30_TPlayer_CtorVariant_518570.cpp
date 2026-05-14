// FUN_00518570 @ 00518570 size=477

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00518570(undefined4 *param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined4 *puVar4;
  void *local_c;
  undefined1 *puStack_8;
  int local_4;
  
  puStack_8 = &LAB_005a1165;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  *param_1 = &PTR_FUN_005b4f30;
  local_4 = 4;
  if ((param_1[2] & 0x80000000) == 0) {
    FUN_0046e630();
  }
  if (param_1 == DAT_00667fcc) {
    FUN_005496a0(0);
    FUN_00535d80(0);
    FUN_00533dc0(0);
    _DAT_00667e5c = 0;
    _DAT_00667eb0 = 1;
    FUN_005391a0(0);
  }
  iVar3 = 0;
  if (0 < (int)param_1[0xbb]) {
    do {
      if ((-1 < iVar3) && (iVar1 = *(int *)(param_1[0xbf] + iVar3 * 4), iVar1 != 0)) {
        FUN_004830f0(iVar1);
      }
      FUN_0041cb40(iVar3);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)param_1[0xbb]);
  }
  iVar3 = DAT_0065a890;
  iVar1 = 0;
  param_1[0xbb] = 0;
  param_1[0xbc] = 0;
  piVar2 = DAT_0065a8a0;
  if (0 < iVar3) {
    do {
      if ((undefined4 *)*piVar2 == param_1) {
        FUN_0051f200(iVar1,0);
        goto LAB_0051866b;
      }
      iVar1 = iVar1 + 1;
      piVar2 = piVar2 + 1;
    } while (iVar1 < iVar3);
  }
  FUN_0051f370();
LAB_0051866b:
  local_4._0_1_ = 3;
  puVar4 = param_1 + 0xa8;
  for (iVar3 = 0xb; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = 0;
    puVar4 = puVar4 + 1;
  }
  if (param_1[0xd7] != 0) {
    FUN_004830f0(param_1[0xd7]);
  }
  local_4._0_1_ = 2;
  if (param_1[0xd4] != 0) {
    FUN_004830f0(param_1[0xd4]);
  }
  param_1[0xc5] = &PTR_LAB_005b5354;
  piVar2 = param_1 + 0xce;
  iVar3 = 0;
  local_4._0_1_ = 5;
  if (0 < *piVar2) {
    do {
      if ((-1 < iVar3) && (*(int *)(param_1[0xd2] + iVar3 * 4) != 0)) {
        FUN_00520070(1);
      }
      FUN_0041cb40(iVar3);
      iVar3 = iVar3 + 1;
    } while (iVar3 < *piVar2);
  }
  *piVar2 = 0;
  param_1[0xcf] = 0;
  local_4._0_1_ = 1;
  FUN_004830f0(param_1[0xd2]);
  local_4 = (uint)local_4._1_3_ << 8;
  FUN_004830f0(param_1[0xbf]);
  local_4 = 0xffffffff;
  FUN_004c1770();
  ExceptionList = local_c;
  return;
}


