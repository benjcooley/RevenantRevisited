// FUN_0051b960_Load @ 0051b960 size=960

void __thiscall FUN_0051b960_Load(int param_1,int param_2,undefined4 param_3,uint param_4)

{
  byte bVar1;
  undefined1 *puVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  undefined4 *puVar9;
  undefined8 uVar10;
  uint uVar11;
  
  iVar4 = param_2;
  uVar11 = param_4;
  if (3 < (int)param_4) {
    bVar1 = **(byte **)(param_2 + 4);
    *(byte **)(param_2 + 4) = *(byte **)(param_2 + 4) + 1;
    uVar11 = (uint)bVar1;
  }
  FUN_004d4eb0(param_2,param_3,uVar11);
  if (3 < (int)param_4) {
    FUN_0049ce00(param_1 + 0x2cc);
    FUN_0049ce00(param_1 + 0x2d2);
    FUN_0049ce00(param_1 + 0x2d8);
    FUN_0049ce00(param_1 + 0x2de);
    FUN_0049ce00(param_1 + 0x2e4);
  }
  if (4 < (int)param_4) {
    iVar6 = **(int **)(param_2 + 4);
    *(int **)(param_2 + 4) = *(int **)(param_2 + 4) + 1;
    param_2 = iVar6;
    if (0 < iVar6) {
      do {
        iVar5 = FUN_00482fb0(6);
        iVar8 = 0;
        do {
          puVar2 = *(undefined1 **)(iVar4 + 4);
          *(undefined1 *)(iVar8 + iVar5) = *puVar2;
          iVar8 = iVar8 + 1;
          *(undefined1 **)(iVar4 + 4) = puVar2 + 1;
        } while (iVar8 < 6);
        FUN_0041c840(iVar5);
        param_2 = param_2 + -1;
      } while (param_2 != 0);
    }
    if (((5 < (int)param_4) && ((int)param_4 < 9)) && (0 < iVar6)) {
      do {
        iVar6 = iVar6 + -1;
        *(int *)(iVar4 + 4) = *(int *)(iVar4 + 4) + 4;
      } while (iVar6 != 0);
    }
  }
  if (6 < (int)param_4) {
    puVar9 = *(undefined4 **)(iVar4 + 4);
    *(undefined4 *)(param_1 + 0x304) = *puVar9;
    *(undefined4 *)(param_1 + 0x308) = puVar9[1];
    *(undefined4 *)(param_1 + 0x30c) = puVar9[2];
    *(undefined4 *)(param_1 + 0x310) = puVar9[3];
    *(undefined4 **)(iVar4 + 4) = puVar9 + 4;
  }
  if ((int)param_4 < 8) {
    *(undefined4 *)(param_1 + 0x364) = 0;
    *(undefined4 *)(param_1 + 0x360) = 0;
    *(undefined4 *)(param_1 + 0x368) = 0;
  }
  else {
    puVar9 = *(undefined4 **)(iVar4 + 4);
    *(undefined4 *)(param_1 + 0x360) = *puVar9;
    *(undefined4 *)(param_1 + 0x364) = puVar9[1];
    *(undefined4 *)(param_1 + 0x368) = puVar9[2];
    *(undefined4 **)(iVar4 + 4) = puVar9 + 3;
  }
  if ((int)param_4 < 0xd) {
    if ((int)param_4 < 0xb) {
      if ((int)param_4 < 10) {
        *(undefined1 *)(param_1 + 0x494) = 0;
      }
      else {
        iVar6 = 0;
        do {
          puVar2 = *(undefined1 **)(iVar4 + 4);
          *(undefined1 *)(param_1 + 0x494 + iVar6) = *puVar2;
          iVar6 = iVar6 + 1;
          *(undefined1 **)(iVar4 + 4) = puVar2 + 1;
        } while (iVar6 < 0x32);
      }
      *(undefined1 *)(param_1 + 0x4c6) = 0;
      *(undefined4 *)(param_1 + 0x4d8) = 0xffffffff;
      *(undefined4 *)(param_1 + 0x490) = 0;
      *(undefined4 *)(param_1 + 0x4dc) = 0;
      *(undefined1 *)(param_1 + 0x4f0) = 0;
    }
    else {
      FUN_0049ce00(param_1 + 0x494);
      FUN_0049ce00(param_1 + 0x4c6);
      uVar7 = **(undefined4 **)(iVar4 + 4);
      *(undefined4 **)(iVar4 + 4) = *(undefined4 **)(iVar4 + 4) + 1;
      *(undefined4 *)(param_1 + 0x490) = 0;
      *(undefined4 *)(param_1 + 0x4dc) = 0;
      *(undefined4 *)(param_1 + 0x4d8) = uVar7;
      FUN_0049ce00(param_1 + 0x4f0);
    }
  }
  else {
    FUN_0049ce00(param_1 + 0x494);
    FUN_0049ce00(param_1 + 0x4c6);
    puVar9 = *(undefined4 **)(iVar4 + 4);
    uVar7 = puVar9[1];
    *(undefined4 *)(param_1 + 0x4d8) = *puVar9;
    uVar3 = puVar9[2];
    *(undefined4 *)(param_1 + 0x490) = uVar7;
    *(undefined4 *)(param_1 + 0x4dc) = uVar3;
    *(undefined4 **)(iVar4 + 4) = puVar9 + 3;
    FUN_0049ce00(param_1 + 0x4f0);
  }
  if ((int)param_4 < 0xd) {
    *(undefined4 *)(param_1 + 0x36c) = 0;
    uVar7 = FUN_0047e940();
    *(undefined4 *)(param_1 + 0x370) = uVar7;
    uVar7 = FUN_0047e920_PlayScreen_GameFrame();
    *(undefined4 *)(param_1 + 0x374) = uVar7;
    *(undefined1 *)(param_1 + 0x378) = 0;
    puVar9 = (undefined4 *)(param_1 + 0x570);
    for (iVar6 = 0x38; iVar6 != 0; iVar6 = iVar6 + -1) {
      *puVar9 = 0;
      puVar9 = puVar9 + 1;
    }
    *(undefined4 *)(param_1 + 0x65c) = 0;
    *(undefined4 *)(param_1 + 0x658) = 0;
    *(undefined4 *)(param_1 + 0x654) = 0;
    *(undefined4 *)(param_1 + 0x650) = 0;
  }
  else {
    puVar9 = *(undefined4 **)(iVar4 + 4);
    uVar7 = *puVar9;
    iVar6 = puVar9[1];
    *(int *)(param_1 + 0x370) = iVar6;
    *(undefined4 **)(iVar4 + 4) = puVar9 + 2;
    *(undefined4 *)(param_1 + 0x36c) = uVar7;
    uVar10 = __allmul(iVar6,iVar6 >> 0x1f,0x18,0);
    uVar7 = __alldiv(uVar10,100,0);
    *(undefined4 *)(param_1 + 0x374) = uVar7;
    iVar6 = param_1 + 0x5d0;
    iVar5 = param_1 + 0x590;
    iVar8 = param_1 + 0x570;
    FUN_0049ce00(param_1 + 0x378);
    FUN_0049ce00(iVar8);
    FUN_0049ce00(iVar5);
    FUN_0049ce00(iVar6);
    puVar9 = *(undefined4 **)(iVar4 + 4);
    uVar7 = puVar9[1];
    *(undefined4 *)(param_1 + 0x650) = *puVar9;
    *(undefined4 *)(param_1 + 0x654) = uVar7;
    *(undefined4 **)(iVar4 + 4) = puVar9 + 2;
    if ((int)param_4 < 0xf) {
      *(undefined4 *)(param_1 + 0x65c) = 0;
      *(undefined4 *)(param_1 + 0x658) = 0;
    }
    else {
      uVar7 = puVar9[3];
      *(undefined4 *)(param_1 + 0x658) = puVar9[2];
      *(undefined4 *)(param_1 + 0x65c) = uVar7;
      *(undefined4 **)(iVar4 + 4) = puVar9 + 4;
    }
  }
  if (0xd < (int)param_4) {
    FUN_00529770(iVar4,param_3,param_4);
  }
  *(undefined4 *)(param_1 + 0x668) = *(undefined4 *)(param_1 + 0x10);
  *(undefined4 *)(param_1 + 0x66c) = *(undefined4 *)(param_1 + 0x14);
  *(undefined4 *)(param_1 + 0x670) = *(undefined4 *)(param_1 + 0x18);
  FUN_0051c660_TPlayer_RefreshStats();
  return;
}


