// Decompiled methods and structure for class: cls_0x495cf0

/*
/OOAnalyzer/cls_0x495cf0
pack(disabled)
Structure cls_0x495cf0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   16388   dword   4   mbr_0x4004   "Unsigned Double-Word (ddw, 4-bytes)"
   32772   dword   4   mbr_0x8004   "Unsigned Double-Word (ddw, 4-bytes)"
   32776   dword   4   mbr_0x8008   "Unsigned Double-Word (ddw, 4-bytes)"
   32780   dword   4   mbr_0x800c   "Unsigned Double-Word (ddw, 4-bytes)"
   32788   dword   4   mbr_0x8014   "Unsigned Double-Word (ddw, 4-bytes)"
   32792   dword   4   mbr_0x8018   "Unsigned Double-Word (ddw, 4-bytes)"
   32796   dword   4   mbr_0x801c   "Unsigned Double-Word (ddw, 4-bytes)"
   32800   dword   4   mbr_0x8020   "Unsigned Double-Word (ddw, 4-bytes)"
   32808   dword   4   mbr_0x8028   "Unsigned Double-Word (ddw, 4-bytes)"
   32812   dword   4   mbr_0x802c   "Unsigned Double-Word (ddw, 4-bytes)"
   32816   dword   4   mbr_0x8030   "Unsigned Double-Word (ddw, 4-bytes)"
   32820   dword   4   mbr_0x8034   "Unsigned Double-Word (ddw, 4-bytes)"
   32828   dword   4   mbr_0x803c   "Unsigned Double-Word (ddw, 4-bytes)"
   32832   dword   4   mbr_0x8040   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 32836 Alignment: 1

*/

// Function at 00495cf0

/* WARNING: Removing unreachable block (ram,0x00495f0a) */
/* WARNING: Removing unreachable block (ram,0x00495f40) */
/* WARNING: Removing unreachable block (ram,0x00495f46) */
/* WARNING: Removing unreachable block (ram,0x00495e43) */
/* WARNING: Removing unreachable block (ram,0x00495e4c) */
/* WARNING: Removing unreachable block (ram,0x00495e4e) */
/* WARNING: Removing unreachable block (ram,0x00495e6f) */
/* WARNING: Removing unreachable block (ram,0x00495e71) */
/* WARNING: Removing unreachable block (ram,0x00495e83) */
/* WARNING: Removing unreachable block (ram,0x00495e85) */
/* WARNING: Removing unreachable block (ram,0x00495e8a) */
/* WARNING: Removing unreachable block (ram,0x00495e8c) */
/* WARNING: Removing unreachable block (ram,0x00495ea8) */
/* WARNING: Removing unreachable block (ram,0x00495eb1) */
/* WARNING: Removing unreachable block (ram,0x00495eb8) */
/* WARNING: Removing unreachable block (ram,0x00495ecd) */
/* WARNING: Removing unreachable block (ram,0x00495f2a) */
/* WARNING: Removing unreachable block (ram,0x00495f5a) */
/* WARNING: Removing unreachable block (ram,0x00495f5d) */

void __thiscall OOAnalyzer::cls_0x495cf0::meth_0x495cf0(cls_0x495cf0 *this)

{
  undefined *puVar1;
  dword *pdVar2;
  int iVar3;
  undefined4 in_stack_00000004;
  cls_0x478720 local_118;
  undefined local_110 [260];
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059da42;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  if (0 < (int)this->mbr_0x0) {
    iVar3 = 0;
    ExceptionList = &pvStack_c;
    if (0 < (int)this->mbr_0x0) {
      pdVar2 = &this->mbr_0x4004;
      ExceptionList = &pvStack_c;
      do {
        if (*pdVar2 != 0) {
          FUN_004830f0(*pdVar2);
        }
        *pdVar2 = 0;
        iVar3 = iVar3 + 1;
        pdVar2 = pdVar2 + 1;
      } while (iVar3 < (int)this->mbr_0x0);
    }
    this->mbr_0x0 = 0;
  }
  if (DAT_0065a784 < 0) {
    iVar3 = 0;
  }
  else {
    iVar3 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
  }
  FUN_0058b100(local_110,s__s_s__s_005da474,&DAT_0065d6a4,iVar3 + 0x58,in_stack_00000004);
  iVar3 = FUN_004a1c00(local_110,0);
  if (iVar3 == 0) {
    FUN_0058b100(local_110,&DAT_005da47c,0x65bd48,in_stack_00000004);
  }
  cls_0x478720::cls_0x478720(&local_118);
  local_4 = 0;
  puVar1 = (undefined *)FUN_00482fb0(0x2000);
  *puVar1 = 0;
  local_4 = 1;
  FUN_004789c0(local_110);
  FUN_004795a0();
  this->mbr_0x0 = 0;
  do {
    FUN_00479680();
    FUN_00481c10(s_Expected_gamestate_identifier_in_005da4b0,in_stack_00000004);
  } while( true );
}



