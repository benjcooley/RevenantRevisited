#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x495cf0



// Function at 00495cf0

undefined4 cls_0x495cf0::meth_0x495cf0()

{
  char cVar1;
  bool bVar2;
  undefined4 *puVar3;
  undefined3 extraout_var;
  uint uVar4;
  uint uVar5;
  dword *pdVar6;
  int iVar7;
  undefined4 *puVar8;
  undefined4 local_150;
  LPCVOID local_14c;
  FILE **local_148;
  LPCVOID local_144;
  int local_140;
  dword local_13c;
  undefined4 local_138;
  undefined4 local_130;
  undefined4 local_12c;
  undefined4 *local_128;
  undefined local_124;
  undefined4 local_120;
  cls_0x478720 local_118;
  cls_0x5a36f8__vftable_5a36f8 local_110 [13];
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059da42;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  if (0 < (int)this->mbr_0x0) {
    iVar7 = 0;
    ExceptionList = &pvStack_c;
    if (0 < (int)this->mbr_0x0) {
      pdVar6 = &this->mbr_0x4004;
      ExceptionList = &pvStack_c;
      do {
        if ((LPCVOID)*pdVar6 != (LPCVOID)0x0) {
          FUN_004830f0((LPCVOID)*pdVar6);
        }
        *pdVar6 = 0;
        iVar7 = iVar7 + 1;
        pdVar6 = pdVar6 + 1;
      } while (iVar7 < (int)this->mbr_0x0);
    }
    this->mbr_0x0 = 0;
  }
  FUN_0058b100((char *)local_110,(byte *)s__s_s__s_005da474);
  iVar7 = FUN_004a1c00((char *)local_110,(char *)0x0);
  if (iVar7 == 0) {
    FUN_0058b100((char *)local_110,&DAT_005da47c);
  }
  cls_0x478720::cls_0x478720(&local_118);
  local_4 = 0;
  local_150 = 0;
  local_14c = (LPCVOID)0x0;
  local_148 = (FILE **)0x0;
  local_144 = (LPCVOID)0x0;
  local_140 = 0;
  local_13c = 0;
  local_138 = 0;
  local_130 = 0;
  local_12c = 0;
  local_124 = 0;
  local_120 = 1;
  local_128 = FUN_00482fb0(0x2000);
  *(undefined *)local_128 = 0;
  local_4 = 1;
  FUN_004789c0((int)&local_150,local_110);
  FUN_004795a0((byte *)&local_150);
  this->mbr_0x0 = 0;
  do {
    while( true ) {
      if (local_140 == 10) {
        local_4 = 2;
        FUN_004830f0(local_128);
        if (local_14c == (LPCVOID)0x0) {
          if (local_148 != (FILE **)0x0) {
            FUN_004830f0(local_144);
            FUN_004a1540(local_148);
          }
        }
        else {
          FUN_004830f0(local_144);
          FUN_004830f0(local_14c);
        }
        local_4 = 0xffffffff;
        cls_0x478720::meth_0x478730(&local_118);
        ExceptionList = pvStack_c;
        return 1;
      }
      FUN_00479680((byte *)&local_150);
      if (local_140 == 4) break;
      FUN_00481c10((byte *)s_Expected_gamestate_identifier_in_005da4b0);
    }
    uVar4 = 0xffffffff;
    puVar3 = local_128;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar1 = *(char *)puVar3;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    } while (cVar1 != '\0');
    puVar3 = FUN_00482fb0(~uVar4);
    (&this->mbr_0x4004)[this->mbr_0x0] = (dword)puVar3;
    uVar4 = 0xffffffff;
    puVar3 = local_128;
    do {
      puVar8 = puVar3;
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      puVar8 = (undefined4 *)((int)puVar3 + 1);
      cVar1 = *(char *)puVar3;
      puVar3 = puVar8;
    } while (cVar1 != '\0');
    uVar4 = ~uVar4;
    puVar3 = (undefined4 *)((int)puVar8 - uVar4);
    puVar8 = (undefined4 *)(&this->mbr_0x4004)[this->mbr_0x0];
    for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
      *puVar8 = *puVar3;
      puVar3 = puVar3 + 1;
      puVar8 = puVar8 + 1;
    }
    for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined *)puVar8 = *(undefined *)puVar3;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
      puVar8 = (undefined4 *)((int)puVar8 + 1);
    }
    FUN_00479580((byte *)&local_150);
    bVar2 = FUN_00479700((uint)&local_150,&DAT_005da484,0);
    if (CONCAT31(extraout_var,bVar2) != 0) {
      FUN_00479580((byte *)&local_150);
    }
    if (local_140 != 8) {
      FUN_00481c10((byte *)s_Invalid_init_value_for_game_stat_005da488);
    }
    (&this->mbr_0x4)[this->mbr_0x0] = local_13c;
    this->mbr_0x0 = this->mbr_0x0 + 1;
    FUN_004795a0((byte *)&local_150);
  } while( true );
}



