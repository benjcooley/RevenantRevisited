// FUN_0052f7d0 @ 0052f7d0 size=645

void __thiscall FUN_0052f7d0(int *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  char *pcVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  int iVar9;
  undefined4 uVar10;
  undefined1 uStack_38;
  undefined1 uStack_37;
  undefined1 uStack_36;
  undefined1 uStack_35;
  undefined1 auStack_34 [52];
  
  uStack_36 = 0xff;
  uStack_37 = 0xba;
  uStack_38 = 0;
  uStack_35 = 0xff;
  iVar1 = FUN_00436900(0);
  if (iVar1 != 0) {
    piVar2 = (int *)FUN_00436900(0);
    (**(code **)(*piVar2 + 0x1c))(piVar2[5] | 0x20);
  }
  iVar1 = FUN_00436900(1);
  if (iVar1 != 0) {
    piVar2 = (int *)FUN_00436900(1);
    (**(code **)(*piVar2 + 0x1c))(piVar2[5] | 0x20);
  }
  iVar1 = FUN_00436900(2);
  if (iVar1 != 0) {
    piVar2 = (int *)FUN_00436900(2);
    (**(code **)(*piVar2 + 0x1c))(piVar2[5] | 0x20);
  }
  iVar1 = FUN_00436900(3);
  if (iVar1 != 0) {
    piVar2 = (int *)FUN_00436900(3);
    (**(code **)(*piVar2 + 0x1c))(piVar2[5] | 0x20);
  }
  if (param_1[100] != 0) {
    uVar3 = FUN_0046d710(s_BuySellMain_005e3d78);
    FUN_004bd680(0,0,uVar3,param_3,0);
    (**(code **)(*param_1 + 0x90))();
  }
  uVar3 = 0;
  if ((int *)param_1[0x6d] != (int *)0x0) {
    uVar3 = (**(code **)(*(int *)param_1[0x6d] + 0x84))(&DAT_005e3d84);
    uVar4 = FUN_0049d800(s_BSGOLD_005e3d8c);
    FUN_0058b100(auStack_34,s__s__d_005e3d94,uVar4,uVar3);
    uVar3 = extraout_ECX;
  }
  uVar10 = 0x441;
  uVar4 = param_3;
  FUN_00419dd0(&uStack_38);
  FUN_004be2b0(0x7d,0,0x62,0x2c,auStack_34,0,DAT_00667540,uVar3,uVar10,uVar4);
  iVar1 = param_1[0x60];
  if (iVar1 < (short)param_1[0x65]) {
    do {
      if (param_1[0x60] + param_1[0x61] <= iVar1) break;
      if (iVar1 == param_1[99]) {
        uVar3 = 1;
LAB_0052f96f:
        iVar9 = param_1[0x67];
      }
      else {
        if (iVar1 != param_1[0x62]) {
          uVar3 = 0;
          goto LAB_0052f96f;
        }
        iVar9 = param_1[0x67];
        uVar3 = 2;
      }
      FUN_0052f040(param_2,iVar9,param_3,iVar1,uVar3);
      iVar1 = iVar1 + 1;
    } while (iVar1 < (short)param_1[0x65]);
  }
  FUN_00435de0();
  uVar3 = extraout_ECX_00;
  uVar4 = param_3;
  if ((param_1[0x5f] & 1U) == 0) {
    if ((param_1[0x5f] & 2U) == 0) goto LAB_0052fa0e;
    uVar10 = 0x442;
    FUN_00419dd0(&uStack_38);
    pcVar6 = s_BSSELL_005e3da4;
  }
  else {
    uVar10 = 0x442;
    FUN_00419dd0(&uStack_38);
    pcVar6 = s_BSBUY_005e3d9c;
  }
  uVar7 = 0;
  uVar8 = DAT_00667540;
  uVar5 = FUN_0049d800(pcVar6);
  FUN_004be2b0(0,0,100,0x2c,uVar5,uVar7,uVar8,uVar3,uVar10,uVar4);
  uVar3 = extraout_ECX_01;
LAB_0052fa0e:
  uVar5 = 0x442;
  FUN_00419dd0(&uStack_38);
  uVar8 = 0;
  uVar4 = DAT_00667540;
  uVar10 = FUN_0049d800(s_BSEXIT_005e3dac);
  FUN_004be2b0(0x159,0,100,0x2c,uVar10,uVar8,uVar4,uVar3,uVar5,param_3);
  return;
}


