#include "TScreen.h"

// Decompiled methods and structure for class: TScreen



// Function at 00460620

undefined4 TScreen::meth_0x460620()

{
  dword *this_00;
  cls_0x45f7c0 *this_01;
  char cVar1;
  LPCVOID pvVar2;
  bool bVar3;
  bool bVar4;
  HANDLE pvVar5;
  uint *puVar6;
  uint uVar7;
  uint uVar8;
  uint extraout_ECX;
  int iVar9;
  undefined4 *puVar10;
  char *pcVar11;
  char *pcVar12;
  undefined4 *puVar13;
  uint *puVar14;
  byte bStack_430;
  undefined auStack_421 [5];
  uint auStack_41c [64];
  char acStack_31a [261];
  char cStack_215;
  undefined4 auStack_214 [64];
  char acStack_112 [262];
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059d121;
  pvStack_c = ExceptionList;
  this_00 = &this->complete;
  ExceptionList = &pvStack_c;
  this->nextscreen = 0xffffffff;
  this->firstframe = 0xffffffff;
  iVar9 = 0;
  if (0 < (int)*this_00) {
    do {
      if ((-1 < iVar9) &&
         (pvVar2 = *(LPCVOID *)(this->numexclusive + iVar9 * 4), pvVar2 != (LPCVOID)0x0)) {
        FUN_004616e0(pvVar2,1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar9 = iVar9 + 1;
    } while (iVar9 < (int)*this_00);
  }
  *this_00 = 0;
  this->curexclusive = 0;
  FUN_00483120((char *)&DAT_0065d6a4,acStack_31a + 2,0x104);
  uVar7 = 0xffffffff;
  pcVar11 = acStack_31a + 2;
  do {
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    cVar1 = *pcVar11;
    pcVar11 = pcVar11 + 1;
  } while (cVar1 != '\0');
  if (acStack_31a[~uVar7] == '\\') {
    uVar7 = 0xffffffff;
    pcVar11 = acStack_31a + 2;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *pcVar11;
      pcVar11 = pcVar11 + 1;
    } while (cVar1 != '\0');
    acStack_31a[~uVar7] = '\0';
  }
  CreateDirectoryA(acStack_31a + 2,(LPSECURITY_ATTRIBUTES)0x0);
  uVar7 = 0xffffffff;
  pcVar11 = acStack_31a + 2;
  do {
    pcVar12 = pcVar11;
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    pcVar12 = pcVar11 + 1;
    cVar1 = *pcVar11;
    pcVar11 = pcVar12;
  } while (cVar1 != '\0');
  uVar7 = ~uVar7;
  puVar10 = (undefined4 *)(pcVar12 + -uVar7);
  puVar13 = auStack_214;
  for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
    *puVar13 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar13 = puVar13 + 1;
  }
  for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
    *(undefined *)puVar13 = *(undefined *)puVar10;
    puVar10 = (undefined4 *)((int)puVar10 + 1);
    puVar13 = (undefined4 *)((int)puVar13 + 1);
  }
  uVar7 = 0xffffffff;
  puVar10 = auStack_214;
  do {
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    cVar1 = *(char *)puVar10;
    puVar10 = (undefined4 *)((int)puVar10 + 1);
  } while (cVar1 != '\0');
  iVar9 = -(~uVar7 - 1);
  pcVar11 = (char *)((int)auStack_214 + (~uVar7 - 1));
  _strncpy(pcVar11,&DAT_005d0d28,iVar9 + 0x103);
  pcVar11[iVar9 + 0x103] = '\0';
  pvVar5 = FUN_0058c680((LPCSTR)auStack_214,(uint *)&bStack_430);
  bVar3 = true;
  if (pvVar5 != (HANDLE)0xffffffff) {
    while (bVar3) {
      bVar4 = false;
      FUN_0059bd3e(auStack_41c);
      if (((bStack_430 & 0x10) == 0) || ((char)auStack_41c[0] == '.')) {
        puVar6 = FUN_0058ad30(auStack_41c,&DAT_005d0d30);
        if (puVar6 != (uint *)0x0) {
          FUN_0058b100(acStack_112 + 2,(byte *)s__s__s_005d0d38);
          iVar9 = cls_0x49ead0::meth_0x49ee20
                            ((cls_0x49ead0 *)&UNK_0065b2dc.field_0x1c,acStack_112 + 2);
          if (iVar9 == 0) {
            FUN_00481c10((byte *)s_Unable_to_open_module_file__s_005d0d40);
          }
          if ((UNK_0065b2dc._28_1_ & 8) != 0) {
            FUN_00481c10((byte *)s_Compressed_files_found_in_module_005d0d60);
          }
          uVar7 = 0xffffffff;
          bVar4 = true;
          puVar6 = auStack_41c;
          do {
            if (uVar7 == 0) break;
            uVar7 = uVar7 - 1;
            cVar1 = *(char *)puVar6;
            puVar6 = (uint *)((int)puVar6 + 1);
          } while (cVar1 != '\0');
          auStack_421[~uVar7] = 0;
          goto LAB_00460836;
        }
      }
      else {
LAB_00460836:
        puVar6 = FUN_00482fb0(0x920);
        uStack_4 = 0;
        if (puVar6 == (uint *)0x0) {
          puVar6 = (uint *)0x0;
        }
        else {
          cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)(puVar6 + 0x22),4);
          *puVar6 = 0;
          *(undefined *)(puVar6 + 0x16) = 0;
          *(undefined *)(puVar6 + 2) = 0;
          puVar6[0x21] = 0;
          uStack_4 = CONCAT31(uStack_4._1_3_,1);
          puVar14 = puVar6 + 0x27;
          for (iVar9 = 0x216; iVar9 != 0; iVar9 = iVar9 + -1) {
            *puVar14 = 0;
            puVar14 = puVar14 + 1;
          }
          puVar14 = puVar6 + 0x23d;
          for (iVar9 = 0xb; iVar9 != 0; iVar9 = iVar9 + -1) {
            *puVar14 = 0;
            puVar14 = puVar14 + 1;
          }
          cls_0x41c7f0::meth_0x461700((cls_0x41c7f0 *)(puVar6 + 0x22));
        }
        uStack_4 = 0xffffffff;
        iVar9 = FUN_0045f980(puVar6,(char *)auStack_41c,0);
        if (iVar9 == 0) {
          if (puVar6 != (uint *)0x0) {
            this_01 = (cls_0x45f7c0 *)(puVar6 + 0x22);
            iVar9 = 0;
            uStack_4 = 2;
            if (0 < (int)puVar6[0x22]) {
              do {
                if ((-1 < iVar9) &&
                   (pvVar2 = *(LPCVOID *)(puVar6[0x26] + iVar9 * 4), pvVar2 != (LPCVOID)0x0)) {
                  FUN_004830f0(pvVar2);
                }
                cls_0x45f7c0::meth_0x41cb40(this_01);
                iVar9 = iVar9 + 1;
              } while (iVar9 < (int)(this_01->cls_0x41c7f0).mbr_0x0);
            }
            pvVar2 = (LPCVOID)puVar6[0x26];
            (this_01->cls_0x41c7f0).mbr_0x0 = 0;
            puVar6[0x23] = 0;
            uStack_4 = 0xffffffff;
            FUN_004830f0(pvVar2);
            FUN_004830f0(puVar6);
          }
        }
        else {
          puVar6[1] = this->complete;
          cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->complete);
          uVar7 = FUN_0059a530(extraout_ECX,(byte *)(puVar6 + 0x16),(byte *)&DAT_0065c958);
          if (uVar7 == 0) {
            this->nextscreen = this->complete - 1;
          }
        }
        if (bVar4) {
          cls_0x49ead0::meth_0x49eff0((cls_0x49ead0 *)&UNK_0065b2dc.field_0x1c);
        }
      }
      iVar9 = FUN_0058c74d(pvVar5,(uint *)&bStack_430);
      if (iVar9 != 0) {
        bVar3 = false;
      }
    }
  }
  if ((int)this->complete < 1) {
    FUN_00481c10((byte *)s_There_are_no_modules_to_play_005d0d8c);
  }
  if ((int)this->nextscreen < 0) {
    FUN_00481c10((byte *)s_Unable_to_find_main_module___s__005d0dac);
  }
  this->exclusive = 1;
  ExceptionList = pvStack_c;
  return 1;
}