// Function at 00496240

undefined4 __thiscall OOAnalyzer::cls_0x495cf0::meth_0x496240(cls_0x495cf0 *this)

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
    FUN_00481c10(s_Unable_to_load_MASTER_S_file_005da4e8,0);
  }
  if (DAT_0066829c == 0) {
    iVar2 = meth_0x495cf0(this);
    if (iVar2 == 0) {
      FUN_00481c10(s_Unable_to_load_STATE_DEF_file_005da514,0);
    }
  }
  return 1;
}



// Function at 00496490

undefined4 __thiscall OOAnalyzer::cls_0x495cf0::meth_0x496490(cls_0x495cf0 *this)

{
  int iVar1;
  int iVar2;
  int iVar3;
  char *_Dest;
  int iVar4;
  char *in_stack_00000004;
  undefined4 in_stack_00000008;
  undefined4 local_20c;
  undefined local_208 [260];
  undefined local_104 [260];
  
  if (DAT_0065a784 < 0) {
    iVar4 = 0;
  }
  else {
    iVar4 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
  }
  FUN_0058b100(local_104,&DAT_005da54c,0x65bd48);
  if (iVar4 != 0) {
    if (DAT_0065a784 < 0) {
      iVar4 = 0;
    }
    else {
      iVar4 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    FUN_0058b100(local_208,s__s_s__s_005da554,&DAT_0065d6a4,iVar4 + 0x58);
  }
  iVar4 = FUN_004a13f0(local_208,&DAT_005da55c,0);
  if (iVar4 == 0) {
    iVar4 = FUN_004a13f0(local_104,&DAT_005da560,0);
    if (iVar4 == 0) {
      FUN_00481c10(s_Unable_to_find_game_script_file___005da564);
    }
  }
  local_20c = 1;
  iVar1 = FUN_004a17b0(iVar4);
  iVar2 = FUN_00482fb0(iVar1 + 1);
  iVar3 = FUN_004a15a0(iVar2,1,iVar1,iVar4);
  if (iVar3 < iVar1) {
    local_20c = 0;
  }
  else {
    *(undefined *)(iVar2 + iVar1) = 0;
    iVar3 = FUN_004834e0(iVar2,iVar1);
    if (iVar3 != 0) {
      FUN_00483540(iVar2,iVar1);
    }
    meth_0x496860(this,iVar2,in_stack_00000004);
  }
  FUN_004830f0(iVar2);
  FUN_004a1540(iVar4);
  iVar4 = 0;
  if (0 < (int)this->mbr_0x802c) {
    do {
      iVar1 = FUN_0059a530(*(undefined4 *)(this->mbr_0x803c + iVar4 * 4),in_stack_00000004);
      if (iVar1 == 0) {
        if ((-1 < iVar4) && (iVar4 = *(int *)(this->mbr_0x803c + iVar4 * 4), iVar4 != 0)) {
          *(undefined4 *)(iVar4 + 0x80) = in_stack_00000008;
          goto LAB_00496638;
        }
        break;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)this->mbr_0x802c);
  }
  _Dest = (char *)FUN_00482fb0(0x84);
  _strncpy(_Dest,in_stack_00000004,0x7f);
  _Dest[0x7f] = '\0';
  *(undefined4 *)(_Dest + 0x80) = in_stack_00000008;
  cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x802c);
LAB_00496638:
  if (DAT_00666918 != 0) {
    FUN_0045a680(8,0);
  }
  this->mbr_0x8040 = 0;
  return local_20c;
}



// Function at 004967a0

void __thiscall OOAnalyzer::cls_0x495cf0::meth_0x4967a0(cls_0x495cf0 *this)