// Function at 00496240

undefined4 cls_0x495cf0::meth_0x496240()

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  
  if ((undefined4 *)this->mbr_0x8014 != (undefined4 *)0x0) {
    puVar3 = (undefined4 *)this->mbr_0x8014;
    for (uVar1 = this->mbr_0x800c & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined *)puVar3 = 0;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
  }
  this->mbr_0x8008 = 0;
  this->mbr_0x8004 = 0;
  if ((undefined4 *)this->mbr_0x8028 != (undefined4 *)0x0) {
    puVar3 = (undefined4 *)this->mbr_0x8028;
    for (uVar1 = this->mbr_0x8020 & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined *)puVar3 = 0;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
  }
  this->mbr_0x801c = 0;
  this->mbr_0x8018 = 0;
  if ((undefined4 *)this->mbr_0x803c != (undefined4 *)0x0) {
    puVar3 = (undefined4 *)this->mbr_0x803c;
    for (uVar1 = this->mbr_0x8034 & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined *)puVar3 = 0;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
  }
  this->mbr_0x8030 = 0;
  this->mbr_0x802c = 0;
  iVar2 = meth_0x496490(this);
  if (iVar2 == 0) {
    FUN_00481c10((byte *)s_Unable_to_load_MASTER_S_file_005da4e8);
  }
  if (DAT_0066829c == 0) {
    iVar2 = meth_0x495cf0(this);
    if (iVar2 == 0) {
      FUN_00481c10((byte *)s_Unable_to_load_STATE_DEF_file_005da514);
    }
  }
  return 1;
}



// Function at 00496490

undefined4 cls_0x495cf0::meth_0x496490()

{
  byte bVar1;
  FILE **ppFVar2;
  DWORD DVar3;
  undefined4 *puVar4;
  uint uVar5;
  undefined3 extraout_var;
  int iVar6;
  char *in_stack_00000008;
  undefined4 in_stack_0000000c;
  undefined4 local_208 [65];
  char local_104 [260];
  
  if (DAT_0065a784 < 0) {
    iVar6 = 0;
  }
  else {
    iVar6 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
  }
  FUN_0058b100(local_104,&DAT_005da54c);
  if (iVar6 != 0) {
    FUN_0058b100((char *)local_208,(byte *)s__s_s__s_005da554);
  }
  ppFVar2 = (FILE **)FUN_004a13f0((char *)local_208,(uint *)&DAT_005da55c,0);
  if ((ppFVar2 == (FILE **)0x0) &&
     (ppFVar2 = (FILE **)FUN_004a13f0(local_104,(uint *)&DAT_005da560,0), ppFVar2 == (FILE **)0x0))
  {
    FUN_00481c10((byte *)s_Unable_to_find_game_script_file___005da564);
  }
  local_208[0] = 1;
  DVar3 = FUN_004a17b0((int *)ppFVar2);
  puVar4 = FUN_00482fb0(DVar3 + 1);
  uVar5 = FUN_004a15a0(puVar4,1,DVar3,ppFVar2);
  if ((int)uVar5 < (int)DVar3) {
    local_208[0] = 0;
  }
  else {
    *(undefined *)((int)puVar4 + DVar3) = 0;
    bVar1 = FUN_004834e0((byte *)puVar4,DVar3);
    if (CONCAT31(extraout_var,bVar1) != 0) {
      FUN_00483540((byte *)puVar4,DVar3);
    }
    meth_0x496860(this,puVar4,in_stack_00000008);
  }
  FUN_004830f0(puVar4);
  FUN_004a1540(ppFVar2);
  iVar6 = 0;
  if (0 < (int)this->mbr_0x802c) {
    do {
      uVar5 = FUN_0059a530((uint)in_stack_00000008,*(byte **)(this->mbr_0x803c + iVar6 * 4),
                           (byte *)in_stack_00000008);
      if (uVar5 == 0) {
        if ((-1 < iVar6) && (iVar6 = *(int *)(this->mbr_0x803c + iVar6 * 4), iVar6 != 0)) {
          *(undefined4 *)(iVar6 + 0x80) = in_stack_0000000c;
          goto LAB_00496638;
        }
        break;
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)this->mbr_0x802c);
  }
  puVar4 = FUN_00482fb0(0x84);
  _strncpy((char *)puVar4,in_stack_00000008,0x7f);
  *(undefined *)((int)puVar4 + 0x7f) = 0;
  puVar4[0x20] = in_stack_0000000c;
  cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x802c);
