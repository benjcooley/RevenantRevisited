// FUN_004223c0 @ 004223c0 size=928

undefined4 FUN_004223c0(undefined4 param_1,int param_2)

{
  char cVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  undefined1 uVar7;
  int iVar8;
  int iVar9;
  int *piVar10;
  int iVar11;
  char *pcVar12;
  char *pcVar13;
  int iStack_c4;
  uint uStack_c0;
  int iStack_bc;
  int iStack_b8;
  uint uStack_b4;
  int iStack_b0;
  int iStack_ac;
  undefined1 auStack_a4 [4];
  char acStack_a0 [80];
  char acStack_50 [80];
  
  if (*(int *)(param_2 + 0x10) != 4) {
    return 4;
  }
  uVar5 = 0xffffffff;
  pcVar13 = *(char **)(param_2 + 0x28);
  do {
    pcVar12 = pcVar13;
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    pcVar12 = pcVar13 + 1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar12;
  } while (cVar1 != '\0');
  uVar5 = ~uVar5;
  pcVar13 = pcVar12 + -uVar5;
  pcVar12 = acStack_a0;
  for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    *(undefined4 *)pcVar12 = *(undefined4 *)pcVar13;
    pcVar13 = pcVar13 + 4;
    pcVar12 = pcVar12 + 4;
  }
  for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
    *pcVar12 = *pcVar13;
    pcVar13 = pcVar13 + 1;
    pcVar12 = pcVar12 + 1;
  }
  FUN_00479580();
  if (*(int *)(param_2 + 0x10) != 4) {
    return 4;
  }
  uVar5 = 0xffffffff;
  pcVar13 = *(char **)(param_2 + 0x28);
  do {
    pcVar12 = pcVar13;
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    pcVar12 = pcVar13 + 1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar12;
  } while (cVar1 != '\0');
  uVar5 = ~uVar5;
  pcVar13 = pcVar12 + -uVar5;
  pcVar12 = acStack_50;
  for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    *(undefined4 *)pcVar12 = *(undefined4 *)pcVar13;
    pcVar13 = pcVar13 + 4;
    pcVar12 = pcVar12 + 4;
  }
  for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
    *pcVar12 = *pcVar13;
    pcVar13 = pcVar13 + 1;
    pcVar12 = pcVar12 + 1;
  }
  FUN_00479580();
  if (*(int *)(param_2 + 0x10) != 8) {
    return 4;
  }
  iStack_b0 = __ftol();
  if ((((iStack_b0 != 0) && (iStack_b0 != 0x5a)) && (iStack_b0 != 0xb4)) && (iStack_b0 != 0x10e)) {
    FUN_0041ee50(s_rotation_must_be_0__90__180_or_2_005cb384);
    return 4;
  }
  uVar5 = 0;
  uStack_c0 = 0xffffffff;
  uStack_b4 = 0xffffffff;
  piVar10 = &DAT_0065a148;
  do {
    if (uVar5 < DAT_0065a258) {
      iVar8 = *piVar10;
      if ((iVar8 != 0) && (uStack_c0 = FUN_00475210(acStack_a0,0), -1 < (int)uStack_c0)) break;
    }
    else {
      iVar8 = 0;
    }
    piVar10 = piVar10 + 1;
    uVar5 = uVar5 + 1;
  } while ((int)piVar10 < 0x65a248);
  uVar5 = 0;
  piVar10 = &DAT_0065a148;
  do {
    if (uVar5 < DAT_0065a258) {
      iVar9 = *piVar10;
      if ((iVar9 != 0) && (uStack_b4 = FUN_00475210(acStack_50,0), -1 < (int)uStack_b4)) break;
    }
    else {
      iVar9 = 0;
    }
    piVar10 = piVar10 + 1;
    uVar5 = uVar5 + 1;
  } while ((int)piVar10 < 0x65a248);
  if ((uStack_c0 == 0xffffffff) || (uStack_b4 == 0xffffffff)) {
    pcVar13 = s_bad_source_or_destination_tile__c_005cb3f8;
  }
  else {
    if (((*(int *)(iVar8 + 0x34) == 0) || (*(uint *)(iVar8 + 0x24) <= uStack_c0)) ||
       (*(int *)(*(int *)(iVar8 + 0x34) + uStack_c0 * 4) == 0)) {
      iVar2 = 0;
    }
    else {
      iVar2 = *(int *)(*(int *)(iVar8 + 0x34) + uStack_c0 * 4);
      if (iVar2 == 0) {
        iVar2 = *(int *)(iVar8 + 0x38);
      }
    }
    piVar10 = (int *)FUN_00446b10(*(undefined4 *)(iVar2 + 8),1);
    if (((*(int *)(iVar9 + 0x34) == 0) || (*(uint *)(iVar9 + 0x24) <= uStack_b4)) ||
       (*(int *)(*(int *)(iVar9 + 0x34) + uStack_b4 * 4) == 0)) {
      iVar8 = 0;
    }
    else {
      iVar8 = *(int *)(*(int *)(iVar9 + 0x34) + uStack_b4 * 4);
      if (iVar8 == 0) {
        iVar8 = *(int *)(iVar9 + 0x38);
      }
    }
    piVar3 = (int *)FUN_00446b10(*(undefined4 *)(iVar8 + 8),1);
    if ((piVar10 == (int *)0x0) || (piVar3 == (int *)0x0)) {
      pcVar13 = s_bad_source_or_destination_tile__c_005cb3b0;
    }
    else {
      (**(code **)(*piVar10 + 0xb4))(0,&iStack_b8,&iStack_bc,auStack_a4);
      (**(code **)(*piVar3 + 0xb4))(0,&stack0xffffff2c,&iStack_bc,&iStack_b8);
      iVar8 = (**(code **)(*piVar10 + 0x60))(0);
      iVar9 = (**(code **)(*piVar3 + 0x60))(0);
      if (iVar8 == 0) {
        pcVar13 = s_no_walkmap_for_source_tile_005cb440;
      }
      else if (iVar9 == 0) {
        pcVar13 = s_no_walkmap_for_destination_tile_005cb460;
      }
      else {
        if ((((iStack_b0 != 0x5a) && (iStack_b0 != 0x10e)) ||
            ((iStack_b8 == iStack_ac && (iStack_bc == iStack_c4)))) &&
           (((iStack_b0 != 0 && (iStack_b0 != 0xb4)) ||
            ((iStack_b8 == iStack_c4 && (iStack_bc == iStack_ac)))))) {
          iVar2 = 0;
          if (0 < iStack_ac) {
            do {
              iVar11 = 0;
              if (0 < iStack_c4) {
                do {
                  if (iStack_b0 < 0xb5) {
                    if (iStack_b0 == 0xb4) {
                      uVar7 = *(undefined1 *)
                               (((iStack_bc - iVar2) * iStack_b8 - iVar11) + -1 + iVar8);
                    }
                    else if (iStack_b0 == 0) {
                      uVar7 = *(undefined1 *)(iStack_b8 * iVar2 + iVar11 + iVar8);
                    }
                    else {
                      if (iStack_b0 != 0x5a) goto LAB_0042272e;
                      uVar7 = *(undefined1 *)(((iVar11 + 1) * iStack_b8 - iVar2) + -1 + iVar8);
                    }
                  }
                  else {
                    if (iStack_b0 != 0x10e) {
LAB_0042272e:
                      pcVar13 = s_Error_in_walkmap_dimensions__Abo_005cb508;
                      goto LAB_00422748;
                    }
                    uVar7 = *(undefined1 *)(((iStack_bc - iVar11) + -1) * iStack_b8 + iVar8 + iVar2)
                    ;
                  }
                  iVar4 = iStack_c4 * iVar2 + iVar11;
                  iVar11 = iVar11 + 1;
                  *(undefined1 *)(iVar4 + iVar9) = uVar7;
                } while (iVar11 < iStack_c4);
              }
              iVar2 = iVar2 + 1;
            } while (iVar2 < iStack_ac);
          }
          return 0;
        }
        pcVar13 = s_tile_dimensions_do_not_support_g_005cb484;
      }
    }
  }
LAB_00422748:
  FUN_0041ee50(pcVar13);
  return 4;
}


