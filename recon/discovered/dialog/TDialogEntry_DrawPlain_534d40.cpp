// FUN_00534d40 @ 00534d40 size=642

void __fastcall FUN_00534d40(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  undefined *puVar3;
  int *piVar4;
  int *piVar5;
  int iStack_60;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined2 local_4;
  undefined2 local_2;
  
  if (*(int *)(param_1 + 0x20) != -10000) {
    iVar2 = (*(int *)(param_1 + 0x58) * 0xff) / 0xc;
    if (0x7f < iVar2) {
      if (DAT_006680c8 != 0) {
        local_2c = *(int *)(param_1 + 0x18) + *(int *)(param_1 + 0x20);
        local_20 = *(int *)(param_1 + 0x2c);
        local_28 = *(int *)(param_1 + 0x24) + *(int *)(param_1 + 0x1c);
        local_24 = 0x32;
        local_14 = 0x32;
        local_54 = 0x100;
        local_50 = 0;
        local_4c = 0;
        local_48 = 0;
        local_1c = 0;
        local_18 = 0;
        local_2 = 0;
        local_4 = 0;
        local_40 = 0;
        local_44 = 0;
        local_30 = 0;
        local_34 = 0;
        local_38 = 0;
        local_3c = 0;
        local_8 = 0x1f;
        local_c = 0;
        local_10 = local_20;
        (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
                  (&local_54,*(undefined4 *)(param_1 + 0x48),0,0);
        puVar3 = PTR_DAT_005d79e0;
        uVar1 = *(undefined4 *)(param_1 + 0x48);
        FUN_00438d80(&stack0xffffff9c,*(int *)(param_1 + 0x18) + 0x32 + *(int *)(param_1 + 0x20),
                     *(int *)(param_1 + 0x24) + *(int *)(param_1 + 0x1c),0x32,0,
                     *(int *)(param_1 + 0x28) + -0x32,*(undefined4 *)(param_1 + 0x2c),0x100);
        (**(code **)(*(int *)puVar3 + 0x5c))(&stack0xffffff9c,uVar1,0,0);
        iStack_60 = 0;
        if (0 < *(int *)(param_1 + 0x5c)) {
          piVar5 = (int *)(param_1 + 0x140);
          piVar4 = (int *)(param_1 + 0x80);
          do {
            if ((piVar5[-0x10] != 0) &&
               (0x7f < (((int)(*piVar5 * 0xff + (*piVar5 * 0xff >> 0x1f & 7U)) >> 3) * iVar2) / 0xff
               )) {
              local_20 = (piVar4[3] - piVar4[1]) + 1;
              local_2c = *piVar4 + *(int *)(param_1 + 0x18) + *(int *)(param_1 + 0x20);
              local_24 = (piVar4[2] - *piVar4) + 1;
              local_28 = piVar4[1] + *(int *)(param_1 + 0x24) + *(int *)(param_1 + 0x1c);
              local_1c = *piVar4;
              local_18 = piVar4[1];
              local_54 = 0x100;
              local_50 = 0;
              local_4c = 0;
              local_48 = 0;
              local_2 = 0;
              local_4 = 0;
              local_40 = 0;
              local_44 = 0;
              local_30 = 0;
              local_34 = 0;
              local_38 = 0;
              local_3c = 0;
              local_8 = 0x1f;
              local_c = 0;
              local_14 = local_24;
              local_10 = local_20;
              (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
                        (&local_54,*(undefined4 *)(param_1 + 0x4c),0,0);
            }
            iStack_60 = iStack_60 + 1;
            piVar5 = piVar5 + 1;
            piVar4 = piVar4 + 4;
          } while (iStack_60 < *(int *)(param_1 + 0x5c));
        }
      }
      if (DAT_005d7a18 == 0) {
        FUN_004aacb0(*(int *)(param_1 + 0x18) + *(int *)(param_1 + 0x20),
                     *(int *)(param_1 + 0x24) + *(int *)(param_1 + 0x1c),
                     *(undefined4 *)(param_1 + 0x28),*(undefined4 *)(param_1 + 0x2c),1);
      }
    }
  }
  return;
}