LAB_00496638:
  if (DAT_00666918 != 0) {
    FUN_0045a680(8,0);
  }
  this->mbr_0x8040 = 0;
  return local_208[0];
}



// Function at 004967a0

void cls_0x495cf0::meth_0x4967a0()

{
  LPCVOID *ppvVar1;
  dword dVar2;
  LPCVOID pvVar3;
  uint uVar4;
  LPCVOID in_stack_00000004;
  
  uVar4 = 0;
  if (0 < (int)this->mbr_0x8004) {
    do {
      if ((((this->mbr_0x8014 != 0) && (uVar4 < this->mbr_0x8004)) &&
          (*(int *)(this->mbr_0x8014 + uVar4 * 4) != 0)) &&
         ((ppvVar1 = *(LPCVOID **)(this->mbr_0x8014 + uVar4 * 4), ppvVar1[0xe] == in_stack_00000004
          && (ppvVar1 != (LPCVOID *)0x0)))) {
        meth_0x496f20(this,uVar4);
        FUN_004948e0(ppvVar1);
        FUN_004830f0(ppvVar1);
      }
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < (int)this->mbr_0x8004);
  }
  dVar2 = this->mbr_0x802c;
  while (dVar2 = dVar2 - 1, -1 < (int)dVar2) {
    if (*(LPCVOID *)(*(int *)(this->mbr_0x803c + dVar2 * 4) + 0x80) == in_stack_00000004) {
      pvVar3 = *(LPCVOID *)(this->mbr_0x803c + dVar2 * 4);
      if (pvVar3 != (LPCVOID)0x0) {
        FUN_004830f0(pvVar3);
      }
      cls_0x45f7c0::meth_0x41cb80((cls_0x45f7c0 *)&this->mbr_0x802c);
    }
  }
  return;
}



// Function at 00496860

void cls_0x495cf0::meth_0x496860(byte **param_1, char *param_2)

