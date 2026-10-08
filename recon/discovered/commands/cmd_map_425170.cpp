// FUN_00425170 @ 00425170 size=377

undefined4 FUN_00425170(undefined4 param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined *puVar5;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  iVar4 = DAT_00666970;
  if (*(int *)(param_2 + 0x10) == 4) {
    iVar1 = FUN_00479700(&DAT_005cbd04,0);
    if (iVar1 != 0) {
      FUN_0041ee50(s_The_following_map_locations_are_a_005cbd0c);
      iVar4 = 0;
      if (0 < DAT_00655494) {
        puVar3 = &DAT_00654eb8;
        do {
          FUN_0058b100(&DAT_00654a88,s__20s___5d___5d___3d__level__2d_005cbd38,puVar3 + -10,
                       puVar3[-2],puVar3[-1],*puVar3,puVar3[1]);
          FUN_0041ee50(&DAT_00654a88);
          iVar4 = iVar4 + 1;
          puVar3 = puVar3 + 0xc;
        } while (iVar4 < DAT_00655494);
      }
      FUN_00479580();
      return 0;
    }
    iVar1 = 0;
    if (0 < DAT_00655494) {
      puVar5 = &DAT_00654e90;
      do {
        iVar2 = FUN_00479700(puVar5,3);
        if (iVar2 != 0) {
          uStack_c = (&DAT_00654eb0)[iVar1 * 0xc];
          uStack_8 = (&DAT_00654eb4)[iVar1 * 0xc];
          iVar4 = (&DAT_00654ebc)[iVar1 * 0xc];
          uStack_4 = (&DAT_00654eb8)[iVar1 * 0xc];
          break;
        }
        iVar1 = iVar1 + 1;
        puVar5 = puVar5 + 0x30;
      } while (iVar1 < DAT_00655494);
    }
    FUN_00479580();
    if (DAT_00655494 <= iVar1) {
      FUN_0041ee50(s_No_map_location_by_that_name_exi_005cbd58);
      return 4;
    }
  }
  else {
    iVar1 = FUN_0047a410(param_2,s__d__d__d_005cbda0,&uStack_c,&uStack_8,&uStack_4);
    if (iVar1 == 0) {
      return 4;
    }
  }
  FUN_00450d20(&uStack_c);
  DAT_00666970 = iVar4;
  if (iVar4 != DAT_00666974) {
    FUN_004546a0();
  }
  return 0;
}


