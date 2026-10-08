// FUN_004237b0 @ 004237b0 size=399

undefined4 FUN_004237b0(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  undefined4 uVar5;
  undefined2 auStack_8 [2];
  undefined2 auStack_4 [2];
  
  piVar4 = param_1;
  iVar1 = (**(code **)(*param_1 + 0x24))();
  iVar2 = param_2;
  if (iVar1 == 0) {
    FUN_0041ee50(s_Context_has_no_animator_005cb7c0);
    return 0;
  }
  iVar1 = FUN_00479700(s_RESET_005cb7dc,0);
  if (iVar1 == 0) {
    if (*(int *)(iVar2 + 0x10) == 8) {
      uVar3 = (uint)*(ushort *)(piVar4 + 3);
      iVar2 = FUN_0047a410(iVar2,s__i__i__i__i_005cb810,&param_1,&param_2,auStack_8,auStack_4);
      if (iVar2 == 0) {
        return 4;
      }
      iVar2 = FUN_0046e8a0();
      iVar2 = *(int *)(*(int *)(iVar2 + 4) + 0x54);
      if (uVar3 < *(uint *)(iVar2 + 4)) {
        iVar2 = iVar2 + 8 + uVar3 * 0x4c;
      }
      else {
        iVar2 = 0;
      }
      *(undefined2 *)(iVar2 + 0x30) = param_1._0_2_;
      *(undefined2 *)(iVar2 + 0x32) = (undefined2)param_2;
      *(undefined2 *)(iVar2 + 0x2c) = auStack_8[0];
      *(undefined2 *)(iVar2 + 0x2e) = auStack_4[0];
    }
    else {
      uVar3 = (uint)*(ushort *)(piVar4 + 3);
    }
    iVar2 = FUN_0046e8a0();
    iVar2 = *(int *)(*(int *)(iVar2 + 4) + 0x54);
    if (uVar3 < *(uint *)(iVar2 + 4)) {
      iVar2 = iVar2 + 8 + uVar3 * 0x4c;
    }
    else {
      iVar2 = 0;
    }
    FUN_0041ee50(s_Screen_extents_state__d__regx__d_005cb81c,uVar3,(int)*(short *)(iVar2 + 0x30),
                 (int)*(short *)(iVar2 + 0x32),(int)*(short *)(iVar2 + 0x2c),
                 (int)*(short *)(iVar2 + 0x2e));
    return 0;
  }
  FUN_0041ee50(s_Resetting_state_screen_extents_005cb7e4);
  FUN_00479580();
  param_1 = (int *)0xffffffff;
  if (*(int *)(iVar2 + 0x10) == 8) {
    FUN_0047a410(iVar2,&DAT_005cb804,&param_1);
  }
  iVar2 = FUN_00479700(s_FRONT_005cb808,0);
  if (iVar2 != 0) {
    FUN_00479580();
  }
  uVar3 = (uint)(iVar2 != 0);
  uVar5 = param_1;
  (**(code **)(*piVar4 + 0x24))(piVar4,param_1,uVar3);
  FUN_0040e110(piVar4,uVar5,uVar3);
  return 0;
}