{
  uint *puVar1;
  dword *this_00;
  char cVar2;
  byte bVar3;
  cls_0x4922c0 *this_01;
  LPCVOID *ppvVar4;
  dword dVar5;
  TScreen *this_02;
  undefined4 uVar6;
  int iVar7;
  char **ppcVar8;
  char *pcVar9;
  byte *pbVar10;
  uint uVar11;
  int iVar12;
  byte *pbVar13;
  bool bVar14;
  char *in_stack_0000000c;
  cls_0x5a36f8__vftable_5a36f8 *local_60;
  char *local_5c;
  char *local_58;
  char *local_54;
  char *local_50;
  uint local_4c;
  LPCVOID local_48;
  FILE **local_44;
  cls_0x5a36f8__vftable_5a36f8 **local_40;
  int local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 *local_24;
  undefined local_20;
  undefined4 local_1c;
  cls_0x478720 local_14;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059da77;
  local_c = ExceptionList;
  uVar11 = 0xffffffff;
  pcVar9 = (char *)param_1;
  do {
    if (uVar11 == 0) break;
    uVar11 = uVar11 - 1;
    cVar2 = *pcVar9;
    pcVar9 = pcVar9 + 1;
  } while (cVar2 != '\0');
  local_60 = &cls_0x5a36f8__vftable_5a36f8_005a36f8;
  local_54 = (char *)((int)param_1 + (~uVar11 - 1));
  local_58 = (char *)param_1;
  local_50 = (char *)param_1;
  local_5c = s_String_005da638;
  ExceptionList = &local_c;
  cls_0x478720::cls_0x478720(&local_14);
  local_40 = &local_60;
  local_4 = 0;
  local_4c = 0;
  local_48 = (LPCVOID)0x0;
  local_44 = (FILE **)0x0;
  local_3c = 0;
  local_38 = 0;
  local_34 = 0;
  local_2c = 0;
  local_28 = 0;
  local_20 = 0;
  local_1c = 1;
  local_24 = FUN_00482fb0(0x2000);
  *(undefined *)local_24 = 0;
  uVar6 = DAT_0066856c;
  local_4 = 1;
  DAT_0066856c = 1;
  FUN_00478a10((byte *)&local_4c);
  iVar12 = 0;
  if (0 < (int)this->mbr_0x8004) {
    do {
      iVar7 = cls_0x45f7c0::meth_0x410160((cls_0x45f7c0 *)&this->mbr_0x8004);
      if ((iVar7 != 0) &&
         (iVar7 = cls_0x495c20::meth_0x495c20(*(cls_0x495c20 **)(this->mbr_0x8014 + iVar12 * 4)),
         iVar7 != 0)) {
        puVar1 = (uint *)(*(int *)(this->mbr_0x8014 + iVar12 * 4) + 0x48);
        *puVar1 = *puVar1 | 1;
      }
      iVar12 = iVar12 + 1;
    } while (iVar12 < (int)this->mbr_0x8004);
  }
  uVar11 = 0;
  if (0 < (int)this->mbr_0x8018) {
    do {
      if (((this->mbr_0x8028 != 0) && (uVar11 < this->mbr_0x8018)) &&
         (*(int *)(this->mbr_0x8028 + uVar11 * 4) != 0)) {
        this_01 = *(cls_0x4922c0 **)(this->mbr_0x8028 + uVar11 * 4);
        iVar12 = cls_0x4922c0::meth_0x4943f0(this_01);
        if (iVar12 != 0) {
          cls_0x4922c0::meth_0x492490(this_01);
        }
      }
      uVar11 = uVar11 + 1;
    } while ((int)uVar11 < (int)this->mbr_0x8018);
  }
joined_r0x004969cb:
  if (local_3c == 10) {
    uVar11 = 0;
    if (0 < (int)this->mbr_0x8018) {
      do {
        if (((this->mbr_0x8028 != 0) && (uVar11 < this->mbr_0x8018)) &&
           (*(int *)(this->mbr_0x8028 + uVar11 * 4) != 0)) {
          iVar12 = *(int *)(this->mbr_0x8028 + uVar11 * 4);
          for (iVar7 = *(int *)(iVar12 + 4); iVar7 != 0; iVar7 = *(int *)(iVar7 + 8)) {
            if ((*(byte *)(iVar7 + 0x48) & 2) != 0) {
              this_02 = *(TScreen **)(iVar12 + 0xc);
              if (this_02 != (TScreen *)0x0) {
                this_02->mbr_0x84 = 0;
                TScreen::meth_0x471150(this_02);
              }
              break;
            }
          }
        }
        uVar11 = uVar11 + 1;
      } while ((int)uVar11 < (int)this->mbr_0x8018);
    }
    uVar11 = 0;
    if (0 < (int)this->mbr_0x8004) {
      do {
        if (((this->mbr_0x8014 != 0) && (uVar11 < this->mbr_0x8004)) &&
           ((*(int *)(this->mbr_0x8014 + uVar11 * 4) != 0 &&
            ((*(byte *)(*(int *)(this->mbr_0x8014 + uVar11 * 4) + 0x48) & 1) != 0)))) {
          meth_0x496f20(this,uVar11);
        }
        uVar11 = uVar11 + 1;
      } while ((int)uVar11 < (int)this->mbr_0x8004);
    }
    local_4 = 4;
    DAT_0066856c = uVar6;
    FUN_004830f0(local_24);
    if (local_48 == (LPCVOID)0x0) {
joined_r0x00496ddb:
      if (local_44 != (FILE **)0x0) {
        FUN_004830f0(local_40);
        FUN_004a1540(local_44);
      }
    }
    else {
      FUN_004830f0(local_40);
      FUN_004830f0(local_48);
    }
  }
  else {
    ppcVar8 = (char **)FUN_00482fb0(0x4c);
    local_4._0_1_ = 2;
    if (ppcVar8 == (char **)0x0) {
      param_1 = (byte **)0x0;
    }
    else {
      param_1 = (byte **)FUN_004946f0(ppcVar8,(char *)0x0,in_stack_0000000c,param_2,(char *)0x0);
    }
    local_4 = CONCAT31(local_4._1_3_,1);
    pcVar9 = FUN_00494e20((char **)param_1,&local_4c);
    if (pcVar9 != (char *)0xffffffff) {
      uVar11 = 0;
      if (0 < (int)this->mbr_0x8004) {
        do {
          if ((((this->mbr_0x8014 != 0) && (uVar11 < this->mbr_0x8004)) &&
              (*(int *)(this->mbr_0x8014 + uVar11 * 4) != 0)) &&
             ((pbVar10 = **(byte ***)(this->mbr_0x8014 + uVar11 * 4), pbVar10 != (byte *)0x0 &&
              (pbVar13 = *param_1, pbVar13 != (byte *)0x0)))) {
            do {
              bVar3 = *pbVar10;
              bVar14 = bVar3 < *pbVar13;
              if (bVar3 != *pbVar13) {
LAB_00496a9c:
                iVar12 = (1 - (uint)bVar14) - (uint)(bVar14 != 0);
                goto LAB_00496aa1;
              }
              if (bVar3 == 0) break;
              bVar3 = pbVar10[1];
              bVar14 = bVar3 < pbVar13[1];
              if (bVar3 != pbVar13[1]) goto LAB_00496a9c;
              pbVar10 = pbVar10 + 2;
              pbVar13 = pbVar13 + 2;
            } while (bVar3 != 0);
            iVar12 = 0;
LAB_00496aa1:
            if (iVar12 == 0) {
              ppvVar4 = *(LPCVOID **)(this->mbr_0x8014 + uVar11 * 4);
              if (*ppvVar4 == (LPCVOID)0x0) goto LAB_00496baf;
              if (param_1 == (byte **)0x0) goto LAB_00496bc3;
              pbVar10 = param_1[1];
              pbVar13 = (byte *)ppvVar4[1];
              goto LAB_00496add;
            }
          }
          uVar11 = uVar11 + 1;
        } while ((int)uVar11 < (int)this->mbr_0x8004);
      }
      goto LAB_00496be3;
    }
    if (param_1 != (byte **)0x0) {
      FUN_004948e0(param_1);
      FUN_004830f0(param_1);
    }
    local_4 = 3;
    FUN_004830f0(local_24);
    if (local_48 == (LPCVOID)0x0) goto joined_r0x00496ddb;
    FUN_004830f0(local_40);
    FUN_004830f0(local_48);
  }
  local_4 = 0xffffffff;
  cls_0x478720::meth_0x478730(&local_14);
  ExceptionList = local_c;
  return;
  while( true ) {
    bVar3 = pbVar13[1];
    bVar14 = bVar3 < pbVar10[1];
    if (bVar3 != pbVar10[1]) goto LAB_00496b01;
    pbVar13 = pbVar13 + 2;
    pbVar10 = pbVar10 + 2;
    if (bVar3 == 0) break;
LAB_00496add:
    bVar3 = *pbVar13;
    bVar14 = bVar3 < *pbVar10;
    if (bVar3 != *pbVar10) {
LAB_00496b01:
      iVar12 = (1 - (uint)bVar14) - (uint)(bVar14 != 0);
      goto LAB_00496b06;
    }
    if (bVar3 == 0) break;
  }
  iVar12 = 0;
LAB_00496b06:
  if (iVar12 == 0) {
LAB_00496baf:
    if (param_1 != (byte **)0x0) {
      FUN_004948e0(param_1);
      FUN_004830f0(param_1);
    }
LAB_00496bc3:
    param_1 = (byte **)0x0;
    puVar1 = (uint *)(*(int *)(this->mbr_0x8014 + uVar11 * 4) + 0x48);
    *puVar1 = *puVar1 & 0xfffffffe;
  }
  else {
    meth_0x496f20(this,uVar11);
    cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)&this->mbr_0x8004);
    if (ppvVar4 != (LPCVOID *)0x0) {
      FUN_004948e0(ppvVar4);
      FUN_004830f0(ppvVar4);
    }
    cls_0x45f7c0::meth_0x41c910((cls_0x45f7c0 *)&this->mbr_0x8004,param_1);
    puVar1 = (uint *)(*(int *)(this->mbr_0x8014 + uVar11 * 4) + 0x48);
    *puVar1 = *puVar1 | 2;
    FUN_0045a680(8,param_1);
    if (DAT_00668154 == 0) {
      FUN_0054d170((TCharacter *)&DAT_0065c5d0,s_Updated___s__script_005da658);
    }
    else {
      FUN_0041ee50((byte *)s_Updated___s__script_005da640);
    }
  }
