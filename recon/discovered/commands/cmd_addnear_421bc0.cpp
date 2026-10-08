// FUN_00421bc0 @ 00421bc0 size=1079

undefined4 FUN_00421bc0(int param_1,uint param_2)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  int iVar4;
  uint uVar5;
  int *piVar6;
  short *psVar7;
  undefined4 uVar8;
  int iStack_48;
  int iStack_44;
  int iStack_40;
  int iStack_3c;
  undefined4 uStack_38;
  short sStack_34;
  undefined2 uStack_32;
  undefined4 uStack_30;
  undefined2 uStack_2a;
  int iStack_28;
  int iStack_24;
  undefined4 uStack_20;
  
  uVar1 = param_2;
  uVar5 = 0xffffffff;
  iStack_48 = 1;
  iVar2 = FUN_00451fe0(*(undefined4 *)(param_2 + 0x28),param_1,0,0);
  FUN_00479580();
  if (iVar2 == 0) {
    return 4;
  }
  iStack_3c = *(int *)(iVar2 + 0x14);
  iVar4 = *(int *)(iVar2 + 0x10);
  uStack_38 = *(undefined4 *)(iVar2 + 0x18);
  iStack_40 = iVar4;
  iVar2 = FUN_0047a410(uVar1,s__d__d_005cb22c,&param_1,&iStack_44);
  if (iVar2 != 0) {
    iStack_40 = iVar4 + param_1;
    iStack_3c = iStack_3c + iStack_44;
  }
  if (*(int *)(uVar1 + 0x10) == 8) {
    iStack_48 = *(int *)(uVar1 + 0x14);
    FUN_00479580();
  }
  iVar2 = FUN_00479700(&DAT_005cb234,0);
  if (iVar2 == 0) {
    iVar2 = FUN_00479700(&DAT_005cb23c,0);
    if (iVar2 == 0) {
      iVar2 = FUN_00479700(s_light_005cb244,1);
      if (iVar2 != 0) {
        psVar7 = &sStack_34;
        for (iVar2 = 0xd; iVar2 != 0; iVar2 = iVar2 + -1) {
          psVar7[0] = 0;
          psVar7[1] = 0;
          psVar7 = psVar7 + 2;
        }
        sStack_34 = 9;
        uStack_32 = FUN_00475210(s_light_005cb24c,0);
        iStack_28 = iStack_40;
        uStack_2a = (undefined2)DAT_00666970;
        uStack_30 = 0x104;
        iStack_24 = iStack_3c;
        uStack_20 = 0x3c;
        iVar2 = FUN_00450e40(&sStack_34,0xffffffff);
        if (iVar2 < 0) {
          FUN_0041ee50(s_ERROR__Unable_to_add_light_005cb254);
          return 0;
        }
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
        piVar6 = &iStack_40;
        iStack_40 = 0;
        iStack_3c = 0;
        uStack_38 = 0;
        FUN_00452690(iVar2,0);
        FUN_004716c0(piVar6);
        FUN_004405d0(iVar2,0);
        return 0;
      }
      param_2 = 0;
      piVar6 = &DAT_0065a148;
      do {
        if (param_2 < DAT_0065a258) {
          iVar2 = *piVar6;
          if (iVar2 != 0) {
            uVar5 = FUN_00475210(*(undefined4 *)(uVar1 + 0x28),0);
            if (-1 < (int)uVar5) goto LAB_00421e94;
          }
        }
        else {
          iVar2 = 0;
        }
        piVar6 = piVar6 + 1;
        param_2 = param_2 + 1;
      } while ((int)piVar6 < 0x65a248);
      if ((int)uVar5 < 0) {
        FUN_0041ee50(s_No_object_type_named___s___005cb2a8,*(undefined4 *)(uVar1 + 0x28));
        return 0;
      }
LAB_00421e94:
      psVar7 = &sStack_34;
      for (iVar4 = 0xd; iVar4 != 0; iVar4 = iVar4 + -1) {
        psVar7[0] = 0;
        psVar7[1] = 0;
        psVar7 = psVar7 + 2;
      }
      sStack_34 = *(short *)(iVar2 + 8);
      uStack_2a = (undefined2)DAT_00666970;
      iStack_28 = iStack_40;
      uStack_20 = uStack_38;
      uStack_32 = (undefined2)uVar5;
      uStack_30 = 0;
      iStack_24 = iStack_3c;
      iVar4 = FUN_00450e40(&sStack_34,0xffffffff);
      if (iVar4 < 0) {
        FUN_0041ee50(s_ERROR__Creating_object_005cb270);
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
      FUN_0041ee50(s__s__s_added_at___d___d___d___005cb288,*(undefined4 *)(iVar2 + 4),*puVar3,
                   iStack_28,iStack_24,uStack_20);
      if (iStack_48 != 1) {
        piVar6 = (int *)FUN_00452690(iVar4,0);
        (**(code **)(*piVar6 + 0x19c))(iStack_48);
      }
      if (sStack_34 == 0xb) {
        iVar2 = FUN_00452690(iVar4,0);
        if (iVar2 != 0) {
          FUN_0051f0a0(iVar2,0xffffffff);
          FUN_0051f060(iVar2);
          FUN_00479580();
          return 0;
        }
      }
      goto LAB_00421cfc;
    }
    iStack_40 = 0;
    DAT_00658198 = DAT_00658198 + -1;
    iStack_3c = 0;
    uStack_38 = 0;
    DAT_0065819c = DAT_00658198;
    (**(code **)(DAT_00658018 + 0x2c))(1);
  }
  else {
    iStack_40 = 0;
    DAT_00658198 = DAT_00658198 + 1;
    iStack_3c = 0;
    uStack_38 = 0;
    DAT_0065819c = DAT_00658198;
    (**(code **)(DAT_00658018 + 0x2c))(1);
  }
  FUN_004430f0(&iStack_40);
LAB_00421cfc:
  FUN_00479580();
  return 0;
}


