// Decompiled methods and structure for class: cls_0x48b690

/*
/OOAnalyzer/cls_0x48b690
pack(disabled)
Structure cls_0x48b690 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   104   dword   4   mbr_0x68   "Unsigned Double-Word (ddw, 4-bytes)"
   108   dword   4   mbr_0x6c   "Unsigned Double-Word (ddw, 4-bytes)"
   112   dword   4   mbr_0x70   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   132   dword   4   mbr_0x84   "Unsigned Double-Word (ddw, 4-bytes)"
   136   dword   4   mbr_0x88   "Unsigned Double-Word (ddw, 4-bytes)"
   140   dword   4   mbr_0x8c   "Unsigned Double-Word (ddw, 4-bytes)"
   144   dword   4   mbr_0x90   "Unsigned Double-Word (ddw, 4-bytes)"
   148   dword   4   mbr_0x94   "Unsigned Double-Word (ddw, 4-bytes)"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   160   dword   4   mbr_0xa0   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
   168   dword   4   mbr_0xa8   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   208   dword   4   mbr_0xd0   "Unsigned Double-Word (ddw, 4-bytes)"
   212   dword   4   mbr_0xd4   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   220   dword   4   mbr_0xdc   "Unsigned Double-Word (ddw, 4-bytes)"
   224   dword   4   mbr_0xe0   "Unsigned Double-Word (ddw, 4-bytes)"
   228   dword   4   mbr_0xe4   "Unsigned Double-Word (ddw, 4-bytes)"
   232   dword   4   mbr_0xe8   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   240   dword   4   mbr_0xf0   "Unsigned Double-Word (ddw, 4-bytes)"
   244   dword   4   mbr_0xf4   "Unsigned Double-Word (ddw, 4-bytes)"
   356   dword   4   mbr_0x164   "Unsigned Double-Word (ddw, 4-bytes)"
   360   dword   4   mbr_0x168   "Unsigned Double-Word (ddw, 4-bytes)"
   476   dword   4   mbr_0x1dc   "Unsigned Double-Word (ddw, 4-bytes)"
   480   dword   4   mbr_0x1e0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 484 Alignment: 1

*/

// Function at 0048b690

undefined4 __thiscall OOAnalyzer::cls_0x48b690::meth_0x48b690(cls_0x48b690 *this)

