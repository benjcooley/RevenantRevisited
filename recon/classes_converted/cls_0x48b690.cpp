#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x48b690



// Function at 0048b690

undefined4 cls_0x48b690::meth_0x48b690()

{
  dword *pdVar1;
  LPCVOID pvVar2;
  int iVar3;
  
  if (this->mbr_0x0 != 0) {
    return 1;
  }
  pdVar1 = &this->mbr_0x18;
  iVar3 = 0;
  if (0 < (int)this->mbr_0x18) {
    do {
      if ((-1 < iVar3) &&
         (pvVar2 = *(LPCVOID *)(this->mbr_0x28 + iVar3 * 4), pvVar2 != (LPCVOID)0x0)) {
        FUN_004894f0((int)pvVar2);
        FUN_004830f0(pvVar2);
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
      if ((-1 < iVar3) &&
         (pvVar2 = *(LPCVOID *)(this->mbr_0x14 + iVar3 * 4), pvVar2 != (LPCVOID)0x0)) {
        FUN_004830f0(pvVar2);
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

undefined4 cls_0x48b690::meth_0x48b990()

{
  LPCVOID pvVar1;
  void **ppvVar2;
  bool bVar3;
  uint uVar4;
  dword dVar5;
  undefined4 *puVar6;
  undefined3 extraout_var;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  int iVar7;
  uint extraout_ECX_03;
  cls_0x45f7c0 *this_00;
  uint extraout_ECX_04;
  uint extraout_ECX_05;
  uint extraout_ECX_06;
  uint extraout_ECX_07;
  uint extraout_ECX_08;
  int iVar8;
  dword *pdVar9;
  byte *pbVar10;
  float **ppfVar11;
  char *pcVar12;
  undefined4 *local_2b0;
  int local_2ac;
  uint local_2a8;
  LPCVOID local_2a4;
  FILE **local_2a0;
  LPCVOID local_29c;
  int local_298;
  undefined4 local_294;
  undefined4 local_290;
  undefined4 local_288;
  undefined4 local_284;
  undefined4 *local_280;
  undefined local_27c;
  undefined4 local_278;
  cls_0x478720 local_270;
  byte local_268 [39];
  undefined local_241;
  dword local_240;
  cls_0x48b690 *local_23c;
  dword local_238;
  cls_0x41c7f0 *local_234;
  cls_0x5a36f8__vftable_5a36f8 local_230 [13];
  char local_12c [32];
  char local_10c [64];
  byte local_cc [64];
  char local_8c [64];
  char local_4c [64];
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d896;
  local_c = ExceptionList;
  iVar8 = 0;
  pdVar9 = &this->mbr_0x18;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  local_23c = this;
  if (0 < (int)this->mbr_0x18) {
    do {
      if ((-1 < iVar8) &&
         (pvVar1 = *(LPCVOID *)(this->mbr_0x28 + iVar8 * 4), pvVar1 != (LPCVOID)0x0)) {
        FUN_004894f0((int)pvVar1);
        FUN_004830f0(pvVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar9);
      iVar8 = iVar8 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar8 < (int)*pdVar9);
  }
  ExceptionList = ppvVar2;
  *pdVar9 = 0;
  this->mbr_0x1c = 0;
  pdVar9 = &this->mbr_0x4;
  iVar8 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      if ((-1 < iVar8) &&
         (pvVar1 = *(LPCVOID *)(this->mbr_0x14 + iVar8 * 4), pvVar1 != (LPCVOID)0x0)) {
        FUN_004830f0(pvVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar9);
      iVar8 = iVar8 + 1;
    } while (iVar8 < (int)*pdVar9);
  }
  *pdVar9 = 0;
  this->mbr_0x8 = 0;
  pdVar9 = &this->mbr_0x2c;
  iVar8 = 0;
  if (0 < (int)this->mbr_0x2c) {
    do {
      if ((-1 < iVar8) &&
         (pvVar1 = *(LPCVOID *)(this->mbr_0x3c + iVar8 * 4), pvVar1 != (LPCVOID)0x0)) {
        FUN_0048ac00((int)pvVar1);
        FUN_004830f0(pvVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar9);
      iVar8 = iVar8 + 1;
    } while (iVar8 < (int)*pdVar9);
  }
  *pdVar9 = 0;
  this->mbr_0x30 = 0;
  pdVar9 = &this->mbr_0x40;
  iVar8 = 0;
  if (0 < (int)this->mbr_0x40) {
    do {
      if ((-1 < iVar8) &&
         (pvVar1 = *(LPCVOID *)(this->mbr_0x50 + iVar8 * 4), pvVar1 != (LPCVOID)0x0)) {
        FUN_0048b170((int)pvVar1);
        FUN_004830f0(pvVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar9);
      iVar8 = iVar8 + 1;
    } while (iVar8 < (int)*pdVar9);
  }
  *pdVar9 = 0;
  this->mbr_0x44 = 0;
  local_2ac = 0;
  do {
    if (local_2ac == 0) {
      FUN_0058b100((char *)local_230,&DAT_005d98d8);
      goto LAB_0048bca8;
    }
    if (local_2ac == 1) {
      pbVar10 = &DAT_005d98ec;
LAB_0048bb03:
      FUN_0058b100((char *)local_230,pbVar10);
LAB_0048bb19:
      iVar8 = FUN_004a1c00((char *)local_230,(char *)0x0);
      if (iVar8 != 0) goto LAB_0048bca8;
    }
    else {
      if (local_2ac == 2) {
        FUN_0058b100((char *)local_230,&DAT_005d9900);
        iVar8 = FUN_004a1c00((char *)local_230,(char *)0x0);
        if (iVar8 == 0) {
          FUN_0058b100((char *)local_230,&DAT_005d9914);
          goto LAB_0048bb19;
        }
      }
      else if (local_2ac == 3) {
        FUN_0058b100((char *)local_230,&DAT_005d9928);
        iVar8 = FUN_004a1c00((char *)local_230,(char *)0x0);
        if (iVar8 == 0) {
          FUN_0058b100((char *)local_230,&DAT_005d993c);
          goto LAB_0048bb19;
        }
      }
      else if (local_2ac == 4) {
        FUN_0058b100((char *)local_230,&DAT_005d9950);
        iVar8 = FUN_004a1c00((char *)local_230,(char *)0x0);
        if (iVar8 == 0) {
          pbVar10 = &DAT_005d9964;
          goto LAB_0048bb03;
        }
      }
      else if (local_2ac == 5) {
        FUN_0058b100((char *)local_230,&DAT_005d9978);
        iVar8 = FUN_004a1c00((char *)local_230,(char *)0x0);
        if (iVar8 == 0) {
          FUN_0058b100((char *)local_230,&DAT_005d998c);
          iVar8 = FUN_004a1c00((char *)local_230,(char *)0x0);
          if (iVar8 == 0) goto LAB_0048c68c;
        }
      }
LAB_0048bca8:
      cls_0x478720::cls_0x478720(&local_270);
      local_4 = 0;
      local_2a8 = 0;
      local_2a4 = (LPCVOID)0x0;
      local_2a0 = (FILE **)0x0;
      local_29c = (LPCVOID)0x0;
      local_298 = 0;
      local_294 = 0;
      local_290 = 0;
      local_288 = 0;
      local_284 = 0;
      local_27c = 0;
      local_278 = 1;
      local_280 = FUN_00482fb0(0x2000);
      *(undefined *)local_280 = 0;
      local_4 = 1;
      iVar8 = FUN_004789c0((int)&local_2a8,local_230);
      if (iVar8 == 0) {
        local_4 = 2;
        FUN_004830f0(local_280);
        if (local_2a4 == (LPCVOID)0x0) {
          if (local_2a0 != (FILE **)0x0) {
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
      iVar8 = FUN_00479450(&local_2a8);
      if (iVar8 == 0) {
        FUN_00479950(&local_2a8,(byte *)s_Syntax_error_in_header_005d9994);
      }
      while (local_298 != 10) {
        if (local_298 != 4) {
          FUN_00479950(&local_2a8,(byte *)s_Rules_block_name_or_tag_expected_005d99ac);
        }
        _strncpy((char *)local_268,(char *)local_280,0x27);
        local_241 = 0;
        FUN_00479580((byte *)&local_2a8);
        uVar4 = FUN_0059a530_stricmp(extraout_ECX,local_268,(byte *)s_DAYLENGTH_005d99d0);
        if (uVar4 == 0) {
          ppfVar11 = (float **)&DAT_005d99dc;
          goto LAB_0048bdb2;
        }
        uVar4 = FUN_0059a530_stricmp(extraout_ECX_00,local_268,(byte *)s_TWILIGHT_005d99e0);
        if (uVar4 == 0) {
          iVar8 = FUN_0047a410(&local_2a8,(float **)s__i___i_005d99ec);
          if (DAT_005d79e4 == 5) {
            dVar5 = FUN_0047eb50(this->mbr_0x60);
            this->mbr_0x64 = dVar5;
          }
LAB_0048bdbc:
          if (iVar8 == 0) {
LAB_0048bdc0:
            pcVar12 = s_Error_parsing_tag__s_005d8dfc;
            goto LAB_0048bdca;
          }
        }
        else {
          uVar4 = FUN_0059a530_stricmp(extraout_ECX_01,local_268,(byte *)s_STEALTH_005d99f4);
          if (uVar4 == 0) {
            pcVar12 = s__i___i___i_005d99fc;
LAB_0048bee5:
            iVar8 = FUN_0047a410(&local_2a8,(float **)pcVar12);
            goto LAB_0048bdbc;
          }
          uVar4 = FUN_0059a530_stricmp((uint)local_268,local_268,(byte *)s_CHARACTER_005d9a08);
          if (uVar4 == 0) {
            iVar8 = FUN_0047a410(&local_2a8,(float **)&DAT_005d9a14);
            if (iVar8 != 0) {
              local_2b0 = FUN_00482fb0(0x590);
              local_4._0_1_ = 3;
              if (local_2b0 == (undefined4 *)0x0) {
                local_2b0 = (undefined4 *)0x0;
              }
              else {
                local_234 = (cls_0x41c7f0 *)(local_2b0 + 0x33);
                cls_0x41c7f0::cls_0x41c7f0(local_234,0x10);
                local_4._0_1_ = 4;
                puVar6 = FUN_00482fb0(800);
                local_2b0[0x38] = puVar6;
                for (iVar7 = 200; iVar7 != 0; iVar7 = iVar7 + -1) {
                  *puVar6 = 0;
                  puVar6 = puVar6 + 1;
                }
                local_4._0_1_ = 5;
                FUN_004895d0((int)local_2b0);
                this = local_23c;
              }
              iVar7 = 0;
              local_4 = CONCAT31(local_4._1_3_,1);
              if (0 < (int)this->mbr_0x18) {
                do {
                  uVar4 = FUN_0059a530_stricmp(this->mbr_0x28,*(byte **)(this->mbr_0x28 + iVar7 * 4),
                                       local_cc);
                  if (uVar4 == 0) {
                    FUN_00479950(&local_2a8,(byte *)s_More_than_one__s_in_CHAR_DEF_fil_005d9a18);
                  }
                  iVar7 = iVar7 + 1;
                } while (iVar7 < (int)this->mbr_0x18);
              }
              iVar7 = FUN_00489850((byte *)local_2b0,local_cc,&local_2a8);
              if (iVar7 == 0) {
                FUN_00479950(&local_2a8,(byte *)s_Error_loading_char_data_005d9a3c);
              }
              cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x18);
              if (local_2b0[0x30] == -1) {
                this->mbr_0x54 = (dword)local_2b0;
              }
              FUN_00478a10((byte *)&local_2a8);
              goto LAB_0048bdbc;
            }
            goto LAB_0048bdc0;
          }
          uVar4 = FUN_0059a530_stricmp(extraout_ECX_02,local_268,(byte *)s_CLASS_005d9a54);
          if (uVar4 == 0) {
            iVar8 = FUN_0047a410(&local_2a8,(float **)&DAT_005d9a5c);
            if (iVar8 != 0) {
              puVar6 = FUN_00482fb0(0x74);
              iVar7 = FUN_004891c0((char *)puVar6,local_12c,&local_2a8);
              if (iVar7 == 0) {
                FUN_00479950(&local_2a8,(byte *)s_Error_loading_class_data_005d9a60);
              }
              this_00 = (cls_0x45f7c0 *)&this->mbr_0x4;
LAB_0048c0ba:
              cls_0x45f7c0::meth_0x41c840(this_00);
              FUN_00478a10((byte *)&local_2a8);
              goto LAB_0048bdbc;
            }
            goto LAB_0048bdc0;
          }
          uVar4 = FUN_0059a530_stricmp(extraout_ECX_03,local_268,(byte *)s_WEAPON_005d9a7c);
          if (uVar4 == 0) {
            iVar8 = FUN_0047a410(&local_2a8,(float **)&DAT_005d9a84);
            if (iVar8 != 0) {
              local_2b0 = FUN_00482fb0(0xd0);
              if (local_2b0 == (undefined4 *)0x0) {
                local_2b0 = (undefined4 *)0x0;
              }
              else {
                local_2b0[0x33] = 0;
                *(undefined *)local_2b0 = 0;
                *(undefined *)(local_2b0 + 8) = 0;
                local_2b0[0x28] = 0;
                local_2b0[0x29] = 0;
                local_2b0[0x2a] = 0;
                local_2b0[0x2b] = 0;
                local_2b0[0x2c] = 0;
                local_2b0[0x2d] = 0;
                local_2b0[0x2e] = 0xffffffff;
                local_2b0[0x2f] = 0;
                local_2b0[0x31] = 0xffffffff;
                local_2b0[0x32] = 0xffffffff;
              }
              iVar7 = 0;
              if (0 < (int)this->mbr_0x2c) {
                do {
                  uVar4 = FUN_0059a530_stricmp((uint)local_8c,*(byte **)(this->mbr_0x3c + iVar7 * 4),
                                       (byte *)local_8c);
                  if (uVar4 == 0) {
                    FUN_00479950(&local_2a8,(byte *)s_More_than_one__s_in_WEAPON_DEF_f_005d9a88);
                  }
                  iVar7 = iVar7 + 1;
                } while (iVar7 < (int)this->mbr_0x2c);
              }
              iVar7 = FUN_0048ac30((char *)local_2b0,local_8c,&local_2a8);
              if (iVar7 == 0) {
                FUN_00479950(&local_2a8,(byte *)s_Error_loading_weapon_data_005d9aac);
              }
              this_00 = (cls_0x45f7c0 *)&this->mbr_0x2c;
              goto LAB_0048c0ba;
            }
            goto LAB_0048bdc0;
          }
          uVar4 = FUN_0059a530_stricmp(extraout_ECX_04,local_268,(byte *)s_ARMOR_005d9ac8);
          if (uVar4 == 0) {
            iVar8 = FUN_0047a410(&local_2a8,(float **)&DAT_005d9ad0);
            if (iVar8 != 0) {
              local_2b0 = FUN_00482fb0(0xcc);
              if (local_2b0 == (undefined4 *)0x0) {
                local_2b0 = (undefined4 *)0x0;
              }
              else {
                local_2b0[0x32] = 0;
                *(undefined *)local_2b0 = 0;
                *(undefined *)(local_2b0 + 8) = 0;
                local_2b0[0x28] = 0;
                local_2b0[0x29] = 0;
                local_2b0[0x2a] = 0;
                local_2b0[0x2b] = 0;
                local_2b0[0x2c] = 0;
                local_2b0[0x2d] = 0;
                local_2b0[0x2e] = 0;
                local_2b0[0x2f] = 0;
                local_2b0[0x30] = 0xffffffff;
                local_2b0[0x31] = 0xffffffff;
              }
              iVar7 = 0;
              if (0 < (int)this->mbr_0x40) {
                do {
                  uVar4 = FUN_0059a530_stricmp((uint)local_10c,*(byte **)(this->mbr_0x50 + iVar7 * 4),
                                       (byte *)local_10c);
                  if (uVar4 == 0) {
                    FUN_00479950(&local_2a8,(byte *)s_More_than_one__s_in_ARMOR_DEF_fi_005d9ad4);
                  }
                  iVar7 = iVar7 + 1;
                } while (iVar7 < (int)this->mbr_0x40);
              }
              iVar7 = FUN_0048b1a0((char *)local_2b0,local_10c,&local_2a8);
              if (iVar7 == 0) {
                FUN_00479950(&local_2a8,(byte *)s_Error_loading_armor_data_005d9af8);
              }
              cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x40);
              FUN_00479580((byte *)&local_2a8);
              goto LAB_0048bdbc;
            }
            goto LAB_0048bdc0;
          }
          uVar4 = FUN_0059a530_stricmp(extraout_ECX_05,local_268,(byte *)s_HEALTHDATA_005d9b14);
          if (uVar4 == 0) {
            pcVar12 = s__i___i___i_005d9b20;
            goto LAB_0048bee5;
          }
          uVar4 = FUN_0059a530_stricmp((uint)local_268,local_268,(byte *)s_FATIGUEDATA_005d9b2c);
          if (uVar4 == 0) {
            iVar8 = FUN_0047a410(&local_2a8,(float **)s__i___i___i_005d9b38);
            goto LAB_0048bdbc;
          }
          uVar4 = FUN_0059a530_stricmp(extraout_ECX_06,local_268,(byte *)s_MANADATA_005d9b44);
          if (uVar4 == 0) {
            iVar8 = FUN_0047a410(&local_2a8,(float **)s__i___i___i_005d9b50);
            goto LAB_0048bdbc;
          }
          uVar4 = FUN_0059a530_stricmp(extraout_ECX_07,local_268,(byte *)s_POISONDATA_005d9b5c);
          if (uVar4 == 0) {
            iVar8 = FUN_0047a410(&local_2a8,(float **)s__i___i_005d9b68);
            goto LAB_0048bdbc;
          }
          uVar4 = FUN_0059a530_stricmp(extraout_ECX_08,local_268,(byte *)s_AMMODATA_005d9b70);
          if (uVar4 == 0) {
            iVar8 = FUN_0047a410(&local_2a8,(float **)s__i___i___i___i_005d9b7c);
            goto LAB_0048bdbc;
          }
          uVar4 = FUN_0059a530_stricmp((uint)local_268,local_268,(byte *)s_TOHITCENTER_005d9b8c);
          if (uVar4 == 0) {
            ppfVar11 = (float **)&DAT_005d9b98;
LAB_0048bdb2:
            iVar8 = FUN_0047a410(&local_2a8,ppfVar11);
            goto LAB_0048bdbc;
          }
          uVar4 = FUN_0059a530_stricmp((uint)local_268,local_268,(byte *)s_TOHITRANGECHAR_005d9b9c);
          if (uVar4 == 0) {
            ppfVar11 = (float **)&DAT_005d9bac;
            goto LAB_0048bdb2;
          }
          uVar4 = FUN_0059a530_stricmp((uint)local_268,local_268,(byte *)s_TOHITRANGEPLYR_005d9bb0);
          if (uVar4 == 0) {
            ppfVar11 = (float **)&DAT_005d9bc0;
            goto LAB_0048bdb2;
          }
          uVar4 = FUN_0059a530_stricmp((uint)local_268,local_268,(byte *)s_TOHITBLOCK_005d9bc4);
          if (uVar4 == 0) {
            ppfVar11 = (float **)&DAT_005d9bd0;
            goto LAB_0048bdb2;
          }
          uVar4 = FUN_0059a530_stricmp((uint)local_268,local_268,(byte *)s_TOHITFACE_005d9bd4);
          if (uVar4 == 0) {
            ppfVar11 = (float **)&DAT_005d9be0;
            goto LAB_0048bdb2;
          }
          uVar4 = FUN_0059a530_stricmp((uint)local_268,local_268,(byte *)s_TOHITDAMAGE_005d9be4);
          if (uVar4 == 0) {
            FUN_00479580((byte *)&local_2a8);
            FUN_00479790((byte *)&local_2a8);
            iVar7 = 0;
            pdVar9 = &this->mbr_0xac;
            do {
              bVar3 = FUN_00479700((uint)&local_2a8,(byte *)s_ENTRY_005d9bf0,0);
              if (CONCAT31(extraout_var,bVar3) == 0) {
                iVar8 = 0;
                break;
              }
              FUN_00478a10((byte *)&local_2a8);
              FUN_00479580((byte *)&local_2a8);
              iVar8 = FUN_0047a410(&local_2a8,(float **)s__i__i_005d9bf8);
              if (iVar8 != 0) {
                pdVar9[-1] = local_238;
                *pdVar9 = local_240;
              }
              FUN_00479580((byte *)&local_2a8);
              iVar7 = iVar7 + 1;
              pdVar9 = pdVar9 + 2;
            } while (iVar7 < 5);
            FUN_00478a10((byte *)&local_2a8);
            FUN_00479580((byte *)&local_2a8);
            goto LAB_0048bdbc;
          }
          uVar4 = FUN_0059a530_stricmp((uint)local_268,local_268,(byte *)s_STATLEVEL_005d9c00);
          if (uVar4 == 0) {
            iVar8 = FUN_0047a410(&local_2a8,(float **)&DAT_005d9c0c);
            if (iVar8 == 0) goto LAB_0048bdc0;
            iVar7 = FUN_0049c9f0((int *)&this->mbr_0x58,local_4c,&local_2a8);
            if (iVar7 == 0) {
              FUN_00479950(&local_2a8,(byte *)s_Error_loading_statlevel_data_005d9c10);
            }
            FUN_00479580((byte *)&local_2a8);
            goto LAB_0048bdbc;
          }
          pcVar12 = s_Invalid_block_or_tag__s_005d9c30;
LAB_0048bdca:
          FUN_00479950(&local_2a8,(byte *)pcVar12);
        }
        if ((local_298 != 9) && (local_298 != 10)) {
          FUN_00479950(&local_2a8,(byte *)s_Return_expected_005d9c48);
        }
        iVar8 = FUN_00479450(&local_2a8);
        if (iVar8 == 0) {
          FUN_00479950(&local_2a8,(byte *)s_Rules_file_syntax_error_005d9c58);
        }
      }
      local_4 = 6;
      FUN_004830f0(local_280);
      if (local_2a4 == (LPCVOID)0x0) {
        if (local_2a0 != (FILE **)0x0) {
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



