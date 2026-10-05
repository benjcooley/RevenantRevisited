// FUN_0054c780 @ 0054c780 size=561

void __fastcall FUN_0054c780(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined *puVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iStack_74;
  int iStack_70;
  int iStack_6c;
  int iStack_68;
  int iStack_64;
  int iStack_60;
  int iStack_5c;
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
  int iStack_28;
  undefined4 uStack_24;
  int iStack_20;
  undefined4 uStack_1c;
  int iStack_18;
  undefined4 uStack_14;
  int iStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined2 uStack_4;
  undefined2 uStack_2;
  
  iVar4 = DAT_005d7a18;
  if (DAT_006680c8 != 0) {
    iStack_6c = -1;
    iStack_74 = -1;
    iStack_5c = -1;
    iVar4 = 0;
    iStack_60 = 0;
    if (0 < *(int *)(param_1 + 0x60)) {
      iStack_64 = 0;
      iVar1 = *(int *)(param_1 + 0x10) - *(int *)(param_1 + 0x78);
      iVar5 = *(int *)(param_1 + 0x7c);
      do {
        iStack_68 = iVar5;
        iVar6 = iVar1;
        puVar3 = PTR_DAT_005d79e0;
        iVar1 = *(int *)(*(int *)(param_1 + 0x6c) + 8 + iStack_64);
        if (iVar1 < 0x18) {
          iStack_70 = (iVar1 * 0xff) / 0x18;
        }
        else {
          iStack_70 = 0xff;
        }
        if ((iStack_6c != -1) && ((iStack_70 != iStack_6c || (iStack_74 == 0)))) {
          if (0x80 < iStack_6c) {
            uVar2 = *(undefined4 *)(param_1 + 0x8c);
            FUN_00438d80(&uStack_54,0,iStack_5c,0,iStack_74,*(undefined4 *)(param_1 + 0xc),iVar4,
                         0x120);
            (**(code **)(*(int *)puVar3 + 0x5c))(&uStack_54,uVar2,0,0);
          }
          iVar4 = 0;
        }
        iVar1 = *(int *)(param_1 + 0x78);
        iVar4 = iVar4 + iVar1;
        iVar5 = iStack_68 - iVar1;
        if (iVar5 < 0) {
          iVar5 = iVar5 + *(int *)(*(int *)(param_1 + 0x8c) + 8);
        }
        iStack_60 = iStack_60 + 1;
        iStack_64 = iStack_64 + 0x5c;
        iVar1 = iVar6 - iVar1;
        iStack_74 = iStack_68;
        iStack_6c = iStack_70;
        iStack_5c = iVar6;
      } while (iStack_60 < *(int *)(param_1 + 0x60));
      if ((iStack_70 != -1) && (0x80 < iStack_70)) {
        uStack_24 = *(undefined4 *)(param_1 + 0xc);
        uStack_50 = 0;
        uStack_4c = 0;
        uStack_48 = 0;
        uStack_2c = 0;
        uStack_1c = 0;
        uStack_2 = 0;
        uStack_4 = 0;
        uStack_40 = 0;
        uStack_44 = 0;
        uStack_30 = 0;
        uStack_34 = 0;
        uStack_38 = 0;
        uStack_3c = 0;
        uStack_c = 0;
        uStack_54 = 0x120;
        uStack_8 = 0x1f;
        iStack_28 = iVar6;
        iStack_20 = iVar4;
        iStack_18 = iStack_68;
        uStack_14 = uStack_24;
        iStack_10 = iVar4;
        (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
                  (&uStack_54,*(undefined4 *)(param_1 + 0x8c),0,0);
      }
    }
    iVar4 = DAT_005d7a18;
    *(undefined4 *)(param_1 + 0x94) = 0;
  }
  if (iVar4 == 0) {
    FUN_004aacb0(*(undefined4 *)(param_1 + 4),*(undefined4 *)(param_1 + 8),
                 *(undefined4 *)(param_1 + 0xc),*(undefined4 *)(param_1 + 0x10),1);
  }
  return;
}


