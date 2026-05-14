// FUN_0041b770_TArea_Pulse @ 0041b770 size=631

void __fastcall FUN_0041b770_TArea_Pulse(int param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined4 local_54;
  undefined1 local_50 [4];
  int local_4c [4];
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined2 local_2a;
  int local_28;
  int local_24;
  int local_20;
  
  if (((*(byte *)(param_1 + 0x44) & 0x20) != 0) && (0 < *(int *)(param_1 + 0x1bc))) {
    FUN_0041b240_TArea_PlayCDMusic();
  }
  iVar1 = FUN_0047eab0();
  if ((*(byte *)(param_1 + 0x44) & 8) != 0) {
    if (((iVar1 != DAT_005e91e8) && (iVar1 != 2)) && (iVar1 != 5)) {
      local_4c[0] = FUN_0049d800(s_BASEMIDNIGHT_005c6c8c);
      local_4c[1] = FUN_0049d800(s_BASEMORNING_005c6c9c);
      local_4c[2] = FUN_0049d800(s_BASEDAYTIME_005c6ca8);
      local_4c[3] = FUN_0049d800(s_BASENOON_005c6cb4);
      local_3c = FUN_0049d800(s_BASEEVENING_005c6cc0);
      local_38 = FUN_0049d800(s_BASENIGHT_005c6ccc);
      iVar2 = FUN_0049d6d0(s_FULLBASEITWAS_005c6cd8);
      if (iVar2 < 0) {
        iVar2 = local_4c[iVar1];
        uVar3 = FUN_0049d800(s_BASEITWAS_005c6cf8);
        FUN_0054d170(&DAT_0065c5d0,s__s__s_005c6d04,uVar3,iVar2);
      }
      else {
        iVar2 = local_4c[iVar1];
        uVar3 = FUN_0049d800(s_FULLBASEITWAS_005c6ce8);
        FUN_0054d170(&DAT_0065c5d0,uVar3,iVar2);
      }
    }
    iVar2 = FUN_0047ea00();
    if (iVar2 != *(int *)(param_1 + 0xe0)) {
      FUN_0041b3c0_TArea_GetCurrentAmbient(&local_54,local_50);
      FUN_00453640(local_54,0);
      FUN_004536b0(local_50,0);
      *(int *)(param_1 + 0xe0) = iVar2;
    }
  }
  iVar2 = *(int *)(param_1 + 0x25c);
  if (iVar2 != 0) {
    if ((*(int *)(iVar2 + 0x1c) == 0) && (*(int *)(iVar2 + 0x20) != 0)) {
      puVar6 = &local_34;
      for (iVar5 = 0xd; iVar5 != 0; iVar5 = iVar5 + -1) {
        *puVar6 = 0;
        puVar6 = puVar6 + 1;
      }
      local_34._0_2_ = 0x19;
      local_34._2_2_ = FUN_00475210(iVar2,0);
      local_28 = DAT_00666988;
      local_2a = (undefined2)DAT_00666970;
      local_24 = DAT_0066698c;
      local_20 = DAT_00666990;
      uVar3 = FUN_00450e40_TMapPane_NewObject(&local_34,0xffffffff);
      piVar4 = (int *)FUN_00452690_TMapPane_GetInstance(uVar3,0);
      *(int **)(*(int *)(param_1 + 0x25c) + 0x1c) = piVar4;
      if (piVar4 != (int *)0x0) {
        (**(code **)(*piVar4 + 0x40))(piVar4[2] | 0x80000);
      }
    }
    if (*(int *)(*(int *)(param_1 + 0x25c) + 0x1c) != 0) {
      local_4c[0] = DAT_00666988 + -500;
      local_4c[1] = DAT_0066698c + -500;
      local_4c[2] = DAT_00666990 + -600;
      (**(code **)(**(int **)(*(int *)(param_1 + 0x25c) + 0x1c) + 8))(local_4c,0xffffffff,0);
    }
  }
  DAT_005e91e8 = iVar1;
  return;
}


