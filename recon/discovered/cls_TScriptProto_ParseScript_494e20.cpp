// FUN_00494e20_TScriptProto_ParseScript @ 00494e20 size=2284

undefined4 __thiscall FUN_00494e20_TScriptProto_ParseScript(undefined4 *param_1,int param_2)

{
  char cVar1;
  int iVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  char *pcVar6;
  undefined4 *puVar7;
  int *piVar8;
  char *pcVar9;
  int iStack_374;
  undefined4 *puStack_370;
  int aiStack_36c [2];
  char acStack_364 [19];
  undefined1 uStack_351;
  int iStack_350;
  int iStack_34c;
  int iStack_348;
  int iStack_344;
  int iStack_340;
  int iStack_33c;
  undefined4 uStack_338;
  char acStack_331 [41];
  int iStack_308;
  int iStack_304;
  int iStack_300;
  int iStack_2fc;
  int iStack_2f8;
  undefined1 auStack_2f4 [100];
  char acStack_290 [20];
  undefined1 auStack_27c [80];
  undefined1 auStack_22c [100];
  undefined1 auStack_1c8 [100];
  undefined1 auStack_164 [100];
  undefined1 auStack_100 [256];
  
  FUN_00497f90();
  param_1[0x11] = 0;
  FUN_00479680();
  iVar2 = (**(code **)(**(int **)(param_2 + 0xc) + 0xc))();
  uVar4 = 0xffffffff;
  pcVar6 = *(char **)(param_2 + 0x28);
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  puStack_370 = (undefined4 *)((iVar2 - (~uVar4 - 1)) + -1);
  FUN_00494c50_TScriptProto_ParseCriteria(param_2);
  FUN_00479680();
  iVar2 = FUN_00479700(s_BEGIN_005da27c,0);
  if (iVar2 == 0) {
    FUN_0058b100(aiStack_36c,s_Script_error_at_line__d___s_005da0d0,*(undefined4 *)(param_2 + 0x30),
                 s_Object_block_BEGIN_expected_005da284);
    if (DAT_00668154 == 0) {
      FUN_0054d170(&DAT_0065c5d0,aiStack_36c);
    }
    else {
      FUN_0041ee50(aiStack_36c);
    }
  }
  iVar2 = *(int *)(param_2 + 0x10);
  while ((iVar2 != 9 && (iVar2 != 10))) {
    FUN_00478a10();
    iVar2 = *(int *)(param_2 + 0x10);
  }
  FUN_004795a0();
  iVar2 = *(int *)(param_2 + 0x10);
  do {
    if ((iVar2 == 10) || (iVar2 = FUN_00479700(&DAT_005da2a0,0), iVar2 != 0)) {
      iVar2 = FUN_00479700(&DAT_005da410,0);
      if (iVar2 == 0) {
        FUN_0058b100(auStack_2f4,s_Script_error_at_line__d___s_005da0d0,
                     *(undefined4 *)(param_2 + 0x30),s_Object_block_END_expected_005da414);
        if (DAT_00668154 == 0) {
          FUN_0054d170(&DAT_0065c5d0,auStack_2f4);
        }
        else {
          FUN_0041ee50(auStack_2f4);
        }
      }
      FUN_00479580();
      if (*(int *)(param_2 + 0x10) != 9) {
        FUN_0058b100(auStack_100,s_RETURN_expected_in__s__s_005da430,param_1[0xf],
                     *(undefined4 *)(param_2 + 0x28));
        FUN_0058b100(auStack_2f4,s_Script_error_at_line__d___s_005da0d0,
                     *(undefined4 *)(param_2 + 0x30),auStack_100);
        if (DAT_00668154 == 0) {
          FUN_0054d170(&DAT_0065c5d0,auStack_2f4);
        }
        else {
          FUN_0041ee50(auStack_2f4);
        }
      }
      iVar2 = (**(code **)(**(int **)(param_2 + 0xc) + 0xc))();
      puVar7 = puStack_370;
      if (iVar2 - (int)puStack_370 < 0) {
        iVar2 = 0;
      }
      else {
        iVar2 = (**(code **)(**(int **)(param_2 + 0xc) + 0xc))();
        iVar2 = iVar2 - (int)puVar7;
      }
      param_1[0x10] = iVar2;
      if (param_1[1] != 0) {
        FUN_004830f0(param_1[1]);
        param_1[1] = 0;
      }
      puVar3 = (undefined4 *)FUN_00482fb0(param_1[0x10] + 1);
      uVar4 = param_1[0x10];
      param_1[1] = puVar3;
      puVar7 = puStack_370;
      for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
        *puVar3 = *puVar7;
        puVar7 = puVar7 + 1;
        puVar3 = puVar3 + 1;
      }
      for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
        *(undefined1 *)puVar3 = *(undefined1 *)puVar7;
        puVar7 = (undefined4 *)((int)puVar7 + 1);
        puVar3 = (undefined4 *)((int)puVar3 + 1);
      }
      *(undefined1 *)(param_1[1] + param_1[0x10]) = 0;
      FUN_004795a0();
      return param_1[0x10];
    }
    if (*(int *)(param_2 + 0x10) != 4) {
      FUN_0058b100(auStack_1c8,s_Script_error_at_line__d___s_005da0d0,
                   *(undefined4 *)(param_2 + 0x30),s_Trigger_identifier_expected_005da2a4);
      if (DAT_00668154 == 0) {
        FUN_0054d170(&DAT_0065c5d0,auStack_1c8);
      }
      else {
        FUN_0041ee50(auStack_1c8);
      }
    }
    piVar8 = aiStack_36c;
    for (iVar2 = 0x14; iVar2 != 0; iVar2 = iVar2 + -1) {
      *piVar8 = 0;
      piVar8 = piVar8 + 1;
    }
    uVar4 = 0xffffffff;
    aiStack_36c[0] = 0;
    pcVar6 = *(char **)(param_2 + 0x28);
    do {
      pcVar9 = pcVar6;
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      pcVar9 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar9;
    } while (cVar1 != '\0');
    uVar4 = ~uVar4;
    pcVar6 = pcVar9 + -uVar4;
    pcVar9 = acStack_290;
    for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
      *(undefined4 *)pcVar9 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar9 = pcVar9 + 4;
    }
    for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *pcVar9 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar9 = pcVar9 + 1;
    }
    iVar2 = FUN_00479700(s_ALWAYS_005da2c0,0);
    if (iVar2 == 0) {
      iVar2 = FUN_00479700(s_TRIGGER_005da2c8,0);
      if (iVar2 != 0) {
        FUN_00479580();
        iVar2 = *(int *)(param_2 + 0x10);
        aiStack_36c[0] = 2;
joined_r0x00494fe5:
        if ((iVar2 == 4) || (iVar2 == 2)) {
          pcVar6 = *(char **)(param_2 + 0x28);
          goto LAB_00494ffb;
        }
        goto LAB_004953cb;
      }
      iVar2 = FUN_00479700(s_DIALOG_005da2d0,0);
      if (iVar2 != 0) {
        FUN_00479580();
        aiStack_36c[0] = 3;
        goto LAB_004953cb;
      }
      iVar2 = FUN_00479700(s_PROXIMITY_005da2d8,0);
      if (iVar2 != 0) {
        FUN_00479580();
        aiStack_36c[0] = 4;
        uStack_338 = 0x100;
        if ((*(int *)(param_2 + 0x10) == 4) || (*(int *)(param_2 + 0x10) == 2)) {
          _strncpy(acStack_364,*(char **)(param_2 + 0x28),0x13);
          uStack_351 = 0;
          FUN_00479580();
        }
        if (*(int *)(param_2 + 0x10) == 8) {
          FUN_0047a410(param_2,&DAT_005da2e4,&uStack_338);
        }
        goto LAB_004953cb;
      }
      iVar2 = FUN_00479700(&DAT_005da2e8,0);
      if (iVar2 != 0) {
        FUN_00479580();
        aiStack_36c[0] = 5;
        if ((*(int *)(param_2 + 0x10) == 4) || (*(int *)(param_2 + 0x10) == 2)) {
          iVar2 = FUN_00479700(&DAT_005da2f0,0);
          if (iVar2 == 0) {
            _strncpy(acStack_364,*(char **)(param_2 + 0x28),0x13);
            uStack_351 = 0;
          }
          FUN_00479580();
        }
        else {
          _strncpy(acStack_364,(char *)*param_1,0x13);
          uStack_351 = 0;
        }
        if ((*(int *)(param_2 + 0x10) == 4) || (*(int *)(param_2 + 0x10) == 2)) {
          uVar4 = 0xffffffff;
          pcVar6 = *(char **)(param_2 + 0x28);
          do {
            if (uVar4 == 0) break;
            uVar4 = uVar4 - 1;
            cVar1 = *pcVar6;
            pcVar6 = pcVar6 + 1;
          } while (cVar1 != '\0');
          _strncpy(acStack_331 + 1,*(char **)(param_2 + 0x28),~uVar4 - 1);
          acStack_331[~uVar4] = '\0';
          iStack_350 = 0;
          iStack_34c = 0;
          iStack_348 = 0;
          iStack_344 = 0;
          iStack_340 = 0;
          iStack_33c = 0;
          FUN_00478a10();
        }
        else {
          iVar2 = FUN_0047a410(param_2,s__i__i__i__i__i__i_005da2f8,&iStack_374,&iStack_2fc,
                               &iStack_2f8,&iStack_304,&iStack_300,&iStack_308);
          if (iVar2 == 0) {
            FUN_0058b100(auStack_164,s_Script_error_at_line__d___s_005da0d0,
                         *(undefined4 *)(param_2 + 0x30),s_Invalid_cube_trigger_005da30c);
            if (DAT_00668154 == 0) {
              FUN_0054d170(&DAT_0065c5d0,auStack_164);
            }
            else {
              FUN_0041ee50(auStack_164);
            }
          }
          iStack_350 = iStack_374;
          if (iStack_304 <= iStack_374) {
            iStack_350 = iStack_304;
          }
          iStack_34c = iStack_2fc;
          if (iStack_300 <= iStack_2fc) {
            iStack_34c = iStack_300;
          }
          iStack_348 = iStack_2f8;
          if (iStack_308 <= iStack_2f8) {
            iStack_348 = iStack_308;
          }
          iStack_344 = iStack_304;
          if (iStack_304 < iStack_374) {
            iStack_344 = iStack_374;
          }
          iStack_340 = iStack_2fc;
          if (iStack_2fc <= iStack_300) {
            iStack_340 = iStack_300;
          }
          if (iStack_308 < iStack_2f8) {
            iStack_33c = iStack_2f8;
          }
          else {
            iStack_33c = iStack_308;
          }
        }
        goto LAB_004953cb;
      }
      iVar2 = FUN_00479700(s_ACTIVATE_005da324,0);
      if (iVar2 != 0) {
        FUN_00479580();
        aiStack_36c[0] = 6;
        goto LAB_004953cb;
      }
      iVar2 = FUN_00479700(&PTR_LAB_00455351_4_005da330,0);
      if (iVar2 != 0) {
        FUN_00479580();
        aiStack_36c[0] = 7;
        if ((*(int *)(param_2 + 0x10) == 4) || (*(int *)(param_2 + 0x10) == 2)) {
          pcVar6 = *(char **)(param_2 + 0x28);
LAB_00494ffb:
          _strncpy(acStack_364,pcVar6,0x13);
          uStack_351 = 0;
          FUN_00479580();
        }
        else {
          _strncpy(acStack_364,(char *)*param_1,0x13);
          uStack_351 = 0;
        }
        goto LAB_004953cb;
      }
      iVar2 = FUN_00479700(&DAT_005da334,0);
      if (iVar2 != 0) {
        FUN_00479580();
        iVar2 = *(int *)(param_2 + 0x10);
        aiStack_36c[0] = 8;
        goto joined_r0x00494fe5;
      }
      iVar2 = FUN_00479700(&DAT_005da33c,0);
      if (iVar2 != 0) {
        FUN_00479580();
        aiStack_36c[0] = 9;
        if ((*(int *)(param_2 + 0x10) == 4) || (*(int *)(param_2 + 0x10) == 2)) {
          pcVar6 = *(char **)(param_2 + 0x28);
          goto LAB_00494ffb;
        }
        goto LAB_004953cb;
      }
      iVar2 = FUN_00479700(s_COMBAT_005da340,0);
      if (iVar2 != 0) {
        FUN_00479580();
        aiStack_36c[0] = 10;
        goto LAB_004953cb;
      }
      iVar2 = FUN_00479700(&DAT_005da348,0);
      if (iVar2 != 0) {
        FUN_00479580();
        aiStack_36c[0] = 0xb;
        goto LAB_004953cb;
      }
      iVar2 = FUN_00479700(&DAT_005da350,0);
      if (iVar2 == 0) {
        FUN_0058b100(auStack_27c,s_LOAD_STOPPED___Unknown_trigger___005da358,
                     *(undefined4 *)(param_2 + 0x28),*param_1);
        FUN_0058b100(auStack_2f4,s_Script_error_at_line__d___s_005da0d0,
                     *(undefined4 *)(param_2 + 0x30),auStack_27c);
        if (DAT_00668154 == 0) {
          FUN_0054d170(&DAT_0065c5d0,auStack_2f4);
        }
        else {
          FUN_0041ee50(auStack_2f4);
        }
        FUN_004820b0(s_Error_in_script_file_line__d___s_005da380,*(undefined4 *)(param_2 + 0x30),
                     *param_1);
        return 0xffffffff;
      }
      FUN_00495750(param_2);
    }
    else {
      FUN_00479580();
      aiStack_36c[0] = 1;
LAB_004953cb:
      if (*(int *)(param_2 + 0x10) != 9) {
        FUN_0058b100(auStack_27c,s_Bad_parameter_for__s_trigger_005da3d0,acStack_290);
        FUN_0058b100(auStack_22c,s_Script_error_at_line__d___s_005da0d0,
                     *(undefined4 *)(param_2 + 0x30),auStack_27c);
        if (DAT_00668154 == 0) {
          FUN_0054d170(&DAT_0065c5d0,auStack_22c);
        }
        else {
          FUN_0041ee50(auStack_22c);
        }
        FUN_004795c0();
      }
      aiStack_36c[1] = (**(code **)(**(int **)(param_2 + 0xc) + 0xc))();
      aiStack_36c[1] = aiStack_36c[1] - (int)puStack_370;
      FUN_00479680();
      if (aiStack_36c[0] != 0) {
        FUN_00497f60(aiStack_36c);
        param_1[0x11] = param_1[0x11] + 1;
      }
      iVar2 = FUN_00479700(s_BEGIN_005da3f0,0);
      if (iVar2 == 0) {
        FUN_0058b100(auStack_2f4,s_Script_error_at_line__d___s_005da0d0,
                     *(undefined4 *)(param_2 + 0x30),s_Trigger_BEGIN_expected_005da3f8);
        if (DAT_00668154 == 0) {
          FUN_0054d170(&DAT_0065c5d0,auStack_2f4);
        }
        else {
          FUN_0041ee50(auStack_2f4);
        }
        FUN_004795c0();
      }
      FUN_004795f0();
      FUN_00479680();
    }
    iVar2 = *(int *)(param_2 + 0x10);
  } while( true );
}


