// FUN_00420dc0 @ 00420dc0 size=542

undefined4 FUN_00420dc0(int param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  uint uStack_28;
  uint uStack_24;
  undefined1 auStack_20 [32];
  
  uVar2 = param_2;
  if (*(int *)(param_2 + 0x10) != 4) {
    iVar1 = FUN_0047a410(param_2,s__s__d__d__d_005caf5c,auStack_20,&param_2,&uStack_28,&uStack_24);
    if (iVar1 == 0) {
      return 4;
    }
    if ((int)param_2 < 0x7f) {
      uVar2 = param_2 & ((int)param_2 < 0) - 1;
    }
    else {
      uVar2 = 0x7f;
    }
    *(uint *)(param_1 + 0x1a8) = uVar2;
    FUN_0049c760(*(undefined4 *)(param_1 + 0x1a4),uVar2);
    uStack_28 = uStack_28 & ((int)uStack_28 < 0) - 1;
    *(uint *)(param_1 + 0x1ac) = uStack_28;
    if ((int)uStack_24 < (int)uStack_28) {
      uStack_24 = uStack_28;
    }
    *(uint *)(param_1 + 0x1b0) = uStack_24;
    FUN_0049c6b0(*(undefined4 *)(param_1 + 0x1a4),uStack_28,uStack_24);
    FUN_004f4730(auStack_20);
    return 0;
  }
  iVar1 = FUN_00479700(s_sound_005caf68,0);
  if (iVar1 != 0) {
    FUN_00479580();
    iVar1 = FUN_0047a410(uVar2,&DAT_005caf70,auStack_20);
    if (iVar1 == 0) {
      return 4;
    }
    FUN_004f4730(auStack_20);
    FUN_00479580();
    return 0;
  }
  iVar1 = FUN_00479700(s_volume_005caf74,0);
  if (iVar1 == 0) {
    iVar1 = FUN_00479700(s_range_005caf80,0);
    if (iVar1 != 0) {
      FUN_00479580();
      iVar1 = FUN_0047a410(uVar2,s__d__d_005caf88,&uStack_28,&uStack_24);
      if (iVar1 == 0) {
        return 4;
      }
      uStack_28 = uStack_28 & ((int)uStack_28 < 0) - 1;
      *(uint *)(param_1 + 0x1ac) = uStack_28;
      if ((int)uStack_24 < (int)uStack_28) {
        uStack_24 = uStack_28;
      }
      *(uint *)(param_1 + 0x1b0) = uStack_24;
      FUN_0049c6b0(*(undefined4 *)(param_1 + 0x1a4),uStack_28,uStack_24);
    }
    FUN_00479580();
    return 0;
  }
  FUN_00479580();
  iVar1 = FUN_0047a410(uVar2,&DAT_005caf7c,&param_2);
  if (iVar1 == 0) {
    return 4;
  }
  if ((int)param_2 < 0x7f) {
    uVar2 = param_2 & ((int)param_2 < 0) - 1;
  }
  else {
    uVar2 = 0x7f;
  }
  *(uint *)(param_1 + 0x1a8) = uVar2;
  FUN_0049c760(*(undefined4 *)(param_1 + 0x1a4),uVar2);
  FUN_00479580();
  return 0;
}


