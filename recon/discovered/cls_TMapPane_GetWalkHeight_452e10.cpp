// FUN_00452e10_GetWalkHeight @ 00452e10 size=135

void FUN_00452e10_GetWalkHeight(int *param_1,undefined4 param_2,int *param_3)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  uint uVar4;
  uint uVar5;
  
  iVar1 = param_1[1];
  iVar2 = *param_1;
  piVar3 = (int *)FUN_00499e10_TSector_FindLoadedSector(param_2,iVar2 + 8 >> 10,iVar1 + 8 >> 10);
  if (piVar3 == (int *)0x0) {
    return;
  }
  uVar4 = iVar2 + 8 >> 4 & 0x3f;
  uVar5 = iVar1 + 8 >> 4 & 0x3f;
  if (param_3 != (int *)0x0) {
    if (*piVar3 != 0) {
      *param_3 = (int)(uint)*(ushort *)(*piVar3 + (uVar5 * 0x40 + uVar4) * 2) >> 10;
      FUN_00499720_TSector_ReturnWalkmap(uVar4,uVar5);
      return;
    }
    *param_3 = 0;
  }
  FUN_00499720_TSector_ReturnWalkmap(uVar4,uVar5);
  return;
}


