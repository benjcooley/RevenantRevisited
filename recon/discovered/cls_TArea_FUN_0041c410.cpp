// FUN_0041c410_TAreaMgr_Pulse @ 0041c410 size=467

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0041c410_TAreaMgr_Pulse(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  int local_14;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  FUN_0041c750(&DAT_0066697c);
  uVar2 = DAT_00666970;
  iVar5 = 0;
  local_14 = -1;
  if (0 < *(int *)(param_1 + 4)) {
    do {
      if ((iVar5 == 0) || (iVar3 = FUN_0041b0b0_TArea_In(&local_c,uVar2), iVar3 != 0)) {
        local_14 = iVar5;
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < *(int *)(param_1 + 4));
    if ((-1 < local_14) &&
       ((*(byte *)(*(int *)(*(int *)(param_1 + 0x14) + local_14 * 4) + 0x44) & 2) == 0)) {
      iVar5 = 0;
      if (0 < *(int *)(param_1 + 4)) {
        do {
          iVar3 = *(int *)(*(int *)(param_1 + 0x14) + iVar5 * 4);
          if (((*(byte *)(iVar3 + 0x44) & 2) != 0) && ((*(uint *)(iVar3 + 0x44) & 2) != 0)) {
            *(uint *)(iVar3 + 0x44) = *(uint *)(iVar3 + 0x44) & 0xfffffffd;
            if (*(int *)(iVar3 + 0x25c) != 0) {
              FUN_004defe0();
              iVar1 = *(int *)(iVar3 + 0x25c);
              *(undefined4 *)(iVar1 + 0x1c) = 0;
              *(undefined4 *)(iVar1 + 0x20) = 0;
            }
            if ((*(byte *)(iVar3 + 0x44) & 0x20) != 0) {
              FUN_0041c7b0(8);
            }
            if ((*(byte *)(iVar3 + 0x44) & 0x40) != 0) {
              uVar4 = FUN_0049c430(iVar3 + 0x138);
              FUN_0049bd90(uVar4);
              FUN_0041c7d0(iVar3 + 0x138,0);
            }
            if ((DAT_0066829c == 0) && ((*(byte *)(iVar3 + 0x44) & 0x10) != 0)) {
              FUN_004967a0_TScriptManager_Clear(iVar3);
            }
          }
          iVar5 = iVar5 + 1;
        } while (iVar5 < *(int *)(param_1 + 4));
      }
      FUN_0041ba00_TArea_Enter();
      iVar5 = *(int *)(*(int *)(param_1 + 0x14) + local_14 * 4);
      *(int *)(param_1 + 0x18) = iVar5;
      if ((*(uint *)(iVar5 + 0x44) & 0x4000) != 0) {
        *(undefined4 *)(param_1 + 0x1c) = *(undefined4 *)(iVar5 + 0x108);
        *(undefined4 *)(param_1 + 0x20) = *(undefined4 *)(iVar5 + 0x10c);
        *(undefined4 *)(param_1 + 0x24) = *(undefined4 *)(iVar5 + 0x110);
        *(undefined4 *)(param_1 + 0x28) = *(undefined4 *)(iVar5 + 0x114);
        *(undefined4 *)(param_1 + 0x2c) = *(undefined4 *)(iVar5 + 0x118);
        *(undefined4 *)(param_1 + 0x30) = *(undefined4 *)(iVar5 + 0x11c);
      }
    }
  }
  iVar5 = 0;
  if (0 < *(int *)(param_1 + 4)) {
    do {
      if ((*(byte *)(*(int *)(*(int *)(param_1 + 0x14) + iVar5 * 4) + 0x44) & 2) != 0) {
        FUN_0041b770_TArea_Pulse();
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < *(int *)(param_1 + 4));
  }
  DAT_005e91ec = uVar2;
  DAT_005e91f0 = local_c;
  DAT_005e91f4 = local_8;
  _DAT_005e91f8 = local_4;
  return;
}