LAB_00496be3:
  dVar5 = this->mbr_0x8004;
  this_00 = &this->mbr_0x8004;
  if ((int)dVar5 <= (int)uVar11) {
    uVar11 = 0;
    if (0 < (int)dVar5) {
      do {
        if (((this->mbr_0x8014 == 0) || (*this_00 <= uVar11)) ||
           (*(int *)(this->mbr_0x8014 + uVar11 * 4) == 0)) {
          FUN_0045a680(5,*(undefined4 *)(this->mbr_0x8014 + uVar11 * 4));
          cls_0x45f7c0::meth_0x41c910((cls_0x45f7c0 *)this_00,param_1);
          break;
        }
        uVar11 = uVar11 + 1;
      } while ((int)uVar11 < (int)dVar5);
    }
    if ((int)*this_00 <= (int)uVar11) {
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)this_00);
    }
    FUN_0045a680(8,param_1);
  }
  goto joined_r0x004969cb;
}



// Function at 00496f20

void cls_0x495cf0::meth_0x496f20(int param_1)

{
  dword *this_00;
  dword dVar1;
  cls_0x4922c0 *this_01;
  int iVar2;
  dword dVar3;
  dword dVar4;
  uint uVar5;
  dword in_stack_00000008;
  
  dVar1 = *(dword *)(this->mbr_0x8014 + param_1 * 4);
  if (dVar1 != 0) {
    FUN_0045a680(5,dVar1);
    uVar5 = 0;
    if (0 < (int)this->mbr_0x8018) {
      do {
        if (((this->mbr_0x8028 != 0) && (uVar5 < this->mbr_0x8018)) &&
           (*(int *)(this->mbr_0x8028 + uVar5 * 4) != 0)) {
          this_01 = *(cls_0x4922c0 **)(this->mbr_0x8028 + uVar5 * 4);
          if (this_01->mbr_0x8 == dVar1) {
            this_01->mbr_0x8 = in_stack_00000008;
          }
          if (this_01->mbr_0x4 == dVar1) {
            this_01->mbr_0x4 = *(dword *)(this_01->mbr_0x4 + 8);
          }
          if ((this_01->mbr_0x4 == 0) && (this_01 != (cls_0x4922c0 *)0x0)) {
            cls_0x4922c0::meth_0x4922c0(this_01);
            FUN_004830f0(this_01);
          }
        }
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)this->mbr_0x8018);
    }
    this_00 = &this->mbr_0x8004;
    uVar5 = 0;
    if (0 < (int)this->mbr_0x8004) {
      do {
        if (((this->mbr_0x8014 != 0) && (uVar5 < *this_00)) &&
           (*(int *)(this->mbr_0x8014 + uVar5 * 4) != 0)) {
          iVar2 = *(int *)(this->mbr_0x8014 + uVar5 * 4);
          dVar3 = *(dword *)(iVar2 + 8);
          if (dVar3 == dVar1) {
            dVar4 = in_stack_00000008;
            if (in_stack_00000008 == 0) {
              dVar4 = *(dword *)(dVar3 + 8);
            }
            *(dword *)(iVar2 + 8) = dVar4;
          }
        }
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)*this_00);
    }
    cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
  }
  return;
}



// Function at 004970b0

void cls_0x495cf0::meth_0x4970b0()

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)this->mbr_0x802c) {
    do {
      iVar1 = meth_0x496490(this);
      if (iVar1 != 0) {
        if (DAT_00668154 == 0) {
          FUN_0054d170((TCharacter *)&DAT_0065c5d0,s_Unable_to_reload_script_file___s_005da690);
        }
        else {
          FUN_0041ee50((byte *)s_Unable_to_reload_script_file___s_005da66c);
        }
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x802c);
  }
  return;
}