{
  int iVar1;
  dword dVar2;
  uint uVar3;
  int in_stack_00000004;
  
  uVar3 = 0;
  if (0 < (int)this->mbr_0x8004) {
    do {
      if ((((this->mbr_0x8014 != 0) && (uVar3 < this->mbr_0x8004)) &&
          (*(int *)(this->mbr_0x8014 + uVar3 * 4) != 0)) &&
         ((iVar1 = *(int *)(this->mbr_0x8014 + uVar3 * 4),
          *(int *)(iVar1 + 0x38) == in_stack_00000004 && (iVar1 != 0)))) {
        meth_0x496f20(this,uVar3);
        FUN_004948e0();
        FUN_004830f0(iVar1);
      }
      uVar3 = uVar3 + 1;
    } while ((int)uVar3 < (int)this->mbr_0x8004);
  }
  dVar2 = this->mbr_0x802c;
  while (dVar2 = dVar2 - 1, -1 < (int)dVar2) {
    if (*(int *)(*(int *)(this->mbr_0x803c + dVar2 * 4) + 0x80) == in_stack_00000004) {
      iVar1 = *(int *)(this->mbr_0x803c + dVar2 * 4);
      if (iVar1 != 0) {
        FUN_004830f0(iVar1);
      }
      cls_0x45f7c0::meth_0x41cb80((cls_0x45f7c0 *)&this->mbr_0x802c);
    }
  }
  return;
}



// Function at 00496860

void __thiscall
OOAnalyzer::cls_0x495cf0::meth_0x496860(cls_0x495cf0 *this,byte **param_1,undefined4 param_2)

