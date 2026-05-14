// FUN_004cf490_SetRunMode @ 004cf490 size=1793
//
// IDENTIFICATION: TCharacter::SetRunMode  (NOT TPlayer-specific despite this
// helper being called from cls_0x5b4f30_TPlayer_Pulse_4c9790.cpp).
// Source-side counterpart: src/character.cpp:2944  bool TCharacter::SetRunMode().
//
// Same shape as SetWalkMode but with a "run" suffix appended before the
// directional variant:
//   - DAT_005e011c/0120/0124 = "run"  (literal suffix appended to root)
//   - DAT_005e0128/012c/0130 = "l","r","f"  (direction variants)
//   - DAT_005e0134/0138       = "f","l"     (used by the " to " transition)
//   - script signal id 0x28 -> SCMD_SETRUNMODE (vs 0x26 for walk).
// Calls SetWalkMode (0x4cf000) when current root is "sneak" so we can
// transition out of sneak before entering combat-run.
// See recon/discovered/player_combat_notes.md for the full walkthrough.

undefined4 __fastcall FUN_004cf490_SetRunMode(int *param_1)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  int *unaff_EBX;
  char *pcVar9;
  char *pcVar10;
  char *pcVar11;
  char acStack_8c [32];
  char acStack_6c [32];
  char acStack_4c [32];
  char acStack_2c [32];
  void *local_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e89d;
  local_c = ExceptionList;
  if ((param_1[0x44] & 0x80000U) == 0) {
    iVar2 = *(int *)(param_1[0x36] + 0x48);
    if ((iVar2 != 0) && ((*(uint *)(iVar2 + 0x24) & 0x2000000) != 0)) {
      return 0;
    }
    iVar2 = *(int *)(param_1[0x36] + 0x4c);
    if ((iVar2 != 0) && ((*(byte *)(iVar2 + 0x24) & 0x80) != 0)) {
      return 0;
    }
  }
  ExceptionList = &local_c;
  iVar2 = (**(code **)(*param_1 + 0x1c0))();
  if (iVar2 < 1) {
    ExceptionList = local_c;
    return 0;
  }
  if ((param_1[0x38] != 0) && (iVar2 = FUN_004dab80_TActionBlock_Is(s_sneak_005c618c), iVar2 != 0)) {
    FUN_004cf000_SetWalkMode();
  }
  if (((int *)param_1[0x38] != (int *)0x0) &&
     ((((*(int *)param_1[0x38] == 3 &&
        ((iVar2 = FUN_004dab80_TActionBlock_Is(s_combatrun_005e0618), iVar2 != 0 ||
         (iVar2 = FUN_004dab80_TActionBlock_Is(s_handrun_005e0610), iVar2 != 0)))) ||
       (((int *)param_1[0x38] != (int *)0x0 &&
        ((*(int *)param_1[0x38] == 0x19 && (iVar2 = FUN_004dab80_TActionBlock_Is(s_bowrun_005e0608), iVar2 != 0)))))
       ) || (iVar2 = FUN_004dab80_TActionBlock_Is(&DAT_005e0604), iVar2 != 0)))) {
    ExceptionList = local_c;
    return 1;
  }
  iVar2 = *(int *)param_1[0x38];
  if (iVar2 == 1) {
    pcVar10 = &DAT_005e011c;
LAB_004cf5f0:
    uVar7 = 0xffffffff;
    do {
      pcVar9 = pcVar10;
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      pcVar9 = pcVar10 + 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar9;
    } while (cVar1 != '\0');
    uVar7 = ~uVar7;
    pcVar10 = pcVar9 + -uVar7;
    pcVar9 = acStack_8c;
    for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
      *(undefined4 *)pcVar9 = *(undefined4 *)pcVar10;
      pcVar10 = pcVar10 + 4;
      pcVar9 = pcVar9 + 4;
    }
    for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
      *pcVar9 = *pcVar10;
      pcVar10 = pcVar10 + 1;
      pcVar9 = pcVar9 + 1;
    }
  }
  else {
    if (iVar2 == 3) {
      uVar3 = (**(code **)(*param_1 + 0x304))(0,&DAT_005e0120);
LAB_004cf5e1:
      pcVar10 = (char *)FUN_004dadd0_StName(uVar3);
      goto LAB_004cf5f0;
    }
    if (iVar2 == 0x19) {
      uVar3 = (**(code **)(*param_1 + 0x308))(0,&DAT_005e0124);
      goto LAB_004cf5e1;
    }
  }
  iVar2 = (**(code **)(*param_1 + 0x1f0))(acStack_8c,0);
  if (iVar2 == 0) {
    ExceptionList = local_c;
    return 0;
  }
  iVar2 = FUN_0057d9d0_FireScriptEvent(param_1,0x28,1,0,0);
  if (iVar2 != 0) {
    FUN_0057dc70_RunQueuedScriptEvent();
  }
  if (((int *)param_1[0x36] == (int *)0x0) ||
     (((iVar2 = *(int *)param_1[0x36], iVar2 != 2 && (iVar2 != 4)) && (iVar2 != 0x1a)))) {
    iVar2 = FUN_00482fb0(100);
    uStack_4 = 6;
    if (iVar2 == 0) {
      iVar2 = 0;
    }
    else {
      iVar2 = FUN_004daae0_TActionBlock_CopyCtor(param_1[0x36],acStack_8c,*(undefined4 *)param_1[0x38]);
    }
    uStack_4 = 0xffffffff;
    goto LAB_004cfae6;
  }
  pcVar10 = (char *)FUN_004dadd0_StName(acStack_8c,&DAT_005e0128);
  uVar7 = 0xffffffff;
  do {
    pcVar9 = pcVar10;
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    pcVar9 = pcVar10 + 1;
    cVar1 = *pcVar10;
    pcVar10 = pcVar9;
  } while (cVar1 != '\0');
  uVar7 = ~uVar7;
  pcVar10 = pcVar9 + -uVar7;
  pcVar9 = acStack_4c;
  for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
    *(undefined4 *)pcVar9 = *(undefined4 *)pcVar10;
    pcVar10 = pcVar10 + 4;
    pcVar9 = pcVar9 + 4;
  }
  for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
    *pcVar9 = *pcVar10;
    pcVar10 = pcVar10 + 1;
    pcVar9 = pcVar9 + 1;
  }
  pcVar10 = (char *)FUN_004dadd0_StName(acStack_8c,&DAT_005e012c);
  uVar7 = 0xffffffff;
  do {
    pcVar9 = pcVar10;
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    pcVar9 = pcVar10 + 1;
    cVar1 = *pcVar10;
    pcVar10 = pcVar9;
  } while (cVar1 != '\0');
  uVar7 = ~uVar7;
  pcVar10 = pcVar9 + -uVar7;
  pcVar9 = acStack_6c;
  for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
    *(undefined4 *)pcVar9 = *(undefined4 *)pcVar10;
    pcVar10 = pcVar10 + 4;
    pcVar9 = pcVar9 + 4;
  }
  for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
    *pcVar9 = *pcVar10;
    pcVar10 = pcVar10 + 1;
    pcVar9 = pcVar9 + 1;
  }
  pcVar10 = (char *)FUN_004dadd0_StName(acStack_8c,&DAT_005e0130);
  uVar7 = 0xffffffff;
  do {
    pcVar9 = pcVar10;
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    pcVar9 = pcVar10 + 1;
    cVar1 = *pcVar10;
    pcVar10 = pcVar9;
  } while (cVar1 != '\0');
  uVar7 = ~uVar7;
  pcVar10 = pcVar9 + -uVar7;
  pcVar9 = acStack_2c;
  for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
    *(undefined4 *)pcVar9 = *(undefined4 *)pcVar10;
    pcVar10 = pcVar10 + 4;
    pcVar9 = pcVar9 + 4;
  }
  for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
    *pcVar9 = *pcVar10;
    pcVar10 = pcVar10 + 1;
    pcVar9 = pcVar9 + 1;
  }
  uVar3 = FUN_004dadd0_StName(param_1[0x38] + 4,&DAT_005e0134);
  iVar2 = FUN_004dab80_TActionBlock_Is(uVar3);
  if (iVar2 == 0) {
    uVar3 = FUN_004dadd0_StName(param_1[0x38] + 4,&DAT_005e0138);
    iVar2 = FUN_004dab80_TActionBlock_Is(uVar3);
    if (iVar2 == 0) {
      iVar2 = (**(code **)(*param_1 + 0x1f0))(acStack_6c,0);
      if (iVar2 == 0) {
        iVar2 = FUN_00482fb0(100);
        uStack_4 = 5;
        goto LAB_004cf970;
      }
      pcVar10 = (char *)param_1[0x36];
      uVar7 = 0xffffffff;
      pcVar9 = acStack_4c;
      do {
        pcVar11 = pcVar9;
        if (uVar7 == 0) break;
        uVar7 = uVar7 - 1;
        pcVar11 = pcVar9 + 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar11;
      } while (cVar1 != '\0');
      uVar7 = ~uVar7;
      pcVar9 = pcVar11 + -uVar7;
      for (uVar8 = uVar7 >> 2; pcVar10 = pcVar10 + 4, uVar8 != 0; uVar8 = uVar8 - 1) {
        *(undefined4 *)pcVar10 = *(undefined4 *)pcVar9;
        pcVar9 = pcVar9 + 4;
      }
      for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
        *pcVar10 = *pcVar9;
        pcVar9 = pcVar9 + 1;
        pcVar10 = pcVar10 + 1;
      }
      iVar2 = FUN_00482fb0(100);
      uStack_4 = 4;
      if (iVar2 == 0) {
        iVar2 = 0;
      }
      else {
        iVar2 = FUN_004daae0_TActionBlock_CopyCtor((undefined4 *)param_1[0x36],acStack_6c,*(undefined4 *)param_1[0x36]);
      }
      uStack_4 = 0xffffffff;
      iVar4 = (**(code **)(*param_1 + 0x13c))(acStack_4c,acStack_6c,0xffffffff);
      if (-1 < iVar4) {
        FUN_0046e8a0();
        piVar5 = (int *)FUN_0046e8a0();
        iVar6 = (**(code **)(*piVar5 + 0x90))((short)param_1[3]);
        goto LAB_004cf930;
      }
    }
    else {
      iVar2 = (**(code **)(*param_1 + 0x1f0))(acStack_4c);
      if (iVar2 == 0) {
        iVar2 = FUN_00482fb0(100);
        uStack_4 = 3;
LAB_004cf970:
        if (iVar2 != 0) {
          iVar2 = FUN_004daae0_TActionBlock_CopyCtor((undefined4 *)param_1[0x36],acStack_2c,*(undefined4 *)param_1[0x36]);
          uStack_4 = 0xffffffff;
          *(uint *)(iVar2 + 0x60) = *(uint *)(iVar2 + 0x60) & 0xfffffdff;
          goto LAB_004cfae6;
        }
        goto LAB_004cfa8a;
      }
      pcVar10 = (char *)param_1[0x36];
      uVar7 = 0xffffffff;
      pcVar9 = acStack_6c;
      do {
        pcVar11 = pcVar9;
        if (uVar7 == 0) break;
        uVar7 = uVar7 - 1;
        pcVar11 = pcVar9 + 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar11;
      } while (cVar1 != '\0');
      uVar7 = ~uVar7;
      pcVar9 = pcVar11 + -uVar7;
      for (uVar8 = uVar7 >> 2; pcVar10 = pcVar10 + 4, uVar8 != 0; uVar8 = uVar8 - 1) {
        *(undefined4 *)pcVar10 = *(undefined4 *)pcVar9;
        pcVar9 = pcVar9 + 4;
      }
      for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
        *pcVar10 = *pcVar9;
        pcVar9 = pcVar9 + 1;
        pcVar10 = pcVar10 + 1;
      }
      iVar2 = FUN_00482fb0(100);
      uStack_4 = 2;
      if (iVar2 == 0) {
        iVar2 = 0;
      }
      else {
        iVar2 = FUN_004daae0_TActionBlock_CopyCtor((undefined4 *)param_1[0x36],acStack_4c,*(undefined4 *)param_1[0x36]);
      }
      uStack_4 = 0xffffffff;
      iVar4 = (**(code **)(*param_1 + 0x13c))(acStack_6c,acStack_4c,0xffffffff);
      if (-1 < iVar4) {
        FUN_0046e8a0();
        piVar5 = (int *)FUN_0046e8a0();
        iVar6 = (**(code **)(*piVar5 + 0x90))((short)param_1[3]);
LAB_004cf930:
        iVar4 = (**(code **)(*unaff_EBX + 0x90))(iVar4);
        *(int *)(iVar2 + 0x24) = (iVar4 * (short)param_1[0x17]) / iVar6;
        *(uint *)(iVar2 + 0x60) = *(uint *)(iVar2 + 0x60) & 0xfffffdff;
        goto LAB_004cfae6;
      }
    }
  }
  else {
    iVar2 = 0;
    iVar4 = (**(code **)(*param_1 + 0x1f0))(acStack_2c,0);
    if (iVar4 != 0) {
      iVar4 = FUN_00482fb0(100);
      uStack_4 = 0;
      if (iVar4 != 0) {
        iVar2 = FUN_004daae0_TActionBlock_CopyCtor((undefined4 *)param_1[0x36],acStack_2c,*(undefined4 *)param_1[0x36]);
      }
      uStack_4 = 0xffffffff;
      iVar4 = (**(code **)(*param_1 + 0x138))(acStack_2c,0xffffffff);
      if (-1 < iVar4) {
        FUN_0046e8a0();
        piVar5 = (int *)FUN_0046e8a0();
        iVar6 = (**(code **)(*piVar5 + 0x90))((short)param_1[3]);
        goto LAB_004cf930;
      }
      goto LAB_004cfa95;
    }
    pcVar10 = (char *)param_1[0x36];
    uVar7 = 0xffffffff;
    pcVar9 = acStack_4c;
    do {
      pcVar11 = pcVar9;
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      pcVar11 = pcVar9 + 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar11;
    } while (cVar1 != '\0');
    uVar7 = ~uVar7;
    pcVar9 = pcVar11 + -uVar7;
    for (uVar8 = uVar7 >> 2; pcVar10 = pcVar10 + 4, uVar8 != 0; uVar8 = uVar8 - 1) {
      *(undefined4 *)pcVar10 = *(undefined4 *)pcVar9;
      pcVar9 = pcVar9 + 4;
    }
    for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
      *pcVar10 = *pcVar9;
      pcVar9 = pcVar9 + 1;
      pcVar10 = pcVar10 + 1;
    }
    iVar2 = FUN_00482fb0(100);
    uStack_4 = 1;
    if (iVar2 != 0) {
      iVar2 = FUN_004daae0_TActionBlock_CopyCtor((undefined4 *)param_1[0x36],acStack_6c,*(undefined4 *)param_1[0x36]);
      uStack_4 = 0xffffffff;
      *(uint *)(iVar2 + 0x60) = *(uint *)(iVar2 + 0x60) & 0xfffffdff;
      goto LAB_004cfae6;
    }
