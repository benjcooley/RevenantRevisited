// FUN_00521080 @ 00521080 size=563

undefined4 __fastcall FUN_00521080(int param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  
  FUN_00434e40();
  *(undefined4 *)(param_1 + 400) = 0x1ae;
  *(undefined4 *)(param_1 + 0x188) = 0;
  *(undefined4 *)(param_1 + 0x18c) = 0;
  *(undefined4 *)(param_1 + 0x194) = 0xe6;
  uVar1 = FUN_0047f670(s_scroll_dat_005e2c6c,0xffffffff,0);
  *(undefined4 *)(param_1 + 0x180) = uVar1;
  if ((*(int *)(param_1 + 0x17c) == 0) ||
     (iVar5 = *(int *)(*(int *)(param_1 + 0x17c) + 0xd8), iVar5 == 0)) {
    *(undefined4 *)(param_1 + 0x18c) = 0;
  }
  else {
    uVar1 = *(undefined4 *)(param_1 + 400);
    uVar13 = 0;
    uVar11 = 10000;
    uVar9 = 0;
    uVar7 = 1;
    uVar2 = FUN_0049d800(iVar5);
    uVar1 = FUN_004acb80(DAT_0065c5c0,uVar2,uVar1,uVar7,uVar9,uVar11,uVar13);
    *(undefined4 *)(param_1 + 0x18c) = uVar1;
  }
  iVar5 = *(int *)(DAT_0065b020 + DAT_0065c5c0 * 4);
  if (iVar5 == 0) {
    iVar5 = DAT_0065b024;
  }
  iVar3 = FUN_00521c60(DAT_0065c5c0);
  DAT_0066dacc = (undefined4)(0xe6 / (longlong)(*(int *)(iVar3 + 0x54) + *(int *)(iVar5 + 0x50)));
  if (*(int *)(param_1 + 0x180) != 0) {
    piVar4 = (int *)FUN_0046d710(s_scroll_005e2c78);
    iVar3 = (0x280 - *piVar4) / 2;
    iVar5 = (int)((0x1e0 - piVar4[1]) + (0x1e0 - piVar4[1] >> 0x1f & 3U)) >> 2;
    uVar1 = FUN_0046d710(s_downdown_005e2c88);
    uVar12 = 0;
    uVar10 = 0;
    uVar8 = 0;
    uVar6 = 0;
    uVar13 = 0xffffffff;
    uVar11 = 0;
    uVar9 = 0;
    uVar7 = 0;
    uVar2 = FUN_0046d710(s_downup_005e2c80);
    FUN_00444e30(&DAT_005e2c94,iVar3 + 0x1c4,iVar5 + 0x14a,0x1c,0x1f,0x22,&LAB_00521030,uVar1,uVar2,
                 uVar7,uVar9,uVar11,uVar13,uVar6,uVar8,uVar10,uVar12);
    uVar1 = FUN_0046d710(s_updown_005e2ca4);
    uVar12 = 0;
    uVar10 = 0;
    uVar8 = 0;
    uVar6 = 0;
    uVar13 = 0xffffffff;
    uVar11 = 0;
    uVar9 = 0;
    uVar7 = 0;
    uVar2 = FUN_0046d710(&DAT_005e2c9c);
    FUN_00444e30(&DAT_005e2cac,iVar3 + 0x1e4,iVar5 + 0x143,0x1c,0x1f,0x21,&LAB_00520ff0,uVar1,uVar2,
                 uVar7,uVar9,uVar11,uVar13,uVar6,uVar8,uVar10,uVar12);
    uVar1 = FUN_0046d710(s_exitdown_005e2cb8);
    uVar12 = 0;
    uVar10 = 0;
    uVar8 = 0;
    uVar6 = 0;
    uVar13 = 0xffffffff;
    uVar11 = 0;
    uVar9 = 0;
    uVar7 = 0;
    uVar2 = FUN_0046d710(s_exitup_005e2cb0);
    FUN_00444e30(&DAT_005e2cc4,iVar3 + 0x206,iVar5 + 0x141,0x1c,0x1f,0x1b,&LAB_00521070,uVar1,uVar2,
                 uVar7,uVar9,uVar11,uVar13,uVar6,uVar8,uVar10,uVar12);
    return 1;
  }
  return 0;
}


