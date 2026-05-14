// FUN_00499db0_TSector_InPreloadArea @ 00499db0 size=94

undefined4 FUN_00499db0_TSector_InPreloadArea(undefined4 *param_1,int param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  
  if (param_2 == DAT_005da754) {
    iVar4 = 0;
    uVar1 = *param_1;
    uVar2 = param_1[1];
    if (0 < DAT_006687a8) {
      do {
        iVar3 = FUN_0041c720(uVar1,uVar2);
        if (iVar3 != 0) {
          return 1;
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 < DAT_006687a8);
    }
  }
  return 0;
}