{
  dword *pdVar1;
  int iVar2;
  int iVar3;
  
  if (this->mbr_0x0 != 0) {
    return 1;
  }
  pdVar1 = &this->mbr_0x18;
  iVar3 = 0;
  if (0 < (int)this->mbr_0x18) {
    do {
      if ((-1 < iVar3) && (iVar2 = *(int *)(this->mbr_0x28 + iVar3 * 4), iVar2 != 0)) {
        FUN_004894f0();
        FUN_004830f0(iVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  this->mbr_0x1c = 0;
  pdVar1 = &this->mbr_0x4;
  iVar3 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      if ((-1 < iVar3) && (iVar2 = *(int *)(this->mbr_0x14 + iVar3 * 4), iVar2 != 0)) {
        FUN_004830f0(iVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x54 = 0;
  cls_0x49c9b0::meth_0x49c9b0((cls_0x49c9b0 *)&this->mbr_0x58);
  this->mbr_0x98 = 10;
  this->mbr_0x9c = 10;
  this->mbr_0x88 = 1;
  this->mbr_0x80 = 1;
  this->mbr_0x84 = 1;
  this->mbr_0x90 = 1;
  this->mbr_0x94 = 0x32;
  this->mbr_0xa0 = 0x19;
  this->mbr_0xa4 = 0x19;
  this->mbr_0xdc = 0x14;
  this->mbr_0xe0 = 6;
  this->mbr_0xe4 = 4;
  this->mbr_0xe8 = 1;
  this->mbr_0xec = 0x19;
  iVar3 = meth_0x48b990(this);
  if (iVar3 != 0) {
    this->mbr_0x0 = 1;
    iVar3 = 0;
    do {
      if (iVar3 == 0) {
        this->mbr_0xf0 = 0;
      }
      else if (iVar3 == 1) {
        this->mbr_0xf4 = 300;
      }
      else {
        (&this->mbr_0xf0)[iVar3] = (&this->mbr_0xec)[iVar3] + (iVar3 * 5 + 10) * 0x14;
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < 0x1e);
    iVar3 = 0;
    do {
      if (iVar3 == 0) {
        this->mbr_0x168 = 300;
      }
      else {
        (&this->mbr_0x168)[iVar3] = (&this->mbr_0x164)[iVar3] + (iVar3 * 5 + 0xf) * 0x14;
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < 0x1e);
    iVar3 = 0;
    do {
      if (iVar3 == 0) {
        this->mbr_0x1e0 = 500;
      }
      else {
        (&this->mbr_0x1e0)[iVar3] = (&this->mbr_0x1dc)[iVar3] + 500 + iVar3 * 2000;
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < 0x1e);
    return 1;
  }
  return 0;
}



// Function at 0048b990

undefined4 __thiscall OOAnalyzer::cls_0x48b690::meth_0x48b990(cls_0x48b690 *this)

{
  void **ppvVar1;
  dword dVar2;
  undefined4 *puVar3;
  int iVar4;
  int iVar5;
  cls_0x45f7c0 *this_00;
  int iVar6;
  dword *pdVar7;
  char *pcVar8;
  dword *pdVar9;
  undefined *puVar10;
  char *pcVar11;
  dword *pdVar12;
  dword local_2b0;
  int local_2ac;
  undefined4 local_2a8;
  int local_2a4;
  int local_2a0;
  undefined4 local_29c;
  int local_298;
  undefined4 local_294;
  undefined4 local_290;
  undefined4 local_288;
  undefined4 local_284;
  char *local_280;
  undefined local_27c;
  undefined4 local_278;
  cls_0x478720 local_270;
  char local_268 [39];
  undefined local_241;
  dword local_240;
  cls_0x48b690 *local_23c;
  dword local_238;
  cls_0x41c7f0 *local_234;
  undefined local_230 [260];
  undefined local_12c [32];
  undefined local_10c [64];
  undefined local_cc [64];
  undefined local_8c [64];
  undefined local_4c [64];
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d896;
  local_c = ExceptionList;
  iVar6 = 0;
  pdVar7 = &this->mbr_0x18;
  ExceptionList = &local_c;
  ppvVar1 = &local_c;
  local_23c = this;
  if (0 < (int)this->mbr_0x18) {
    do {
      if ((-1 < iVar6) && (iVar5 = *(int *)(this->mbr_0x28 + iVar6 * 4), iVar5 != 0)) {
        FUN_004894f0();
        FUN_004830f0(iVar5);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar7);
      iVar6 = iVar6 + 1;
      ppvVar1 = (void **)ExceptionList;
    } while (iVar6 < (int)*pdVar7);
  }
  ExceptionList = ppvVar1;
  *pdVar7 = 0;
  this->mbr_0x1c = 0;
  pdVar7 = &this->mbr_0x4;
  iVar6 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      if ((-1 < iVar6) && (iVar5 = *(int *)(this->mbr_0x14 + iVar6 * 4), iVar5 != 0)) {
        FUN_004830f0(iVar5);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar7);
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)*pdVar7);
  }
  *pdVar7 = 0;
  this->mbr_0x8 = 0;
  pdVar7 = &this->mbr_0x2c;
  iVar6 = 0;
  if (0 < (int)this->mbr_0x2c) {
    do {
      if ((-1 < iVar6) && (iVar5 = *(int *)(this->mbr_0x3c + iVar6 * 4), iVar5 != 0)) {
        FUN_0048ac00();
        FUN_004830f0(iVar5);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar7);
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)*pdVar7);
  }
  *pdVar7 = 0;
  this->mbr_0x30 = 0;
  pdVar7 = &this->mbr_0x40;
  iVar6 = 0;
  if (0 < (int)this->mbr_0x40) {
    do {
      if ((-1 < iVar6) && (iVar5 = *(int *)(this->mbr_0x50 + iVar6 * 4), iVar5 != 0)) {
        FUN_0048b170();
        FUN_004830f0(iVar5);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar7);
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)*pdVar7);
  }
  *pdVar7 = 0;
  this->mbr_0x44 = 0;
  local_2ac = 0;
  do {
    if (local_2ac == 0) {
      FUN_0058b100(local_230,&DAT_005d98d8,0x65bd48,s_rules_def_005d98cc);
      goto LAB_0048bca8;
    }
    if (local_2ac == 1) {
      pcVar8 = s_stats_def_005d98e0;
      puVar10 = &DAT_005d98ec;
LAB_0048bb03:
      FUN_0058b100(local_230,puVar10,0x65bd48,pcVar8);
LAB_0048bb19:
      iVar6 = FUN_004a1c00(local_230,0);
      if (iVar6 != 0) goto LAB_0048bca8;
    }
    else {
      if (local_2ac == 2) {
        FUN_0058b100(local_230,&DAT_005d9900,&DAT_0065bc44,s_char_def_005d98f4);
        iVar6 = FUN_004a1c00(local_230,0);
        if (iVar6 == 0) {
          FUN_0058b100(local_230,&DAT_005d9914,0x65bd48,s_char_def_005d9908);
          goto LAB_0048bb19;
        }
      }
      else if (local_2ac == 3) {
        FUN_0058b100(local_230,&DAT_005d9928,&DAT_0065bc44,s_weapon_def_005d991c);
        iVar6 = FUN_004a1c00(local_230,0);
        if (iVar6 == 0) {
          FUN_0058b100(local_230,&DAT_005d993c,0x65bd48,s_weapon_def_005d9930);
          goto LAB_0048bb19;
        }
      }
      else if (local_2ac == 4) {
        FUN_0058b100(local_230,&DAT_005d9950,&DAT_0065bc44,s_armor_def_005d9944);
        iVar6 = FUN_004a1c00(local_230,0);
        if (iVar6 == 0) {
          pcVar8 = s_armor_def_005d9958;
          puVar10 = &DAT_005d9964;
          goto LAB_0048bb03;
        }
      }
      else if (local_2ac == 5) {
        FUN_0058b100(local_230,&DAT_005d9978,&DAT_0065bc44,s_equip_def_005d996c);
        iVar6 = FUN_004a1c00(local_230,0);
        if (iVar6 == 0) {
          FUN_0058b100(local_230,&DAT_005d998c,0x65bd48,s_equip_def_005d9980);
          iVar6 = FUN_004a1c00(local_230,0);
          if (iVar6 == 0) goto LAB_0048c68c;
        }
      }
LAB_0048bca8:
      cls_0x478720::cls_0x478720(&local_270);
      local_4 = 0;
      local_2a8 = 0;
      local_2a4 = 0;
      local_2a0 = 0;
      local_29c = 0;
      local_298 = 0;
      local_294 = 0;
      local_290 = 0;
      local_288 = 0;
      local_284 = 0;
      local_27c = 0;
      local_278 = 1;
      local_280 = (char *)FUN_00482fb0(0x2000);
      *local_280 = '\0';
      local_4 = 1;
      iVar6 = FUN_004789c0(local_230);
      if (iVar6 == 0) {
        local_4 = 2;
        FUN_004830f0(local_280);
        if (local_2a4 == 0) {
          if (local_2a0 != 0) {
            FUN_004830f0(local_29c);
            FUN_004a1540(local_2a0);
          }
        }
        else {
          FUN_004830f0(local_29c);
          FUN_004830f0(local_2a4);
        }
        local_4 = 0xffffffff;
        cls_0x478720::meth_0x478730(&local_270);
        ExceptionList = local_c;
        return 0;
      }
      iVar6 = FUN_00479450();
      if (iVar6 == 0) {
        FUN_00479950(s_Syntax_error_in_header_005d9994,0);
      }
      while (local_298 != 10) {
        if (local_298 != 4) {
          FUN_00479950(s_Rules_block_name_or_tag_expected_005d99ac,0);
        }
        _strncpy(local_268,local_280,0x27);
        local_241 = 0;
        FUN_00479580();
        iVar6 = FUN_0059a530(local_268,s_DAYLENGTH_005d99d0);
        if (iVar6 == 0) {
          pdVar7 = &this->mbr_0x5c;
          puVar10 = &DAT_005d99dc;
          goto LAB_0048bdb2;
        }
        iVar6 = FUN_0059a530(local_268,s_TWILIGHT_005d99e0);
        if (iVar6 == 0) {
          iVar6 = FUN_0047a410(&local_2a8,s__i___i_005d99ec,&this->mbr_0x60,&this->mbr_0x64);
          if (DAT_005d79e4 == 5) {
            dVar2 = FUN_0047eb50(this->mbr_0x60);
            this->mbr_0x64 = dVar2;
          }
LAB_0048bdbc:
          if (iVar6 == 0) {
LAB_0048bdc0:
            pcVar11 = s_Error_parsing_tag__s_005d8dfc;
            pcVar8 = local_268;
            goto LAB_0048bdca;
          }
        }
        else {
          iVar6 = FUN_0059a530(local_268,s_STEALTH_005d99f4);
          if (iVar6 == 0) {
            pdVar12 = &this->mbr_0xd8;
            pdVar7 = &this->mbr_0xd4;
            pdVar9 = &this->mbr_0xd0;
            pcVar8 = s__i___i___i_005d99fc;
LAB_0048bee5:
            iVar6 = FUN_0047a410(&local_2a8,pcVar8,pdVar9,pdVar7,pdVar12);
            goto LAB_0048bdbc;
          }
          iVar6 = FUN_0059a530(local_268,s_CHARACTER_005d9a08);
          if (iVar6 == 0) {
            iVar6 = FUN_0047a410(&local_2a8,&DAT_005d9a14,local_cc);
            if (iVar6 != 0) {
              local_2b0 = FUN_00482fb0(0x590);
              local_4._0_1_ = 3;
              if (local_2b0 == 0) {
                local_2b0 = 0;
              }
              else {
                local_234 = (cls_0x41c7f0 *)(local_2b0 + 0xcc);
                cls_0x41c7f0::cls_0x41c7f0(local_234,0x10);
                local_4._0_1_ = 4;
                puVar3 = (undefined4 *)FUN_00482fb0(800);
                *(undefined4 **)(local_2b0 + 0xe0) = puVar3;
                for (iVar5 = 200; iVar5 != 0; iVar5 = iVar5 + -1) {
                  *puVar3 = 0;
                  puVar3 = puVar3 + 1;
                }
                local_4._0_1_ = 5;
                FUN_004895d0();
                this = local_23c;
              }
              iVar5 = 0;
              local_4 = CONCAT31(local_4._1_3_,1);
              if (0 < (int)this->mbr_0x18) {
                do {
                  iVar4 = FUN_0059a530(*(undefined4 *)(this->mbr_0x28 + iVar5 * 4),local_cc);
                  if (iVar4 == 0) {
                    FUN_00479950(s_More_than_one__s_in_CHAR_DEF_fil_005d9a18,local_cc);
                  }
                  iVar5 = iVar5 + 1;
                } while (iVar5 < (int)this->mbr_0x18);
              }
              iVar5 = FUN_00489850(local_cc,&local_2a8);
              if (iVar5 == 0) {
                FUN_00479950(s_Error_loading_char_data_005d9a3c,0);
              }
              cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x18);
              if (*(int *)(local_2b0 + 0xc0) == -1) {
                this->mbr_0x54 = local_2b0;
              }
              FUN_00478a10();
              goto LAB_0048bdbc;
            }
            goto LAB_0048bdc0;
          }
          iVar6 = FUN_0059a530(local_268,s_CLASS_005d9a54);
          if (iVar6 == 0) {
            iVar6 = FUN_0047a410(&local_2a8,&DAT_005d9a5c,local_12c);
            if (iVar6 != 0) {
              FUN_00482fb0(0x74);
              iVar5 = FUN_004891c0(local_12c,&local_2a8);
              if (iVar5 == 0) {
                FUN_00479950(s_Error_loading_class_data_005d9a60,0);
              }
              this_00 = (cls_0x45f7c0 *)&this->mbr_0x4;
LAB_0048c0ba:
              cls_0x45f7c0::meth_0x41c840(this_00);
              FUN_00478a10();
              goto LAB_0048bdbc;
            }
            goto LAB_0048bdc0;
          }
          iVar6 = FUN_0059a530(local_268,s_WEAPON_005d9a7c);
          if (iVar6 == 0) {
            iVar6 = FUN_0047a410(&local_2a8,&DAT_005d9a84,local_8c);
            if (iVar6 != 0) {
              puVar10 = (undefined *)FUN_00482fb0(0xd0);
              if (puVar10 != (undefined *)0x0) {
                *(undefined4 *)(puVar10 + 0xcc) = 0;
                *puVar10 = 0;
                puVar10[0x20] = 0;
                *(undefined4 *)(puVar10 + 0xa0) = 0;
                *(undefined4 *)(puVar10 + 0xa4) = 0;
                *(undefined4 *)(puVar10 + 0xa8) = 0;
                *(undefined4 *)(puVar10 + 0xac) = 0;
                *(undefined4 *)(puVar10 + 0xb0) = 0;
                *(undefined4 *)(puVar10 + 0xb4) = 0;
                *(undefined4 *)(puVar10 + 0xb8) = 0xffffffff;
                *(undefined4 *)(puVar10 + 0xbc) = 0;
                *(undefined4 *)(puVar10 + 0xc4) = 0xffffffff;
                *(undefined4 *)(puVar10 + 200) = 0xffffffff;
              }
              iVar5 = 0;
              if (0 < (int)this->mbr_0x2c) {
                do {
                  iVar4 = FUN_0059a530(*(undefined4 *)(this->mbr_0x3c + iVar5 * 4),local_8c);
                  if (iVar4 == 0) {
                    FUN_00479950(s_More_than_one__s_in_WEAPON_DEF_f_005d9a88,local_8c);
                  }
                  iVar5 = iVar5 + 1;
                } while (iVar5 < (int)this->mbr_0x2c);
              }
              iVar5 = FUN_0048ac30(local_8c,&local_2a8);
              if (iVar5 == 0) {
                FUN_00479950(s_Error_loading_weapon_data_005d9aac,0);
              }
              this_00 = (cls_0x45f7c0 *)&this->mbr_0x2c;
              goto LAB_0048c0ba;
            }
            goto LAB_0048bdc0;
          }
          iVar6 = FUN_0059a530(local_268,s_ARMOR_005d9ac8);
          if (iVar6 == 0) {
            iVar6 = FUN_0047a410(&local_2a8,&DAT_005d9ad0,local_10c);
            if (iVar6 != 0) {
              puVar10 = (undefined *)FUN_00482fb0(0xcc);
              if (puVar10 != (undefined *)0x0) {
                *(undefined4 *)(puVar10 + 200) = 0;
                *puVar10 = 0;
                puVar10[0x20] = 0;
                *(undefined4 *)(puVar10 + 0xa0) = 0;
                *(undefined4 *)(puVar10 + 0xa4) = 0;
                *(undefined4 *)(puVar10 + 0xa8) = 0;
                *(undefined4 *)(puVar10 + 0xac) = 0;
                *(undefined4 *)(puVar10 + 0xb0) = 0;
                *(undefined4 *)(puVar10 + 0xb4) = 0;
                *(undefined4 *)(puVar10 + 0xb8) = 0;
                *(undefined4 *)(puVar10 + 0xbc) = 0;
                *(undefined4 *)(puVar10 + 0xc0) = 0xffffffff;
                *(undefined4 *)(puVar10 + 0xc4) = 0xffffffff;
              }
              iVar5 = 0;
              if (0 < (int)this->mbr_0x40) {
                do {
                  iVar4 = FUN_0059a530(*(undefined4 *)(this->mbr_0x50 + iVar5 * 4),local_10c);
                  if (iVar4 == 0) {
                    FUN_00479950(s_More_than_one__s_in_ARMOR_DEF_fi_005d9ad4,local_10c);
                  }
                  iVar5 = iVar5 + 1;
                } while (iVar5 < (int)this->mbr_0x40);
              }
              iVar5 = FUN_0048b1a0(local_10c,&local_2a8);
              if (iVar5 == 0) {
                FUN_00479950(s_Error_loading_armor_data_005d9af8,0);
              }
              cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x40);
              FUN_00479580();
              goto LAB_0048bdbc;
            }
            goto LAB_0048bdc0;
          }
          iVar6 = FUN_0059a530(local_268,s_HEALTHDATA_005d9b14);
          if (iVar6 == 0) {
            pdVar12 = &this->mbr_0x80;
            pdVar7 = &this->mbr_0x74;
            pdVar9 = &this->mbr_0x68;
            pcVar8 = s__i___i___i_005d9b20;
            goto LAB_0048bee5;
          }
          iVar6 = FUN_0059a530(local_268,s_FATIGUEDATA_005d9b2c);
          if (iVar6 == 0) {
            iVar6 = FUN_0047a410(&local_2a8,s__i___i___i_005d9b38,&this->mbr_0x6c,&this->mbr_0x78,
                                 &this->mbr_0x84);
            goto LAB_0048bdbc;
          }
          iVar6 = FUN_0059a530(local_268,s_MANADATA_005d9b44);
          if (iVar6 == 0) {
            iVar6 = FUN_0047a410(&local_2a8,s__i___i___i_005d9b50,&this->mbr_0x70,&this->mbr_0x7c,
                                 &this->mbr_0x88);
            goto LAB_0048bdbc;
          }
          iVar6 = FUN_0059a530(local_268,s_POISONDATA_005d9b5c);
          if (iVar6 == 0) {
            iVar6 = FUN_0047a410(&local_2a8,s__i___i_005d9b68,&this->mbr_0x8c,&this->mbr_0x90);
            goto LAB_0048bdbc;
          }
          iVar6 = FUN_0059a530(local_268,s_AMMODATA_005d9b70);
          if (iVar6 == 0) {
            iVar6 = FUN_0047a410(&local_2a8,s__i___i___i___i_005d9b7c,&this->mbr_0xdc,
                                 &this->mbr_0xe0,&this->mbr_0xe4,&this->mbr_0xe8,&this->mbr_0xec);
            goto LAB_0048bdbc;
          }
          iVar6 = FUN_0059a530(local_268,s_TOHITCENTER_005d9b8c);
          if (iVar6 == 0) {
            pdVar7 = &this->mbr_0x94;
            puVar10 = &DAT_005d9b98;
LAB_0048bdb2:
            iVar6 = FUN_0047a410(&local_2a8,puVar10,pdVar7);
            goto LAB_0048bdbc;
          }
          iVar6 = FUN_0059a530(local_268,s_TOHITRANGECHAR_005d9b9c);
          if (iVar6 == 0) {
            pdVar7 = &this->mbr_0x98;
            puVar10 = &DAT_005d9bac;
            goto LAB_0048bdb2;
          }
          iVar6 = FUN_0059a530(local_268,s_TOHITRANGEPLYR_005d9bb0);
          if (iVar6 == 0) {
            pdVar7 = &this->mbr_0x9c;
            puVar10 = &DAT_005d9bc0;
            goto LAB_0048bdb2;
          }
          iVar6 = FUN_0059a530(local_268,s_TOHITBLOCK_005d9bc4);
          if (iVar6 == 0) {
            pdVar7 = &this->mbr_0xa0;
            puVar10 = &DAT_005d9bd0;
            goto LAB_0048bdb2;
          }
          iVar6 = FUN_0059a530(local_268,s_TOHITFACE_005d9bd4);
          if (iVar6 == 0) {
            pdVar7 = &this->mbr_0xa4;
            puVar10 = &DAT_005d9be0;
            goto LAB_0048bdb2;
          }
          iVar6 = FUN_0059a530(local_268,s_TOHITDAMAGE_005d9be4);
          if (iVar6 == 0) {
            FUN_00479580();
            FUN_00479790();
            iVar5 = 0;
            pdVar7 = &this->mbr_0xac;
            do {
              iVar6 = FUN_00479700(s_ENTRY_005d9bf0,0);
              if (iVar6 == 0) {
                iVar6 = 0;
                break;
              }
              FUN_00478a10();
              FUN_00479580();
              iVar6 = FUN_0047a410(&local_2a8,s__i__i_005d9bf8,&local_238,&local_240);
              if (iVar6 != 0) {
                pdVar7[-1] = local_238;
                *pdVar7 = local_240;
              }
              FUN_00479580();
              iVar5 = iVar5 + 1;
              pdVar7 = pdVar7 + 2;
            } while (iVar5 < 5);
            FUN_00478a10();
            FUN_00479580();
            goto LAB_0048bdbc;
          }
          iVar6 = FUN_0059a530(local_268,s_STATLEVEL_005d9c00);
          if (iVar6 == 0) {
            iVar6 = FUN_0047a410(&local_2a8,&DAT_005d9c0c,local_4c);
            if (iVar6 == 0) goto LAB_0048bdc0;
            iVar5 = FUN_0049c9f0(local_4c,&local_2a8);
            if (iVar5 == 0) {
              FUN_00479950(s_Error_loading_statlevel_data_005d9c10,0);
            }
            FUN_00479580();
            goto LAB_0048bdbc;
          }
          pcVar11 = s_Invalid_block_or_tag__s_005d9c30;
          pcVar8 = local_280;
LAB_0048bdca:
          FUN_00479950(pcVar11,pcVar8);
        }
        if ((local_298 != 9) && (local_298 != 10)) {
          FUN_00479950(s_Return_expected_005d9c48,0);
        }
        iVar6 = FUN_00479450();
        if (iVar6 == 0) {
          FUN_00479950(s_Rules_file_syntax_error_005d9c58,0);
        }
      }
      local_4 = 6;
      FUN_004830f0(local_280);
      if (local_2a4 == 0) {
        if (local_2a0 != 0) {
          FUN_004830f0(local_29c);
          FUN_004a1540(local_2a0);
        }
      }
      else {
        FUN_004830f0(local_29c);
        FUN_004830f0(local_2a4);
      }
      local_4 = 0xffffffff;
      cls_0x478720::meth_0x478730(&local_270);
    }
LAB_0048c68c:
    local_2ac = local_2ac + 1;
    if (5 < local_2ac) {
      ExceptionList = local_c;
      return 1;
    }
  } while( true );
}