{
  uint *puVar1;
  dword *this_00;
  char cVar2;
  byte bVar3;
  cls_0x4922c0 *this_01;
  int *piVar4;
  dword dVar5;
  cls_0x5a50e8 *this_02;
  undefined4 uVar6;
  int iVar7;
  byte *pbVar8;
  uint uVar9;
  int iVar10;
  byte *pbVar11;
  char *pcVar12;
  bool bVar13;
  undefined4 in_stack_0000000c;
  cls_0x5a36f8__vftable_5a36f8 *local_60;
  char *local_5c;
  char *local_58;
  char *local_54;
  char *local_50;
  undefined4 local_4c;
  int local_48;
  int local_44;
  cls_0x5a36f8__vftable_5a36f8 **local_40;
  int local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_2c;
  undefined4 local_28;
  undefined *local_24;
  undefined local_20;
  undefined4 local_1c;
  cls_0x478720 local_14;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059da77;
  local_c = ExceptionList;
  uVar9 = 0xffffffff;
  pcVar12 = (char *)param_1;
  do {
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    cVar2 = *pcVar12;
    pcVar12 = pcVar12 + 1;
  } while (cVar2 != '\0');
  local_60 = &cls_0x5a36f8__vftable_5a36f8_005a36f8;
  local_54 = (char *)((int)param_1 + (~uVar9 - 1));
  local_58 = (char *)param_1;
  local_50 = (char *)param_1;
  local_5c = s_String_005da638;
  ExceptionList = &local_c;
  cls_0x478720::cls_0x478720(&local_14);
  local_40 = &local_60;
  local_4 = 0;
  local_4c = 0;
  local_48 = 0;
  local_44 = 0;
  local_3c = 0;
  local_38 = 0;
  local_34 = 0;
  local_2c = 0;
  local_28 = 0;
  local_20 = 0;
  local_1c = 1;
  local_24 = (undefined *)FUN_00482fb0(0x2000);
  *local_24 = 0;
  uVar6 = DAT_0066856c;
  local_4 = 1;
  DAT_0066856c = 1;
  FUN_00478a10();
  iVar10 = 0;
  if (0 < (int)this->mbr_0x8004) {
    do {
      iVar7 = cls_0x45f7c0::meth_0x410160((cls_0x45f7c0 *)&this->mbr_0x8004);
      if ((iVar7 != 0) &&
         (iVar7 = cls_0x495c20::meth_0x495c20(*(cls_0x495c20 **)(this->mbr_0x8014 + iVar10 * 4)),
         iVar7 != 0)) {
        puVar1 = (uint *)(*(int *)(this->mbr_0x8014 + iVar10 * 4) + 0x48);
        *puVar1 = *puVar1 | 1;
      }
      iVar10 = iVar10 + 1;
    } while (iVar10 < (int)this->mbr_0x8004);
  }
  uVar9 = 0;
  if (0 < (int)this->mbr_0x8018) {
    do {
      if (((this->mbr_0x8028 != 0) && (uVar9 < this->mbr_0x8018)) &&
         (*(int *)(this->mbr_0x8028 + uVar9 * 4) != 0)) {
        this_01 = *(cls_0x4922c0 **)(this->mbr_0x8028 + uVar9 * 4);
        iVar10 = cls_0x4922c0::meth_0x4943f0(this_01);
        if (iVar10 != 0) {
          cls_0x4922c0::meth_0x492490(this_01);
        }
      }
      uVar9 = uVar9 + 1;
    } while ((int)uVar9 < (int)this->mbr_0x8018);
  }
joined_r0x004969cb:
  if (local_3c == 10) {
    uVar9 = 0;
    if (0 < (int)this->mbr_0x8018) {
      do {
        if (((this->mbr_0x8028 != 0) && (uVar9 < this->mbr_0x8018)) &&
           (*(int *)(this->mbr_0x8028 + uVar9 * 4) != 0)) {
          iVar10 = *(int *)(this->mbr_0x8028 + uVar9 * 4);
          for (iVar7 = *(int *)(iVar10 + 4); iVar7 != 0; iVar7 = *(int *)(iVar7 + 8)) {
            if ((*(byte *)(iVar7 + 0x48) & 2) != 0) {
              this_02 = *(cls_0x5a50e8 **)(iVar10 + 0xc);
              if (this_02 != (cls_0x5a50e8 *)0x0) {
                this_02->mbr_0x84 = 0;
                cls_0x5a50e8::meth_0x471150(this_02);
              }
              break;
            }
          }
        }
        uVar9 = uVar9 + 1;
      } while ((int)uVar9 < (int)this->mbr_0x8018);
    }
    uVar9 = 0;
    if (0 < (int)this->mbr_0x8004) {
      do {
        if (((this->mbr_0x8014 != 0) && (uVar9 < this->mbr_0x8004)) &&
           ((*(int *)(this->mbr_0x8014 + uVar9 * 4) != 0 &&
            ((*(byte *)(*(int *)(this->mbr_0x8014 + uVar9 * 4) + 0x48) & 1) != 0)))) {
          meth_0x496f20(this,uVar9);
        }
        uVar9 = uVar9 + 1;
      } while ((int)uVar9 < (int)this->mbr_0x8004);
    }
    local_4 = 4;
    DAT_0066856c = uVar6;
    FUN_004830f0(local_24);
    if (local_48 == 0) {
joined_r0x00496ddb:
      if (local_44 != 0) {
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
    iVar10 = FUN_00482fb0(0x4c);
    local_4._0_1_ = 2;
    if (iVar10 == 0) {
      param_1 = (byte **)0x0;
    }
    else {
      param_1 = (byte **)FUN_004946f0(0,in_stack_0000000c,param_2,0);
    }
    local_4 = CONCAT31(local_4._1_3_,1);
    iVar10 = FUN_00494e20(&local_4c);
    if (iVar10 != -1) {
      uVar9 = 0;
      if (0 < (int)this->mbr_0x8004) {
        do {
          if ((((this->mbr_0x8014 != 0) && (uVar9 < this->mbr_0x8004)) &&
              (*(int *)(this->mbr_0x8014 + uVar9 * 4) != 0)) &&
             ((pbVar8 = **(byte ***)(this->mbr_0x8014 + uVar9 * 4), pbVar8 != (byte *)0x0 &&
              (pbVar11 = *param_1, pbVar11 != (byte *)0x0)))) {
            do {
              bVar3 = *pbVar8;
              bVar13 = bVar3 < *pbVar11;
              if (bVar3 != *pbVar11) {
LAB_00496a9c:
                iVar10 = (1 - (uint)bVar13) - (uint)(bVar13 != 0);
                goto LAB_00496aa1;
              }
              if (bVar3 == 0) break;
              bVar3 = pbVar8[1];
              bVar13 = bVar3 < pbVar11[1];
              if (bVar3 != pbVar11[1]) goto LAB_00496a9c;
              pbVar8 = pbVar8 + 2;
              pbVar11 = pbVar11 + 2;
            } while (bVar3 != 0);
            iVar10 = 0;
LAB_00496aa1:
            if (iVar10 == 0) {
              piVar4 = *(int **)(this->mbr_0x8014 + uVar9 * 4);
              if (*piVar4 == 0) goto LAB_00496baf;
              if (param_1 == (byte **)0x0) goto LAB_00496bc3;
              pbVar8 = param_1[1];
              pbVar11 = (byte *)piVar4[1];
              goto LAB_00496add;
            }
          }
          uVar9 = uVar9 + 1;
        } while ((int)uVar9 < (int)this->mbr_0x8004);
      }
      goto LAB_00496be3;
    }
    if (param_1 != (byte **)0x0) {
      FUN_004948e0();
      FUN_004830f0(param_1);
    }
    local_4 = 3;
    FUN_004830f0(local_24);
    if (local_48 == 0) goto joined_r0x00496ddb;
    FUN_004830f0(local_40);
    FUN_004830f0(local_48);
  }
  local_4 = 0xffffffff;
  cls_0x478720::meth_0x478730(&local_14);
  ExceptionList = local_c;
  return;
  while( true ) {
    bVar3 = pbVar11[1];
    bVar13 = bVar3 < pbVar8[1];
    if (bVar3 != pbVar8[1]) goto LAB_00496b01;
    pbVar11 = pbVar11 + 2;
    pbVar8 = pbVar8 + 2;
    if (bVar3 == 0) break;
LAB_00496add:
    bVar3 = *pbVar11;
    bVar13 = bVar3 < *pbVar8;
    if (bVar3 != *pbVar8) {
LAB_00496b01:
      iVar10 = (1 - (uint)bVar13) - (uint)(bVar13 != 0);
      goto LAB_00496b06;
    }
    if (bVar3 == 0) break;
  }
  iVar10 = 0;
LAB_00496b06:
  if (iVar10 == 0) {
LAB_00496baf:
    if (param_1 != (byte **)0x0) {
      FUN_004948e0();
      FUN_004830f0(param_1);
    }
LAB_00496bc3:
    param_1 = (byte **)0x0;
    puVar1 = (uint *)(*(int *)(this->mbr_0x8014 + uVar9 * 4) + 0x48);
    *puVar1 = *puVar1 & 0xfffffffe;
  }
  else {
    meth_0x496f20(this,uVar9);
    cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)&this->mbr_0x8004);
    if (piVar4 != (int *)0x0) {
      FUN_004948e0();
      FUN_004830f0(piVar4);
    }
    cls_0x45f7c0::meth_0x41c910((cls_0x45f7c0 *)&this->mbr_0x8004,param_1);
    puVar1 = (uint *)(*(int *)(this->mbr_0x8014 + uVar9 * 4) + 0x48);
    *puVar1 = *puVar1 | 2;
    FUN_0045a680(8,param_1);
    if (DAT_00668154 == 0) {
      FUN_0054d170(&DAT_0065c5d0,s_Updated___s__script_005da658,
                   **(undefined4 **)(this->mbr_0x8014 + uVar9 * 4));
    }
    else {
      FUN_0041ee50(s_Updated___s__script_005da640,**(undefined4 **)(this->mbr_0x8014 + uVar9 * 4));
    }
  }
LAB_00496be3:
  dVar5 = this->mbr_0x8004;
  this_00 = &this->mbr_0x8004;
  if ((int)dVar5 <= (int)uVar9) {
    uVar9 = 0;
    if (0 < (int)dVar5) {
      do {
        if (((this->mbr_0x8014 == 0) || (*this_00 <= uVar9)) ||
           (*(int *)(this->mbr_0x8014 + uVar9 * 4) == 0)) {
          FUN_0045a680(5,*(undefined4 *)(this->mbr_0x8014 + uVar9 * 4));
          cls_0x45f7c0::meth_0x41c910((cls_0x45f7c0 *)this_00,param_1);
          break;
        }
        uVar9 = uVar9 + 1;
      } while ((int)uVar9 < (int)dVar5);
    }
    if ((int)*this_00 <= (int)uVar9) {
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)this_00);
    }
    FUN_0045a680(8,param_1);
  }
  goto joined_r0x004969cb;
}



// Function at 00496f20

void __thiscall OOAnalyzer::cls_0x495cf0::meth_0x496f20(cls_0x495cf0 *this,int param_1)

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

void __thiscall OOAnalyzer::cls_0x495cf0::meth_0x4970b0(cls_0x495cf0 *this)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  if (0 < (int)this->mbr_0x802c) {
    do {
      uVar1 = *(undefined4 *)(this->mbr_0x803c + iVar3 * 4);
      iVar2 = meth_0x496490(this);
      if (iVar2 != 0) {
        if (DAT_00668154 == 0) {
          FUN_0054d170(&DAT_0065c5d0,s_Unable_to_reload_script_file___s_005da690,uVar1);
        }
        else {
          FUN_0041ee50(s_Unable_to_reload_script_file___s_005da66c);
        }
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)this->mbr_0x802c);
  }
  return;
}



