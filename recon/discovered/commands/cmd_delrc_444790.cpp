// FUN_00444790 @ 00444790 size=1105

undefined4 FUN_00444790(void)

{
  char cVar1;
  DWORD DVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  undefined1 *puVar7;
  uint uVar8;
  uint uVar9;
  char *pcVar10;
  char *pcVar11;
  char acStack_308 [260];
  char acStack_204 [256];
  char acStack_104 [260];
  
  do {
    FUN_0041ee50(s_Which_Class_are_you_deleting_fro_005cfa54);
    FUN_0041ee50(s__1__Item__2__Weapon__3__Armor_005cfa7c);
    FUN_0041ee50(s__4__Talisman__5__Food__6__Contai_005cfaac);
    FUN_0041ee50(s__7__Light_Source__8__Tool__9__Mo_005cfae0);
    FUN_0041ee50(s__10__Tile__11__Exit__12__Player_005cfb10);
    FUN_0041ee50(s__13__Character__14__Trap__15__Sh_005cfb40);
    FUN_0041ee50(s__16__Helper__17__Fireball__18__I_005cfb70);
    FUN_0041ee50(s__19__Freeze__20__Lightning__21__H_005cfb9c);
    FUN_0041ee50(s__22__Ammo__23__Scroll__24__Range_005cfbc8);
    FUN_0041ee50(s__25__Effect_005cfc00);
    FUN_0041ee50(&DAT_005cfc10);
    do {
      DVar2 = WaitForMultipleObjects(2,(HANDLE *)&DAT_00656b20,0,0xffffffff);
      if (DVar2 != 1) {
        return 0;
      }
      ResetEvent(DAT_00656b24);
      pcVar10 = DAT_00656db4;
      if (DAT_006581b8 == -1) {
        return 0;
      }
    } while (((DAT_006581b8 != 0xd) || (DAT_00656db4 == (char *)0x0)) ||
            (iVar3 = FUN_0058ade0(DAT_00656db4,10), iVar3 == 0));
    cVar1 = *pcVar10;
    for (iVar3 = 0; ((cVar1 != '\0' && (cVar1 != '\n')) && (iVar3 < 0x103)); iVar3 = iVar3 + 1) {
      pcVar10 = pcVar10 + 1;
      acStack_308[iVar3] = cVar1;
      cVar1 = *pcVar10;
    }
    acStack_308[iVar3] = '\0';
    if (acStack_308[0] == '\0') {
      return 0;
    }
    iVar3 = FUN_0058b42c(acStack_308);
  } while ((iVar3 < 1) || (0x19 < iVar3));
  uVar4 = iVar3 - 1;
  if (DAT_0065a258 <= uVar4) {
    return 0;
  }
  iVar3 = (&DAT_0065a148)[uVar4];
  if (iVar3 == 0) {
    return 0;
  }
  while( true ) {
    FUN_0041ee50(s_Enter_the_name_of_the_resource_t_005cfc14);
    FUN_0041ee50(&DAT_005cfc40);
    do {
      DVar2 = WaitForMultipleObjects(2,(HANDLE *)&DAT_00656b20,0,0xffffffff);
      if (DVar2 != 1) {
        return 0;
      }
      ResetEvent(DAT_00656b24);
      pcVar10 = DAT_00656db4;
      if (DAT_006581b8 == -1) {
        return 0;
      }
    } while (((DAT_006581b8 != 0xd) || (DAT_00656db4 == (char *)0x0)) ||
            (iVar5 = FUN_0058ade0(DAT_00656db4,10), iVar5 == 0));
    cVar1 = *pcVar10;
    for (iVar5 = 0; ((cVar1 != '\0' && (cVar1 != '\n')) && (iVar5 < 0x103)); iVar5 = iVar5 + 1) {
      pcVar10 = pcVar10 + 1;
      acStack_308[iVar5] = cVar1;
      cVar1 = *pcVar10;
    }
    acStack_308[iVar5] = '\0';
    if (acStack_308[0] == '\0') {
      return 0;
    }
    uVar6 = FUN_00475210(acStack_308,0);
    if (uVar6 != 0xffffffff) break;
    while( true ) {
      FUN_0041ee50(s_The_specified_resource_does_not_e_005cfc44);
      FUN_0041ee50(s_Do_you_wish_to_re_enter_it__Y_N__005cfc70);
      FUN_0041ee50(&DAT_005cfc94);
      do {
        DVar2 = WaitForMultipleObjects(2,(HANDLE *)&DAT_00656b20,0,0xffffffff);
        if (DVar2 != 1) {
          return 0;
        }
        ResetEvent(DAT_00656b24);
        pcVar10 = DAT_00656db4;
        if (DAT_006581b8 == -1) {
          return 0;
        }
      } while (((DAT_006581b8 != 0xd) || (DAT_00656db4 == (char *)0x0)) ||
              (iVar5 = FUN_0058ade0(DAT_00656db4,10), iVar5 == 0));
      cVar1 = *pcVar10;
      for (iVar5 = 0; ((cVar1 != '\0' && (cVar1 != '\n')) && (iVar5 < 0xff)); iVar5 = iVar5 + 1) {
        pcVar10 = pcVar10 + 1;
        acStack_204[iVar5] = cVar1;
        cVar1 = *pcVar10;
      }
      acStack_204[iVar5] = '\0';
      if (acStack_204[0] == '\0') {
        return 0;
      }
      acStack_204[0] = FUN_0058b266((int)acStack_204[0]);
      if (acStack_204[0] == 'Y') break;
      if (acStack_204[0] == 'N') {
        return 0;
      }
    }
  }
  if (((*(int *)(iVar3 + 0x34) == 0) || (*(uint *)(iVar3 + 0x24) <= uVar6)) ||
     (*(int *)(*(int *)(iVar3 + 0x34) + uVar6 * 4) == 0)) {
    iVar5 = 0;
  }
  else {
    iVar5 = *(int *)(*(int *)(iVar3 + 0x34) + uVar6 * 4);
    if (iVar5 == 0) {
      iVar5 = *(int *)(iVar3 + 0x38);
    }
  }
  pcVar10 = (char *)FUN_00446490(*(undefined4 *)(iVar5 + 8));
  uVar8 = 0xffffffff;
  do {
    pcVar11 = pcVar10;
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    pcVar11 = pcVar10 + 1;
    cVar1 = *pcVar10;
    pcVar10 = pcVar11;
  } while (cVar1 != '\0');
  uVar8 = ~uVar8;
  pcVar10 = pcVar11 + -uVar8;
  pcVar11 = acStack_104;
  for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
    *(undefined4 *)pcVar11 = *(undefined4 *)pcVar10;
    pcVar10 = pcVar10 + 4;
    pcVar11 = pcVar11 + 4;
  }
  for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *pcVar11 = *pcVar10;
    pcVar10 = pcVar10 + 1;
    pcVar11 = pcVar11 + 1;
  }
  FUN_00474b40(uVar6);
  if (uVar4 == 9) {
    acStack_308[0] = '\0';
    iVar3 = 0;
    do {
      iVar5 = FUN_0059a600(acStack_104,*(undefined4 *)((int)&PTR_DAT_005cee30 + iVar3),3);
      if (iVar5 == 0) {
        FUN_0058b100(acStack_308,s__s_s__s_005cfc98,&DAT_0065dde8,
                     *(undefined4 *)((int)&PTR_DAT_005cee00 + iVar3),acStack_104);
      }
      iVar3 = iVar3 + 4;
    } while (iVar3 < 0x30);
    if (acStack_308[0] == '\0') {
      FUN_0041ee50(s_Could_not_determine_graphics_pat_005cfca0);
      FUN_0041ee50(s_Imagery_files_were_not_deleted__005cfcc8);
      return 0;
    }
  }
  else {
    FUN_0058b100(acStack_308,s__s_s__s_005cfcec,&DAT_0065dde8,(&PTR_DAT_005ced9c)[uVar4],acStack_104
                );
  }
  iVar3 = FUN_0058ade0(acStack_308,0x2e);
  if (iVar3 == 0) {
    iVar3 = -1;
    pcVar10 = acStack_308;
    do {
      pcVar11 = pcVar10;
      if (iVar3 == 0) break;
      iVar3 = iVar3 + -1;
      pcVar11 = pcVar10 + 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar11;
    } while (cVar1 != '\0');
    *(undefined4 *)(pcVar11 + -1) = DAT_005cfcf4;
    pcVar11[3] = DAT_005cfcf8;
  }
  FUN_0058bff1(acStack_308);
  puVar7 = (undefined1 *)FUN_0058ade0(acStack_308,0x2e);
  *puVar7 = 0;
  iVar3 = -1;
  pcVar10 = acStack_308;
  do {
    pcVar11 = pcVar10;
    if (iVar3 == 0) break;
    iVar3 = iVar3 + -1;
    pcVar11 = pcVar10 + 1;
    cVar1 = *pcVar10;
    pcVar10 = pcVar11;
  } while (cVar1 != '\0');
  *(undefined4 *)(pcVar11 + -1) = DAT_005cfcfc;
  FUN_0058bff1(acStack_308);
  return 0;
}


