// FUN_004997d0 @ 004997d0 size=214

undefined4 FUN_004997d0(int *param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  undefined4 uVar5;
  int local_10;
  int local_c;
  int local_8;
  int local_4;
  
  piVar3 = param_1;
  if (DAT_005d7a30 == 0) {
    FUN_00499be0_TSector_ClearPreloadSectors(0xffffffff,0,0,0);
    return 1;
  }
  if (param_2 == DAT_005da754) {
    iVar1 = *param_1;
    iVar2 = param_1[1];
    param_1 = (int *)0x0;
    if (0 < DAT_006687a8) {
      do {
        iVar4 = FUN_0041c720(iVar1,iVar2);
        if (iVar4 != 0) {
          return 1;
        }
        param_1 = (int *)((int)param_1 + 1);
      } while ((int)param_1 < DAT_006687a8);
    }
  }
  local_10 = *piVar3;
  local_4 = (DAT_005d79ec * 0x400) / 2;
  local_8 = local_10 + local_4;
  local_10 = local_10 - local_4;
  iVar1 = piVar3[1];
  local_c = iVar1 - local_4;
  local_4 = local_4 + iVar1;
  uVar5 = FUN_004998b0_TSector_LoadPreloadSectors(param_2,1,&local_10,param_3);
  return uVar5;
}


