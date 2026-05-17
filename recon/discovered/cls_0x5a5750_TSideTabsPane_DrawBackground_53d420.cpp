// FUN_0053d420_TSideTabsPane_DrawBackground @ 0053d420 size=278

void __fastcall FUN_0053d420_TSideTabsPane_DrawBackground(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 uStack_8;
  int *piStack_4;
  
  if (DAT_006680c8 == 0) {
    iVar3 = *(int *)(param_1 + 0x18c);
    FUN_00414d70(*(undefined4 *)(param_1 + 4),*(undefined4 *)(param_1 + 8),1,iVar3,0,
                 *(undefined4 *)(iVar3 + 4),*(undefined4 *)(iVar3 + 8),0xffffffff,0,0,
                 *(undefined4 *)(iVar3 + 4),*(undefined4 *)(iVar3 + 8),0,0);
    FUN_00436900(0);
    iVar3 = 0;
    if (0 < *(int *)(param_1 + 0x88)) {
      piStack_4 = (int *)(param_1 + 0x1ac);
      do {
        iVar2 = (int)(*piStack_4 * 0xff + (*piStack_4 * 0xff >> 0x1f & 7U)) >> 3;
        if (0 < iVar2) {
          iVar1 = FUN_00436900(iVar3);
          uStack_8 = CONCAT13((char)iVar2,0xffffff);
          FUN_00414d70(*(int *)(iVar1 + 0x60) + *(int *)(param_1 + 4),
                       *(int *)(param_1 + 8) + *(int *)(iVar1 + 100),1,
                       *(undefined4 *)(param_1 + 400),0,*(undefined4 *)(iVar1 + 0x68),
                       *(undefined4 *)(iVar1 + 0x6c),uStack_8,*(int *)(iVar1 + 0x60),
                       *(int *)(iVar1 + 100),*(undefined4 *)(iVar1 + 0x68),
                       *(undefined4 *)(iVar1 + 0x6c),0,4);
        }
        iVar3 = iVar3 + 1;
        piStack_4 = piStack_4 + 1;
      } while (iVar3 < *(int *)(param_1 + 0x88));
    }
  }
  return;
}


