// FUN_00534b60 @ 00534b60 size=478

void __fastcall FUN_00534b60(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int *piVar6;
  int local_8;
  
  if ((DAT_006680c8 == 0) && (*(int *)(param_1 + 0x20) != -10000)) {
    iVar3 = (*(int *)(param_1 + 0x54) * 0xff) / 0xc;
    if (0 < iVar3) {
      FUN_00414d70(*(int *)(param_1 + 0x18) + *(int *)(param_1 + 0x20),
                   *(int *)(param_1 + 0x24) + *(int *)(param_1 + 0x1c),1,
                   *(undefined4 *)(param_1 + 0x48),0,0x32,*(undefined4 *)(param_1 + 0x2c),
                   iVar3 * 0x1000000 | 0xffffff,0,0,0x32,*(undefined4 *)(param_1 + 0x2c),0,4);
      iVar4 = *(int *)(param_1 + 0x28) + -0x32;
      FUN_00414d70(*(int *)(param_1 + 0x18) + 0x32 + *(int *)(param_1 + 0x20),
                   *(int *)(param_1 + 0x24) + *(int *)(param_1 + 0x1c),1,
                   *(undefined4 *)(param_1 + 0x48),0,iVar4,*(undefined4 *)(param_1 + 0x2c),
                   (((uint)*(byte *)(param_1 + 0xe) | iVar3 * 0x100) << 8 |
                   (uint)*(byte *)(param_1 + 0xd)) << 8 | (uint)*(byte *)(param_1 + 0xc),0x32,0,
                   iVar4,*(undefined4 *)(param_1 + 0x2c),0,4);
      local_8 = 0;
      if (0 < *(int *)(param_1 + 0x5c)) {
        piVar5 = (int *)(param_1 + 0x120);
        piVar6 = (int *)(param_1 + 0x84);
        do {
          if (piVar5[-8] != 0) {
            iVar4 = (((int)(*piVar5 * 0xff + (*piVar5 * 0xff >> 0x1f & 7U)) >> 3) * iVar3) / 0xff;
            if (0 < iVar4) {
              iVar1 = *piVar6;
              iVar2 = piVar6[-1];
              FUN_00414d70(iVar2 + *(int *)(param_1 + 0x18) + *(int *)(param_1 + 0x20),
                           *(int *)(param_1 + 0x24) + iVar1 + *(int *)(param_1 + 0x1c),1,
                           *(undefined4 *)(param_1 + 0x48),0,(piVar6[1] - iVar2) + 1,
                           (piVar6[2] - iVar1) + 1,
                           (((uint)*(byte *)(param_1 + 0x12) | iVar4 * 0x100) << 8 |
                           (uint)*(byte *)(param_1 + 0x11)) << 8 | (uint)*(byte *)(param_1 + 0x10),
                           iVar2,iVar1,(piVar6[1] - iVar2) + 1,(piVar6[2] - iVar1) + 1,0,4);
            }
          }
          local_8 = local_8 + 1;
          piVar5 = piVar5 + 1;
          piVar6 = piVar6 + 4;
        } while (local_8 < *(int *)(param_1 + 0x5c));
      }
    }
  }
  return;
}


