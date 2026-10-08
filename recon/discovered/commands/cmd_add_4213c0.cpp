// FUN_004213c0 @ 004213c0 size=1

/* WARNING: Type propagation algorithm not settling */

undefined4 FUN_004213c0(undefined4 param_1,int param_2)

{
  ushort uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  int *piVar6;
  uint uVar7;
  short *psVar8;
  undefined4 uVar9;
  int aiStack_44 [4];
  short sStack_34;
  ushort uStack_32;
  undefined4 uStack_30;
  undefined2 uStack_2a;
  undefined4 uStack_28;
  undefined4 uStack_24;
  int iStack_20;
  
  iVar4 = DAT_00666990;
  uVar2 = DAT_0066698c;
  uVar9 = DAT_00666988;
  aiStack_44[0] = 1;
  aiStack_44[2] = DAT_0066698c;
  aiStack_44[3] = DAT_00666990;
  if (*(int *)(param_2 + 0x10) == 8) {
    aiStack_44[0] = *(int *)(param_2 + 0x14);
    FUN_00479580();
  }
  iVar3 = FUN_00479700(0x5cb10c,0);
  if (iVar3 == 0) {
    iVar3 = FUN_00479700(0x5cb114,0);
    if (iVar3 == 0) {
      iVar3 = FUN_00479700(s_light_005cb11c,1);
      if (iVar3 != 0) {
        psVar8 = &sStack_34;
        for (iVar3 = 0xd; iVar3 != 0; iVar3 = iVar3 + -1) {
          psVar8[0] = 0;
          psVar8[1] = 0;
          psVar8 = psVar8 + 2;
        }
        sStack_34 = 9;
        uStack_32 = FUN_00475210(s_light_005cb124,0);
        iStack_20 = iVar4 + 0x3c;
        uStack_2a = (undefined2)DAT_00666970;
        uStack_30 = 0x104;
        uStack_28 = uVar9;
        uStack_24 = uVar2;
        iVar4 = FUN_00450e40(&sStack_34,0xffffffff);
        if (iVar4 < 0) {
          FUN_0041ee50(s_ERROR__Unable_to_add_light_005cb12c);
          return 0;
        }
        uVar9 = 0xdc;
        FUN_00452690(iVar4,0);
        FUN_004714e0(uVar9);
        puVar5 = (undefined4 *)FUN_00429950(0xff,0xff,0xff);
        uVar9 = *puVar5;
        FUN_00452690(iVar4,0);
        FUN_00471820(uVar9);
        uVar9 = 0xc;
        FUN_00452690(iVar4,0);
        FUN_004715e0(uVar9);
        piVar6 = aiStack_44 + 1;
        aiStack_44[1] = 0;
        aiStack_44[2] = 0;
        aiStack_44[3] = 0;
        FUN_00452690(iVar4,0);
        FUN_004716c0(piVar6);
        FUN_004405d0(iVar4,0);
        return 0;
      }
      iVar4 = func_0x004752b0(*(undefined4 *)(param_2 + 0x28));
      if (iVar4 == 0) {
        FUN_0041ee50(s_No_object_type_named___s___005cb180,*(undefined4 *)(param_2 + 0x28));
        return 0;
      }
      uVar1 = *(ushort *)(iVar4 + 0x20);
      uVar7 = (uint)uVar1;
      if (*(byte *)(iVar4 + 0x22) < DAT_0065a258) {
        iVar4 = (&DAT_0065a148)[*(byte *)(iVar4 + 0x22)];
      }
      else {
        iVar4 = 0;
      }
      psVar8 = &sStack_34;
      for (iVar3 = 0xd; iVar3 != 0; iVar3 = iVar3 + -1) {
        psVar8[0] = 0;
        psVar8[1] = 0;
        psVar8 = psVar8 + 2;
      }
      sStack_34 = *(short *)(iVar4 + 8);
      uStack_30 = 0;
      uStack_2a = (undefined2)DAT_00666970;
      uStack_24 = aiStack_44[2];
      uStack_28 = uVar9;
      iStack_20 = aiStack_44[3];
      uStack_32 = uVar1;
      iVar3 = FUN_00450e40(&sStack_34,0xffffffff);
      if (iVar3 < 0) {
        FUN_0041ee50(s_ERROR__Creating_object_005cb148);
        FUN_00479580();
        return 0;
      }
      FUN_004405d0(iVar3,0);
      if (((*(int *)(iVar4 + 0x34) == 0) || (*(uint *)(iVar4 + 0x24) <= uVar7)) ||
         (*(int *)(*(int *)(iVar4 + 0x34) + uVar7 * 4) == 0)) {
        puVar5 = (undefined4 *)0x0;
      }
      else {
        puVar5 = *(undefined4 **)(*(int *)(iVar4 + 0x34) + uVar7 * 4);
        if (puVar5 == (undefined4 *)0x0) {
          puVar5 = *(undefined4 **)(iVar4 + 0x38);
        }
      }
      FUN_0041ee50(s__s__s_added_at___d___d___d___005cb160,*(undefined4 *)(iVar4 + 4),*puVar5,
                   uStack_28,uStack_24,iStack_20);
      iVar4 = aiStack_44[0];
      if (aiStack_44[0] != 1) {
        piVar6 = (int *)FUN_00452690(iVar3,0);
        (**(code **)(*piVar6 + 0x19c))(iVar4);
      }
      if ((sStack_34 == 0xb) && (iVar4 = FUN_00452690(iVar3,0), iVar4 != 0)) {
        FUN_0051f0a0(iVar4,0xffffffff);
        FUN_0051f060(iVar4);
      }
      FUN_00479580();
      return 0;
    }
    aiStack_44[1] = 0;
    DAT_00658198 = DAT_00658198 + -1;
    aiStack_44[2] = 0;
    aiStack_44[3] = 0;
    DAT_0065819c = DAT_00658198;
    (**(code **)(DAT_00658018 + 0x2c))(1);
  }
  else {
    aiStack_44[1] = 0;
    DAT_00658198 = DAT_00658198 + 1;
    aiStack_44[2] = 0;
    aiStack_44[3] = 0;
    DAT_0065819c = DAT_00658198;
    (**(code **)(DAT_00658018 + 0x2c))(1);
  }
  FUN_004430f0(aiStack_44);
  FUN_00479580();
  return 0;
}


