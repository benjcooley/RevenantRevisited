// FUN_00421770 @ 00421770 size=1104

undefined4 FUN_00421770(undefined4 param_1,uint param_2)

{
  short sVar1;
  int iVar2;
  undefined4 *puVar3;
  int iVar4;
  uint uVar5;
  int *piVar6;
  short *psVar7;
  undefined4 uVar8;
  int iStack_44;
  undefined4 uStack_40;
  undefined4 uStack_3c;
  undefined4 uStack_38;
  short sStack_34;
  undefined2 uStack_32;
  undefined4 uStack_30;
  undefined2 uStack_2a;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  
  iVar4 = param_2;
  uStack_38 = DAT_00666990;
  uVar5 = 0xffffffff;
  iVar2 = *(int *)(param_2 + 0x10);
  iStack_44 = 1;
  if (iVar2 == 8) {
    uStack_40 = *(undefined4 *)(param_2 + 0x14);
  }
  else {
    if ((iVar2 != 2) && (iVar2 != 4)) {
      return 4;
    }
    uStack_40 = FUN_00497800(*(undefined4 *)(param_2 + 0x28),param_1);
  }
  FUN_00478a10();
  FUN_00479580();
  iVar2 = *(int *)(param_2 + 0x10);
  if (iVar2 == 8) {
    uStack_3c = *(undefined4 *)(param_2 + 0x14);
  }
  else {
    if ((iVar2 != 2) && (iVar2 != 4)) {
      return 4;
    }
    uStack_3c = FUN_00497800(*(undefined4 *)(param_2 + 0x28),param_1);
  }
  FUN_00478a10();
  FUN_00479580();
  if (*(int *)(param_2 + 0x10) == 8) {
    iStack_44 = *(int *)(param_2 + 0x14);
    FUN_00479580();
  }
  iVar2 = FUN_00479700(&DAT_005cb19c,0);
  if (iVar2 == 0) {
    iVar2 = FUN_00479700(&DAT_005cb1a4,0);
    if (iVar2 == 0) {
      iVar2 = FUN_00479700(s_light_005cb1ac,1);
      if (iVar2 != 0) {
        psVar7 = &sStack_34;
        for (iVar2 = 0xd; iVar2 != 0; iVar2 = iVar2 + -1) {
          psVar7[0] = 0;
          psVar7[1] = 0;
          psVar7 = psVar7 + 2;
        }
        sStack_34 = 9;
        uStack_32 = FUN_00475210(s_light_005cb1b4,0);
        uStack_28 = uStack_40;
        uStack_30 = 0x104;
        uStack_24 = uStack_3c;
        uStack_20 = 0x3c;
        uStack_2a = (undefined2)DAT_00666970;
        iVar2 = FUN_00450e40(&sStack_34,0xffffffff);
        if (-1 < iVar2) {
          uVar8 = 0xdc;
          FUN_00452690(iVar2,0);
          FUN_004714e0(uVar8);
          puVar3 = (undefined4 *)FUN_00429950(0xff,0xff,0xff);
          uVar8 = *puVar3;
          FUN_00452690(iVar2,0);
          FUN_00471820(uVar8);
          uVar8 = 0xc;
          FUN_00452690(iVar2,0);
          FUN_004715e0(uVar8);
          puVar3 = &uStack_40;
          uStack_40 = 0;
          uStack_3c = 0;
          uStack_38 = 0;
          FUN_00452690(iVar2,0);
          FUN_004716c0(puVar3);
          FUN_004405d0(iVar2,0);
          return 0;
        }
        FUN_0041ee50(s_ERROR__Unable_to_add_light_005cb1bc);
        return 0;
      }
      param_2 = 0;
      piVar6 = &DAT_0065a148;
      do {
        iVar2 = 0;
        if (((param_2 < DAT_0065a258) && (iVar2 = *piVar6, iVar2 != 0)) &&
           (uVar5 = FUN_00475210(*(undefined4 *)(iVar4 + 0x28),0), -1 < (int)uVar5))
        goto LAB_00421a62;
        piVar6 = piVar6 + 1;
        param_2 = param_2 + 1;
      } while ((int)piVar6 < 0x65a248);
      if ((int)uVar5 < 0) {
        FUN_0041ee50(s_No_object_type_named___s___005cb210,*(undefined4 *)(iVar4 + 0x28));
        return 0;
      }
LAB_00421a62:
      sVar1 = *(short *)(iVar2 + 8);
      psVar7 = &sStack_34;
      for (iVar4 = 0xd; iVar4 != 0; iVar4 = iVar4 + -1) {
        psVar7[0] = 0;
        psVar7[1] = 0;
        psVar7 = psVar7 + 2;
      }
      uStack_30 = 0;
      uStack_28 = uStack_40;
      uStack_2a = (undefined2)DAT_00666970;
      uStack_32 = (undefined2)uVar5;
      uStack_24 = uStack_3c;
      uStack_20 = uStack_38;
      sStack_34 = sVar1;
      iVar4 = FUN_00450e40(&sStack_34,0xffffffff);
      if (iVar4 < 0) {
        FUN_0041ee50(s_ERROR__Creating_object_005cb1d8);
        FUN_00479580();
        return 0;
      }
      FUN_004405d0(iVar4,0);
      if (((*(int *)(iVar2 + 0x34) == 0) || (*(uint *)(iVar2 + 0x24) <= uVar5)) ||
         (*(int *)(*(int *)(iVar2 + 0x34) + uVar5 * 4) == 0)) {
        puVar3 = (undefined4 *)0x0;
      }
      else {
        puVar3 = *(undefined4 **)(*(int *)(iVar2 + 0x34) + uVar5 * 4);
        if (puVar3 == (undefined4 *)0x0) {
          puVar3 = *(undefined4 **)(iVar2 + 0x38);
        }
      }
      FUN_0041ee50(s__s__s_added_at___d___d___d___005cb1f0,*(undefined4 *)(iVar2 + 4),*puVar3,
                   uStack_28,uStack_24,uStack_20);
      if (iStack_44 != 1) {
        piVar6 = (int *)FUN_00452690(iVar4,0);
        (**(code **)(*piVar6 + 0x19c))(iStack_44);
      }
      if ((sStack_34 == 0xb) && (iVar2 = FUN_00452690(iVar4,0), iVar2 != 0)) {
        FUN_0051f0a0(iVar2,0xffffffff);
        FUN_0051f060(iVar2);
        FUN_00479580();
        return 0;
      }
      goto LAB_004218d1;
    }
    uStack_40 = 0;
    DAT_00658198 = DAT_00658198 + -1;
    uStack_3c = 0;
    uStack_38 = 0;
    DAT_0065819c = DAT_00658198;
    (**(code **)(DAT_00658018 + 0x2c))(1);
  }
  else {
    uStack_40 = 0;
    DAT_00658198 = DAT_00658198 + 1;
    uStack_3c = 0;
    uStack_38 = 0;
    DAT_0065819c = DAT_00658198;
    (**(code **)(DAT_00658018 + 0x2c))(1);
  }
  FUN_004430f0(&uStack_40);
LAB_004218d1:
  FUN_00479580();
  return 0;
}


