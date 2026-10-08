// FUN_0054c600 @ 0054c600 size=384

void __fastcall FUN_0054c600(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  iVar6 = 0;
  if (DAT_006680c8 == 0) {
    uStack_10 = *(int *)(param_1 + 0x7c);
    uStack_14 = -1;
    uStack_8 = -1;
    uStack_4 = 0;
    if (0 < *(int *)(param_1 + 0x60)) {
      uStack_c = 0;
      iVar3 = -1;
      iVar1 = (*(int *)(param_1 + 0x10) + *(int *)(param_1 + 8)) - *(int *)(param_1 + 0x78);
      do {
        iVar5 = iVar1;
        iVar2 = uStack_10;
        iVar1 = *(int *)(uStack_c + 8 + *(int *)(param_1 + 0x6c));
        if (iVar1 < 0x18) {
          iVar4 = (iVar1 * 0xff) / 0x18;
        }
        else {
          iVar4 = 0xff;
        }
        if ((iVar3 != -1) && ((iVar4 != iVar3 || (uStack_14 == 0)))) {
          FUN_00414d70(0,uStack_8,1,*(undefined4 *)(param_1 + 0x8c),0,*(undefined4 *)(param_1 + 0xc)
                       ,iVar6,iVar3 << 0x18 | 0xffffff,0,uStack_14,*(undefined4 *)(param_1 + 0xc),
                       iVar6,0,(-(iVar3 != 0xff) & 2U) + 2);
          iVar6 = 0;
        }
        uStack_14 = uStack_10;
        iVar1 = *(int *)(param_1 + 0x78);
        iVar6 = iVar6 + iVar1;
        uStack_10 = uStack_10 - iVar1;
        if (uStack_10 < 0) {
          uStack_10 = uStack_10 + *(int *)(*(int *)(param_1 + 0x8c) + 8);
        }
        uStack_c = uStack_c + 0x5c;
        uStack_4 = uStack_4 + 1;
        iVar3 = iVar4;
        iVar1 = iVar5 - iVar1;
        uStack_8 = iVar5;
      } while (uStack_4 < *(int *)(param_1 + 0x60));
      if (iVar4 != -1) {
        FUN_00414d70(0,iVar5,1,*(undefined4 *)(param_1 + 0x8c),0,*(undefined4 *)(param_1 + 0xc),
                     iVar6,iVar4 << 0x18 | 0xffffff,0,iVar2,*(undefined4 *)(param_1 + 0xc),iVar6,0,
                     (-(iVar4 != 0xff) & 2U) + 2);
      }
    }
    *(undefined4 *)(param_1 + 0x94) = 0;
  }
  return;
}


