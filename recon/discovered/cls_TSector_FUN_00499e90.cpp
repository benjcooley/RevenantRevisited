// FUN_00499e90_TSector_ComputeStateHash @ 00499e90 size=344

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __fastcall FUN_00499e90_TSector_ComputeStateHash(int param_1)

{
  uint *puVar1;
  undefined4 uVar2;
  int *piVar3;
  uint uVar4;
  int iVar5;
  int local_2c;
  uint local_28;
  uint local_24;
  undefined1 local_20 [4];
  int local_1c;
  int local_18;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059db72;
  local_c = ExceptionList;
  local_24 = _DAT_0065a250;
  _DAT_0065a250 = _DAT_0065a250 | 2;
  ExceptionList = &local_c;
  FUN_0049cc20(0x8000,0x3f9c);
  piVar3 = *(int **)(param_1 + 200);
  puVar1 = (uint *)(param_1 + 0xb8);
  uVar4 = 0;
  local_4 = 0;
  FUN_0056ff60();
  local_28 = *(uint *)(param_1 + 4);
  FUN_0056ff80(&local_28,4,0);
  local_28 = *(uint *)(param_1 + 8);
  FUN_0056ff80(&local_28,4,0);
  local_28 = *(uint *)(param_1 + 0xc);
  FUN_0056ff80(&local_28,4,0);
  local_28 = *puVar1;
  FUN_0056ff80(&local_28,4,0);
  if (*puVar1 != 0) {
    for (; (puVar1 != (uint *)0x0 && (uVar4 < *puVar1)); uVar4 = uVar4 + 1) {
      iVar5 = *piVar3;
      if ((iVar5 != 0) && ((*(short *)(iVar5 + 4) == 0xc || (*(short *)(iVar5 + 4) == 0xb)))) {
        FUN_00472110(iVar5,local_20);
        iVar5 = local_18 - local_1c;
        uVar2 = FUN_0049cdd0();
        FUN_0056ff80(uVar2,iVar5,0);
        local_18 = local_1c;
      }
      piVar3 = piVar3 + 1;
    }
  }
  _DAT_0065a250 = local_24;
  if (local_2c == 0) {
    local_2c = -0xf0f0f10;
  }
  local_4 = 0xffffffff;
  FUN_0049cc50();
  ExceptionList = local_c;
  return local_2c;
}