// Function at 00460dc0

undefined4 TScreen::meth_0x460dc0(char *param_1)

{
  char cVar1;
  uint uVar2;
  FILE *pFVar3;
  undefined4 uVar4;
  int iVar5;
  char *pcVar6;
  int in_stack_00000008;
  undefined4 local_414;
  char local_410 [258];
  char acStack_30e [260];
  char acStack_20a [260];
  char acStack_106 [262];
  
  local_414 = this;
  if ((this->exclusive != 0) && (iVar5 = 0, 0 < (int)this->complete)) {
    do {
      uVar2 = FUN_0059a530((uint)param_1,(byte *)(*(int *)(this->numexclusive + iVar5 * 4) + 0x58),
                           (byte *)param_1);
      if (uVar2 == 0) {
        if (*(int *)(this->numexclusive + iVar5 * 4) != 0) {
          return 0;
        }
        break;
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)this->complete);
  }
  _strncpy(local_410,(char *)&DAT_0065d6a4,0x103);
  uVar2 = 0xffffffff;
  acStack_30e[1] = 0;
  pcVar6 = local_410;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  iVar5 = -(~uVar2 - 1);
  _strncpy(local_410 + (~uVar2 - 1),param_1,iVar5 + 0x103);
  (local_410 + (~uVar2 - 1))[iVar5 + 0x103] = '\0';
  CreateDirectoryA(local_410,(LPSECURITY_ATTRIBUTES)0x0);
  _strncpy(acStack_30e + 2,local_410,0x103);
  uVar2 = 0xffffffff;
  acStack_20a[1] = 0;
  pcVar6 = acStack_30e + 2;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  iVar5 = -(~uVar2 - 1);
  _strncpy(acStack_30e + ~uVar2 + 1,&DAT_005d0e1c,iVar5 + 0x103);
  (acStack_30e + ~uVar2 + 1)[iVar5 + 0x103] = '\0';
  CreateDirectoryA(acStack_30e + 2,(LPSECURITY_ATTRIBUTES)0x0);
  _strncpy(acStack_20a + 2,local_410,0x103);
  uVar2 = 0xffffffff;
  acStack_106[1] = 0;
  pcVar6 = acStack_20a + 2;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  iVar5 = -(~uVar2 - 1);
  _strncpy(acStack_20a + ~uVar2 + 1,s__Automaps_005d0e24,iVar5 + 0x103);
  (acStack_20a + ~uVar2 + 1)[iVar5 + 0x103] = '\0';
  CreateDirectoryA(acStack_20a + 2,(LPSECURITY_ATTRIBUTES)0x0);
  pFVar3 = (FILE *)FUN_004819e0(s_module_def_005d0e30,local_410);
  FUN_0058b56e((char **)pFVar3,(byte *)s____________Revenant_Module_Descr_005d0e3c);
  FUN_0058b56e((char **)pFVar3,(byte *)s____Module_flags__use_with_FLAGS_t_005d0e74);
  FUN_0058b56e((char **)pFVar3,(byte *)s____You_can_use_multiple_flags_to_005d0e9c);
  FUN_0058b56e((char **)pFVar3,(byte *)s____DO_NOT_MODIFY_VALUES____005d0efc);
  FUN_0058b56e((char **)pFVar3,(byte *)s__define_GAMEMOD_SINGLEPLAYER_0x0_005d0f18);
  FUN_0058b56e((char **)pFVar3,(byte *)s__define_GAMEMOD_MULTIPLAYER_0x00_005d0f5c);
  FUN_0058b56e((char **)pFVar3,(byte *)s__define_GAMEMOD_DEATHMATCH_0x000_005d0fa0);
  FUN_0058b56e((char **)pFVar3,(byte *)s__define_GAMEMOD_MUD_0x0010____Th_005d0ff0);
  FUN_0058b56e((char **)pFVar3,(byte *)s__define_GAMEMOD_ZIPFILE_0x0020___005d1034);
  FUN_0058b56e((char **)pFVar3,(byte *)s__define_GAMEMOD_ISDEMO_0x0040____005d1090);
  FUN_0058b56e((char **)pFVar3,(byte *)s__define_GAMEMOD_SELFRUNNING_0x00_005d10f0);
  if (in_stack_00000008 == 1) {
    FUN_0058b56e((char **)pFVar3,(byte *)s____Session_flags__use_with_SESSI_005d1160);
    FUN_0058b56e((char **)pFVar3,(byte *)s____You_can_use_multiple_flags_to_005d1190);
    FUN_0058b56e((char **)pFVar3,(byte *)s____DO_NOT_MODIFY_VALUES____005d11f0);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_FLAGS_EDITSTATS_0_005d120c);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_FLAGS_EDITEQUIP_0_005d125c);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_FLAGS_LOOTCORPSES_005d12b0);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_FLAGS_NOMONSTERS_0_005d12f8);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_FLAGS_PASSREQUIRE_005d133c);
    FUN_0058b56e((char **)pFVar3,(byte *)s____Viewing_mode__use_with_VIEWIN_005d138c);
    FUN_0058b56e((char **)pFVar3,(byte *)s____DO_NOT_MODIFY_VALUES____005d13b4);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_VIEWING_SELECTABL_005d13d0);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_VIEWING_ALL_0x000_005d1418);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_VIEWING_NONE_0x00_005d145c);
    FUN_0058b56e((char **)pFVar3,(byte *)s____PKing_mode__use_with_PKING_ta_005d14a4);
    FUN_0058b56e((char **)pFVar3,(byte *)s____DO_NOT_MODIFY_VALUES____005d14c8);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_PKING_SELECTABLE_0_005d14e4);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_PKING_ALL_0x0001___005d1528);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_PKING_NONE_0x0002_005d1568);
    FUN_0058b56e((char **)pFVar3,(byte *)s____Access_mode__use_with_ACCESS_t_005d15a4);
    FUN_0058b56e((char **)pFVar3,(byte *)s____DO_NOT_MODIFY_VALUES____005d15cc);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_ACCESS_OPEN_0x000_005d15e8);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_ACCESS_NEWPLAYER_0_005d163c);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_ACCESS_GUEST_0x00_005d16a0);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_ACCESS_VALIDATE_0_005d16ec);
    FUN_0058b56e((char **)pFVar3,(byte *)s____Ressurection_penalty__use_wit_005d1748);
    FUN_0058b56e((char **)pFVar3,(byte *)s____DO_NOT_MODIFY_VALUES____005d177c);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_RESPENALTY_NONE_0_005d1798);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_RESPENALTY_ATPART_005d17d0);
    FUN_0058b56e((char **)pFVar3,(byte *)s__define_SESOPT_RESPENALTY_ALL_0x_005d1830);
  }
  else {
    FUN_0058b56e((char **)pFVar3,&DAT_005d1884);
  }
  FUN_0058b56e((char **)pFVar3,(byte *)s_MODULE___s_____Same_as_directory_005d1888);
  FUN_0058b56e((char **)pFVar3,(byte *)s_BEGIN_005d18b4);
  FUN_0058b56e((char **)pFVar3,(byte *)s____Primary_Information_005d18c0);
  FUN_0058b56e((char **)pFVar3,(byte *)s________________________005d18dc);
  FUN_0058b56e((char **)pFVar3,(byte *)s____A_descriptive_module_name__le_005d18f8);
  FUN_0058b56e((char **)pFVar3,(byte *)s_NAME__Your_short_description_goe_005d1934);
  FUN_0058b56e((char **)pFVar3,(byte *)s____Version_number_of_module_so_t_005d1964);
  FUN_0058b56e((char **)pFVar3,(byte *)s____Major_version_indicates_major_005d19b4);
  FUN_0058b56e((char **)pFVar3,(byte *)s_VERSION_1_0_005d1a14);
  FUN_0058b56e((char **)pFVar3,(byte *)s____Minimum_version_of_game_requi_005d1a28);
  FUN_0058b56e((char **)pFVar3,(byte *)s____Major_version_indicates_a_new_005d1a64);
  FUN_0058b56e((char **)pFVar3,(byte *)s_GAMEVERSION_1_0_005d1ac4);
  FUN_0058b56e((char **)pFVar3,(byte *)s____Unique_id_generated_by__genui_005d1ad8);
  FUN_00483300(0,-1);
  FUN_0058b56e((char **)pFVar3,(byte *)s_UNIQUEID__8x_005d1b40);
  FUN_0058b56e((char **)pFVar3,(byte *)s____Type_of_module__and_other_opt_005d1b54);
  if (in_stack_00000008 == 1) {
    FUN_0058b56e((char **)pFVar3,(byte *)s_FLAGS_GAMEMOD_MULTIPLAYER_005d1ba8);
    FUN_0058b56e((char **)pFVar3,(byte *)s____Default_Session_Settings_005d1bd0);
    FUN_0058b56e((char **)pFVar3,(byte *)s_____________________________005d1bf0);
    FUN_0058b56e((char **)pFVar3,(byte *)s____Basic_default_session_options_005d1c10);
    FUN_0058b56e((char **)pFVar3,(byte *)s_SESSIONFLAGS_SESOPT_FLAGS_EDITST_005d1c34);
    FUN_0058b56e((char **)pFVar3,(byte *)s____Viewing_players_via_the__View_005d1c78);
    FUN_0058b56e((char **)pFVar3,(byte *)s_VIEWING_SESOPT_VIEWING_ALL_005d1cc0);
    FUN_0058b56e((char **)pFVar3,(byte *)s____Player_killing_options_005d1ce0);
    FUN_0058b56e((char **)pFVar3,(byte *)s_PKING_SESOPT_PKING_SELECTABLE_005d1cfc);
    FUN_0058b56e((char **)pFVar3,(byte *)s____How_new_characters_enter_the_g_005d1d20);
    FUN_0058b56e((char **)pFVar3,(byte *)s_ACCESS_SESOPT_ACCESS_OPEN_005d1d48);
    FUN_0058b56e((char **)pFVar3,(byte *)s____What_kind_of_penalty_given_fo_005d1d68);
    FUN_0058b56e((char **)pFVar3,(byte *)s_RESPENALTY_SESOPT_RESPENALTY_ATP_005d1da4);
    FUN_0058b56e((char **)pFVar3,(byte *)s____Minimum_level_to_join_session_005d1dd8);
    FUN_0058b56e((char **)pFVar3,(byte *)s_MINLEVEL_1_005d1dfc);
    FUN_0058b56e((char **)pFVar3,(byte *)s____Maximum_level_to_join_session_005d1e0c);
    FUN_0058b56e((char **)pFVar3,(byte *)s_MAXLEVEL_5_005d1e30);
    FUN_0058b56e((char **)pFVar3,(byte *)s____Maximum_number_of_players_in_s_005d1e40);
    FUN_0058b56e((char **)pFVar3,(byte *)s_MAXPLAYERS_32_005d1e80);
    FUN_0058b56e((char **)pFVar3,(byte *)s____Maximum_players_online__is_se_005d1e94);
    FUN_0058b56e((char **)pFVar3,(byte *)s____than_MaxPlayers_in_the_ini_fi_005d1eec);
    FUN_0058b56e((char **)pFVar3,(byte *)s____more_players_online_simultane_005d1f44);
    FUN_0058b56e((char **)pFVar3,(byte *)s_MAXONLINE_32_005d1f70);
  }
  else {
    FUN_0058b56e((char **)pFVar3,(byte *)s_FLAGS_GAMEMOD_SINGLEPLAYER_005d1b7c);
  }
  FUN_0058b56e((char **)pFVar3,(byte *)s____Module_Information_005d1f84);
  FUN_0058b56e((char **)pFVar3,(byte *)s_______________________005d1f9c);
  FUN_0058b56e((char **)pFVar3,(byte *)s____Minimum_hours_to_complete_mod_005d1fb8);
  FUN_0058b56e((char **)pFVar3,(byte *)s_MINHOURS_1_005d1fe0);
  FUN_0058b56e((char **)pFVar3,(byte *)s____Maximum_hours_to_complete_mod_005d1ff0);
  FUN_0058b56e((char **)pFVar3,(byte *)s_MAXHOURS_10_005d2018);
  FUN_0058b56e((char **)pFVar3,(byte *)s____General_description_of_module_005d2028);
  FUN_0058b56e((char **)pFVar3,(byte *)s_DESC__Your_really__really__REALL_005d204c);
  FUN_0058b56e((char **)pFVar3,(byte *)s__You_can_continue_on_the_next_li_005d209c);
  FUN_0058b56e((char **)pFVar3,(byte *)s__with_a_BACKSLASH___005d20e4);
  if (in_stack_00000008 == 1) {
    FUN_0058b56e((char **)pFVar3,(byte *)s____Host_s_ip_address_for_MUD_mod_005d2100);
    FUN_0058b56e((char **)pFVar3,(byte *)s_MUDIP__0_0_0_0__005d2128);
    FUN_0058b56e((char **)pFVar3,(byte *)s____Starting_positions_005d2140);
    FUN_0058b56e((char **)pFVar3,(byte *)s_______________________005d2158);
    FUN_0058b56e((char **)pFVar3,(byte *)s_STARTPOS__Temp_Start_Point_1__LE_005d2174);
    FUN_0058b56e((char **)pFVar3,(byte *)s_STARTPOS__Temp_Start_Point_2__LE_005d21d4);
    FUN_0058b56e((char **)pFVar3,(byte *)s____Lists_different_starting_poin_005d2234);
  }
  FUN_0058b56e((char **)pFVar3,&DAT_005d226c);
  FUN_0058c899((int *)pFVar3);
  FUN_0058b4f1(pFVar3);
  pFVar3 = (FILE *)FUN_004819e0(s_area_def_005d2274,local_410);
  FUN_0058b56e((char **)pFVar3,(byte *)s____the_EAX_environments_are_as_f_005d2280);
  FUN_0058b56e((char **)pFVar3,&DAT_005d22c0);
  FUN_0058b56e((char **)pFVar3,(byte *)s____0__EAX_ENVIRONMENT_GENERIC_005d22c4);
  FUN_0058b56e((char **)pFVar3,(byte *)s____1__EAX_ENVIRONMENT_PADDEDCELL_005d22e8);
  FUN_0058b56e((char **)pFVar3,(byte *)s____2__EAX_ENVIRONMENT_ROOM_005d2310);
  FUN_0058b56e((char **)pFVar3,(byte *)s____3__EAX_ENVIRONMENT_BATHROOM_005d2330);
  FUN_0058b56e((char **)pFVar3,(byte *)s____4__EAX_ENVIRONMENT_LIVINGROOM_005d2354);
  FUN_0058b56e((char **)pFVar3,(byte *)s____5__EAX_ENVIRONMENT_STONEROOM_005d237c);
  FUN_0058b56e((char **)pFVar3,(byte *)s____6__EAX_ENVIRONMENT_AUDITORIUM_005d23a0);
  FUN_0058b56e((char **)pFVar3,(byte *)s____7__EAX_ENVIRONMENT_CONCERTHAL_005d23c8);
  FUN_0058b56e((char **)pFVar3,(byte *)s____8__EAX_ENVIRONMENT_CAVE_005d23f0);
  FUN_0058b56e((char **)pFVar3,(byte *)s____9__EAX_ENVIRONMENT_ARENA_005d2410);
  FUN_0058b56e((char **)pFVar3,(byte *)s____10__EAX_ENVIRONMENT_HANGAR_005d2430);
  FUN_0058b56e((char **)pFVar3,(byte *)s____11__EAX_ENVIRONMENT_CARPETEDH_005d2454);
  FUN_0058b56e((char **)pFVar3,(byte *)s____12__EAX_ENVIRONMENT_HALLWAY_005d2480);
  FUN_0058b56e((char **)pFVar3,(byte *)s____13__EAX_ENVIRONMENT_STONECORR_005d24a4);
  FUN_0058b56e((char **)pFVar3,(byte *)s____14__EAX_ENVIRONMENT_ALLEY_005d24cc);
  FUN_0058b56e((char **)pFVar3,(byte *)s____15__EAX_ENVIRONMENT_FOREST_005d24ec);
  FUN_0058b56e((char **)pFVar3,(byte *)s____16__EAX_ENVIRONMENT_CITY_005d2510);
  FUN_0058b56e((char **)pFVar3,(byte *)s____17__EAX_ENVIRONMENT_MOUNTAINS_005d2530);
  FUN_0058b56e((char **)pFVar3,(byte *)s____18__EAX_ENVIRONMENT_QUARRY_005d2554);
  FUN_0058b56e((char **)pFVar3,(byte *)s____19__EAX_ENVIRONMENT_PLAIN_005d2578);
  FUN_0058b56e((char **)pFVar3,(byte *)s____20__EAX_ENVIRONMENT_PARKINGLO_005d2598);
  FUN_0058b56e((char **)pFVar3,(byte *)s____21__EAX_ENVIRONMENT_SEWERPIPE_005d25c0);
  FUN_0058b56e((char **)pFVar3,(byte *)s____22__EAX_ENVIRONMENT_UNDERWATE_005d25e4);
  FUN_0058b56e((char **)pFVar3,(byte *)s____23__EAX_ENVIRONMENT_DRUGGED_005d260c);
  FUN_0058b56e((char **)pFVar3,(byte *)s____24__EAX_ENVIRONMENT_DIZZY_005d2630);
  FUN_0058b56e((char **)pFVar3,(byte *)s____25__EAX_ENVIRONMENT_PSYCHOTIC_005d2650);
  FUN_0058b56e((char **)pFVar3,(byte *)s____Tempoarary_Area_005d2678);
  FUN_0058b56e((char **)pFVar3,(byte *)s_AREA__The_Area__005d268c);
  FUN_0058b56e((char **)pFVar3,(byte *)s_BEGIN_005d26a0);
  FUN_0058b56e((char **)pFVar3,(byte *)s_CDPLAYLIST_RANDOM_TRACKS_1_2_4_5_005d26a8);
  FUN_0058b56e((char **)pFVar3,(byte *)s_LEVEL_0_005d26d0);
  FUN_0058b56e((char **)pFVar3,(byte *)s_AMBLIGHT_60_005d26dc);
  FUN_0058b56e((char **)pFVar3,(byte *)s_AMBCOLOR_250__250__250_005d26ec);
  FUN_0058b56e((char **)pFVar3,(byte *)s_NIGHTAMBLIGHT_25_005d2708);
  FUN_0058b56e((char **)pFVar3,(byte *)s_NIGHTAMBCOLOR_75__100__250_005d271c);
  FUN_0058b56e((char **)pFVar3,(byte *)s_AUDIOENV_15____EAX_ENVIRONMENT_F_005d273c);
  FUN_0058b56e((char **)pFVar3,&DAT_005d2764);
  FUN_0058c899((int *)pFVar3);
  FUN_0058b4f1(pFVar3);
  pFVar3 = (FILE *)FUN_004819e0(s_exit_def_005d276c,local_410);
  FUN_0058b4f1(pFVar3);
  pFVar3 = (FILE *)FUN_004819e0(s_location_def_005d2778,local_410);
  FUN_0058b4f1(pFVar3);
  pFVar3 = (FILE *)FUN_004819e0(s_state_def_005d2788,local_410);
  FUN_0058b56e((char **)pFVar3,(byte *)s____Place_state_variables_for_scr_005d2794);
  FUN_0058b56e((char **)pFVar3,(byte *)s_SOMESTATE_0_005d27cc);
  FUN_0058b56e((char **)pFVar3,(byte *)s_RESCUED_THE_PRINCESS_1_005d27dc);
  FUN_0058b56e((char **)pFVar3,(byte *)s_ACORNS_RETURNED_3_005d27f8);
  FUN_0058c899((int *)pFVar3);
  FUN_0058b4f1(pFVar3);
  FUN_0058b100(acStack_106 + 2,(byte *)s__s_def_005d280c);
  pFVar3 = (FILE *)FUN_004819e0(acStack_106 + 2,local_410);
  FUN_0058b56e((char **)pFVar3,(byte *)s____Place_dialog_lines_here__like_005d2814);
  FUN_0058b56e((char **)pFVar3,(byte *)s_ME1__This_is_a_dialog_line___005d2840);
  FUN_0058b56e((char **)pFVar3,(byte *)s_YOU1__I_can_see_that_____005d2860);
  FUN_0058b56e((char **)pFVar3,(byte *)s_ME2__No_you_can_t___005d287c);
  FUN_0058b56e((char **)pFVar3,(byte *)s_YOU2__Yes_I_can_____005d2894);
  FUN_0058c899((int *)pFVar3);
  FUN_0058b4f1(pFVar3);
  uVar4 = meth_0x460620(local_414);
  return uVar4;
}



