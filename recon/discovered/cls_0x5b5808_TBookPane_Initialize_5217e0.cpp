// FUN_005217e0_TBookPane_Initialize @ 005217e0 size=568

undefined4 __fastcall FUN_005217e0_TBookPane_Initialize(int param_1)

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
  
  FUN_00434e40_TButtonPane_Initialize();
  *(undefined4 *)(param_1 + 400) = 0xd8;
  *(undefined4 *)(param_1 + 0x188) = 0;
  *(undefined4 *)(param_1 + 0x18c) = 0;
  *(undefined4 *)(param_1 + 0x194) = 300;
  uVar1 = FUN_0047f670(s_book_dat_005e2cd4,0xffffffff,0);
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
  iVar5 = *(int *)(DAT_0065b020 + DAT_0065b7d8 * 4);
  if (iVar5 == 0) {
    iVar5 = DAT_0065b024;
  }
  iVar3 = FUN_00521c60(DAT_0065b7d8);
  DAT_0066dac8 = (undefined4)(300 / (longlong)(*(int *)(iVar3 + 0x54) + *(int *)(iVar5 + 0x50)));
  if (*(int *)(param_1 + 0x180) != 0) {
    piVar4 = (int *)FUN_0046d710(&DAT_005e2ce0);
    iVar5 = (0x1e0 - piVar4[1]) / 3;
    iVar3 = (0x280 - *piVar4) / 2;
    uVar1 = FUN_0046d710(s_downdown_005e2cf0);
    uVar12 = 0;
    uVar10 = 0;
    uVar8 = 0;
    uVar6 = 0;
    uVar13 = 0xffffffff;
    uVar11 = 0;
    uVar9 = 0;
    uVar7 = 0;
    uVar2 = FUN_0046d710(s_downup_005e2ce8);
    FUN_00444e30(&DAT_005e2cfc,iVar3 + 0x1b8,iVar5 + 0x182,0x1c,0x16,0x22,&LAB_00521780,uVar1,uVar2,
                 uVar7,uVar9,uVar11,uVar13,uVar6,uVar8,uVar10,uVar12);
    uVar1 = FUN_0046d710(s_updown_005e2d0c);
    uVar12 = 0;
    uVar10 = 0;
    uVar8 = 0;
    uVar6 = 0;
    uVar13 = 0xffffffff;
    uVar11 = 0;
    uVar9 = 0;
    uVar7 = 0;
    uVar2 = FUN_0046d710(&DAT_005e2d04);
    FUN_00444e30(&DAT_005e2d14,iVar3 + 0x1d8,iVar5 + 0x17b,0x1c,0x17,0x21,&LAB_00521730,uVar1,uVar2,
                 uVar7,uVar9,uVar11,uVar13,uVar6,uVar8,uVar10,uVar12);
    uVar1 = FUN_0046d710(s_exitdown_005e2d20);
    uVar12 = 0;
    uVar10 = 0;
    uVar8 = 0;
    uVar6 = 0;
    uVar13 = 0xffffffff;
    uVar11 = 0;
    uVar9 = 0;
    uVar7 = 0;
    uVar2 = FUN_0046d710(s_exitup_005e2d18);
    FUN_00444e30(&DAT_005e2d2c,iVar3 + 0x1f9,iVar5 + 0x179,0x1c,0x1e,0x1b,&LAB_005217d0,uVar1,uVar2,
                 uVar7,uVar9,uVar11,uVar13,uVar6,uVar8,uVar10,uVar12);
    return 1;
  }
  return 0;
}


