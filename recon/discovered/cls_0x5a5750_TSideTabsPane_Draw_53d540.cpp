// FUN_0053d540 @ 0053d540 size=409

void __fastcall FUN_0053d540(int param_1)

{
  int iVar1;
  int iVar2;
  int *piStack_5c;
  int iStack_58;
  undefined4 uStack_54;
  undefined4 uStack_50;
  undefined4 uStack_4c;
  undefined4 uStack_48;
  undefined4 uStack_44;
  undefined4 uStack_40;
  undefined4 uStack_3c;
  undefined4 uStack_38;
  undefined4 uStack_34;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined2 uStack_4;
  undefined2 uStack_2;
  
  if (DAT_006680c8 == 0) {
    if (DAT_005d7a18 == 0) {
      FUN_004aacb0(*(undefined4 *)(param_1 + 4),*(undefined4 *)(param_1 + 8),
                   *(undefined4 *)(param_1 + 0xc),*(undefined4 *)(param_1 + 0x10),1);
      return;
    }
  }
  else {
    iVar2 = *(int *)(param_1 + 0x18c);
    FUN_00438df0(0,0,iVar2,0,0,*(undefined4 *)(iVar2 + 4),*(undefined4 *)(iVar2 + 8),0x100,0,0);
    FUN_00436900(0);
    iStack_58 = 0;
    if (0 < *(int *)(param_1 + 0x88)) {
      piStack_5c = (int *)(param_1 + 0x1ac);
      do {
        iVar2 = (int)(*piStack_5c * 0xff + (*piStack_5c * 0xff >> 0x1f & 7U)) >> 3;
        if ((0 < iVar2) && (iVar1 = FUN_00436900(iStack_58), 0x80 < iVar2)) {
          uStack_20 = *(undefined4 *)(iVar1 + 0x6c);
          uStack_24 = *(undefined4 *)(iVar1 + 0x68);
          uStack_2c = *(undefined4 *)(iVar1 + 0x60);
          uStack_28 = *(undefined4 *)(iVar1 + 100);
          uStack_54 = 0x100;
          uStack_50 = 0;
          uStack_4c = 0;
          uStack_48 = 0;
          uStack_2 = 0;
          uStack_4 = 0;
          uStack_40 = 0;
          uStack_44 = 0;
          uStack_30 = 0;
          uStack_34 = 0;
          uStack_38 = 0;
          uStack_3c = 0;
          uStack_8 = 0x1f;
          uStack_c = 0;
          uStack_1c = uStack_2c;
          uStack_18 = uStack_28;
          uStack_14 = uStack_24;
          uStack_10 = uStack_20;
          (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
                    (&uStack_54,*(undefined4 *)(param_1 + 400),0,0);
        }
        iStack_58 = iStack_58 + 1;
        piStack_5c = piStack_5c + 1;
      } while (iStack_58 < *(int *)(param_1 + 0x88));
    }
    if (DAT_005d7a18 == 0) {
      FUN_004aacb0(*(undefined4 *)(param_1 + 4),*(undefined4 *)(param_1 + 8),
                   *(undefined4 *)(param_1 + 0xc),*(undefined4 *)(param_1 + 0x10),1);
    }
  }
  return;
}