LAB_004cfa8a:
    uStack_4 = 0xffffffff;
    iVar2 = 0;
  }
LAB_004cfa95:
  *(uint *)(iVar2 + 0x60) = *(uint *)(iVar2 + 0x60) & 0xfffffdff;
LAB_004cfae6:
  iVar4 = FUN_00482fb0(100);
  uStack_4 = 7;
  if (iVar4 == 0) {
    uVar3 = 0;
  }
  else {
    uVar3 = FUN_004daae0_TActionBlock_CopyCtor((undefined4 *)param_1[0x38],acStack_8c,*(undefined4 *)param_1[0x38]);
  }
  uStack_4 = 0xffffffff;
  (**(code **)(*param_1 + 0x1f8))(uVar3);
  piVar5 = (int *)param_1[0x36];
  iVar4 = *(int *)(iVar2 + 0x30);
  *(uint *)(iVar2 + 0x60) = *(uint *)(iVar2 + 0x60) | 0x20;
  *(int *)(iVar2 + 0x2c) = iVar4;
  if ((piVar5 != (int *)0x0) && (((iVar6 = *piVar5, iVar6 == 2 || (iVar6 == 4)) || (iVar6 == 0x1a)))
     ) {
    *(char *)((int)param_1 + 0x36) = (char)iVar4;
    param_1[0x2c] = iVar4;
  }
  (**(code **)(*param_1 + 0x208))(iVar2,0);
  ExceptionList = local_c;
  return 1;
}


