// FUN_00498c90_TSector_SaveToStream @ 00498c90 size=403

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall FUN_00498c90_TSector_SaveToStream(int param_1,int param_2)

{
  uint *puVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  uint uVar7;
  undefined4 *puVar8;
  uint local_4;
  
  uVar5 = *(undefined4 *)(param_1 + 0xb8);
  puVar1 = (uint *)(param_1 + 0xb8);
  puVar8 = *(undefined4 **)(param_1 + 200);
  iVar6 = *(int *)(param_2 + 8) - *(int *)(param_2 + 4);
  local_4 = 0;
  if ((iVar6 != 0) && (*(int *)(param_2 + 0xc) != 0)) {
    *(int *)(param_2 + 8) = *(int *)(param_2 + 4);
  }
  FUN_0049a210(DAT_005da750);
  FUN_0049a1d0(0xf);
  FUN_0049a210(0);
  FUN_0049a1d0(uVar5);
  if (iVar6 != 0) {
    FUN_0049a250(iVar6);
  }
  _DAT_0065a250 = _DAT_0065a250 | 1;
  if (*puVar1 != 0) {
    for (; (puVar1 != (uint *)0x0 && (local_4 < *puVar1)); local_4 = local_4 + 1) {
      FUN_00472110(*puVar8,param_2);
      puVar8 = puVar8 + 1;
    }
  }
  uVar5 = FUN_00499e90_TSector_ComputeStateHash();
  uVar2 = *puVar1;
  iVar6 = *(int *)(param_2 + 4);
  uVar7 = *(int *)(param_2 + 8) - iVar6;
  if ((uVar7 != 0) && (*(int *)(param_2 + 0xc) != 0)) {
    *(int *)(param_2 + 8) = iVar6;
  }
  uVar4 = DAT_005da750;
  if ((*(int *)(param_2 + 0xc) - *(int *)(param_2 + 8)) + iVar6 < 4) {
    FUN_0049cc70(4);
  }
  puVar8 = *(undefined4 **)(param_2 + 8);
  iVar6 = *(int *)(param_2 + 4);
  iVar3 = *(int *)(param_2 + 0xc);
  *puVar8 = uVar4;
  puVar8 = puVar8 + 1;
  *(undefined4 **)(param_2 + 8) = puVar8;
  if ((iVar6 - (int)puVar8) + iVar3 < 4) {
    FUN_0049cc70(4);
  }
  puVar8 = *(undefined4 **)(param_2 + 8);
  iVar6 = *(int *)(param_2 + 4);
  iVar3 = *(int *)(param_2 + 0xc);
  *puVar8 = 0xf;
  puVar8 = puVar8 + 1;
  *(undefined4 **)(param_2 + 8) = puVar8;
  if ((iVar6 - (int)puVar8) + iVar3 < 4) {
    FUN_0049cc70(4);
  }
  puVar8 = *(undefined4 **)(param_2 + 8);
  iVar6 = *(int *)(param_2 + 4);
  iVar3 = *(int *)(param_2 + 0xc);
  *puVar8 = uVar5;
  puVar8 = puVar8 + 1;
  *(undefined4 **)(param_2 + 8) = puVar8;
  if ((iVar6 - (int)puVar8) + iVar3 < 4) {
    FUN_0049cc70(4);
  }
  puVar1 = *(uint **)(param_2 + 8);
  *puVar1 = uVar2;
  *(uint **)(param_2 + 8) = puVar1 + 1;
  if ((uVar7 != 0) && (uVar7 < *(uint *)(param_2 + 0xc))) {
    *(uint *)(param_2 + 8) = *(int *)(param_2 + 4) + uVar7;
  }
  return 1;
}


