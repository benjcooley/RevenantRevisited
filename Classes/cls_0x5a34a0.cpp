// Decompiled methods and structure for class: cls_0x5a34a0

/*
/OOAnalyzer/cls_0x5a34a0
pack(disabled)
Structure cls_0x5a34a0 {
   0   cls_0x5a34a0::vftable_5a34a0 *   4   vftptr_0x0   "pointer to cls_0x5a34a0::vftable_5a34a0"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 68 Alignment: 1

*/

// Function at 00401c50

void __thiscall OOAnalyzer::cls_0x5a34a0::virt_meth_0x401c50(cls_0x5a34a0 *this)

{
  int iVar1;
  
  iVar1 = 0;
  if (0 < (int)this->mbr_0x18) {
    do {
      FUN_004830f0(*(undefined4 *)(this->mbr_0x40 + iVar1 * 4));
      iVar1 = iVar1 + 1;
    } while (iVar1 < (int)this->mbr_0x18);
  }
  FUN_004830f0(this->mbr_0x40);
  cls_0x5a3544::virt_meth_0x40db50((cls_0x5a3544 *)this);
  return;
}



// Function at 00405b40

undefined4 * __thiscall
OOAnalyzer::cls_0x5a34a0::virt_meth_0x405b40
          (cls_0x5a34a0 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,
          undefined4 param_4)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 in_stack_00000014;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059c3fb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  puVar1 = (undefined4 *)FUN_00482fb0(0x44);
  local_4 = 0;
  puVar2 = (undefined4 *)0x0;
  if (puVar1 != (undefined4 *)0x0) {
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)(puVar1 + 6),0x10);
    puVar1[1] = param_1;
    puVar1[2] = param_2;
    puVar1[4] = param_4;
    puVar1[3] = param_3;
    puVar1[5] = in_stack_00000014;
    *puVar1 = &PTR_virt_meth_0x40d3e0_005a34c4;
    puVar2 = puVar1;
  }
  ExceptionList = local_c;
  return puVar2;
}



// Function at 00405bc0

cls_0x5a34a0 * __thiscall OOAnalyzer::cls_0x5a34a0::virt_meth_0x405bc0(cls_0x5a34a0 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a34a0(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00405be0

cls_0x5a34a0 * __thiscall OOAnalyzer::cls_0x5a34a0::~cls_0x5a34a0(cls_0x5a34a0 *this)

{
  cls_0x5a34a0 *pcVar1;
  int iVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059c423;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a34a0__vftable_5a34a0 *)&PTR_virt_meth_0x40d3e0_005a34c4;
  iVar2 = 0;
  local_4 = 0;
  if (0 < (int)this->mbr_0x18) {
    do {
      FUN_004830f0(*(undefined4 *)(this->mbr_0x40 + iVar2 * 4));
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x18);
  }
  FUN_004830f0(this->mbr_0x40);
  cls_0x5a3544::virt_meth_0x40db50((cls_0x5a3544 *)this);
  this->vftptr_0x0 = &cls_0x5a34a0__vftable_5a34a0_005a34a0;
  local_4 = 1;
  cls_0x5a3544::virt_meth_0x40db50((cls_0x5a3544 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a34a0 *)FUN_004830f0(this->mbr_0x28);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00406100

cls_0x5a34a0 * __thiscall OOAnalyzer::cls_0x5a34a0::virt_meth_0x406100(cls_0x5a34a0 *this)

{
  byte in_stack_00000004;
  
  cls_0x406070::~cls_0x406070((cls_0x406070 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0040d4a0

undefined4 __thiscall
OOAnalyzer::cls_0x5a34a0::virt_meth_0x40d4a0(cls_0x5a34a0 *this,undefined4 param_1)

{
  byte bVar1;
  byte *pbVar2;
  dword dVar3;
  cls_0x5a35ac *this_00;
  int iVar4;
  byte *pbVar5;
  int iVar6;
  byte *pbVar7;
  dword *this_01;
  bool bVar8;
  int in_stack_00000008;
  
  iVar4 = FUN_0059a530(param_1,&DAT_005c612c);
  if (iVar4 != 0) {
    return 0;
  }
  iVar4 = FUN_00479700(&DAT_005c6130,0);
  if (iVar4 == 0) {
    if ((*(int *)(in_stack_00000008 + 0x10) != 4) && (*(int *)(in_stack_00000008 + 0x10) != 2)) {
      return 0;
    }
    pbVar2 = *(byte **)(in_stack_00000008 + 0x28);
    dVar3 = this->mbr_0xc;
    FUN_0059bd3e(pbVar2);
    iVar4 = 0;
    if (0 < *(int *)(dVar3 + 0x44)) {
      do {
        iVar6 = *(int *)(*(int *)(dVar3 + 0x54) + iVar4 * 4);
        if (iVar6 != 0) {
          this_00 = *(cls_0x5a35ac **)(dVar3 + 8);
          iVar6 = *(int *)(iVar6 + 4);
          if ((this_00->cls_0x5a486c).mbr_0xc == 0) {
            cls_0x5a35ac::meth_0x4109d0(this_00);
            cls_0x5a35ac::meth_0x407510(this_00);
          }
          if (((-1 < iVar6) &&
              (this_01 = &(this_00->cls_0x5a486c).mbr_0x64,
              iVar6 <= (int)(this_00->cls_0x5a486c).mbr_0x64)) &&
             (iVar6 = cls_0x45f7c0::meth_0x410160((cls_0x45f7c0 *)this_01), iVar6 != 0)) {
            cls_0x45f7c0::meth_0x4110a0((cls_0x45f7c0 *)this_01);
          }
          pbVar5 = (byte *)cls_0x5a486c::meth_0x409200(*(cls_0x5a486c **)(dVar3 + 8));
          pbVar7 = pbVar2;
          do {
            bVar1 = *pbVar5;
            bVar8 = bVar1 < *pbVar7;
            if (bVar1 != *pbVar7) {
LAB_0040d6d4:
              iVar6 = (1 - (uint)bVar8) - (uint)(bVar8 != 0);
              goto LAB_0040d6d9;
            }
            if (bVar1 == 0) break;
            bVar1 = pbVar5[1];
            bVar8 = bVar1 < pbVar7[1];
            if (bVar1 != pbVar7[1]) goto LAB_0040d6d4;
            pbVar5 = pbVar5 + 2;
            pbVar7 = pbVar7 + 2;
          } while (bVar1 != 0);
          iVar6 = 0;
LAB_0040d6d9:
          if (iVar6 == 0) {
            if (-1 < iVar4) {
              cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x18);
            }
            break;
          }
        }
        iVar4 = iVar4 + 1;
        if (*(int *)(dVar3 + 0x44) <= iVar4) {
          FUN_00479580();
          return 1;
        }
      } while( true );
    }
    FUN_00479580();
  }
  else {
    FUN_00479580();
    iVar4 = *(int *)(in_stack_00000008 + 0x10);
    while ((iVar4 != 10 && (iVar4 = FUN_00479700(&DAT_005c6134,0), iVar4 == 0))) {
      if ((*(int *)(in_stack_00000008 + 0x10) != 4) && (*(int *)(in_stack_00000008 + 0x10) != 2)) {
        return 0;
      }
      pbVar2 = *(byte **)(in_stack_00000008 + 0x28);
      dVar3 = this->mbr_0xc;
      FUN_0059bd3e(pbVar2);
      iVar4 = 0;
      if (0 < *(int *)(dVar3 + 0x44)) {
        do {
          if (*(int *)(*(int *)(dVar3 + 0x54) + iVar4 * 4) != 0) {
            cls_0x5a486c::meth_0x409200(*(cls_0x5a486c **)(dVar3 + 8));
            pbVar5 = (byte *)cls_0x5a486c::meth_0x409200(*(cls_0x5a486c **)(dVar3 + 8));
            pbVar7 = pbVar2;
            do {
              bVar1 = *pbVar5;
              bVar8 = bVar1 < *pbVar7;
              if (bVar1 != *pbVar7) {
LAB_0040d586:
                iVar6 = (1 - (uint)bVar8) - (uint)(bVar8 != 0);
                goto LAB_0040d58b;
              }
              if (bVar1 == 0) break;
              bVar1 = pbVar5[1];
              bVar8 = bVar1 < pbVar7[1];
              if (bVar1 != pbVar7[1]) goto LAB_0040d586;
              pbVar5 = pbVar5 + 2;
              pbVar7 = pbVar7 + 2;
            } while (bVar1 != 0);
            iVar6 = 0;
LAB_0040d58b:
            if (iVar6 == 0) {
              if (-1 < iVar4) {
                cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x18);
              }
              break;
            }
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 < *(int *)(dVar3 + 0x44));
      }
      FUN_00479580();
      iVar4 = FUN_00479700(&DAT_005c6138,0);
      if (iVar4 != 0) {
        FUN_00479580();
      }
      iVar4 = *(int *)(in_stack_00000008 + 0x10);
    }
    iVar4 = FUN_00479700(&DAT_005c613c,0);
    if (iVar4 != 0) {
      FUN_00479580();
      return 1;
    }
  }
  return 1;
}



// Function at 0040d750

undefined4 __thiscall OOAnalyzer::cls_0x5a34a0::virt_meth_0x40d750(cls_0x5a34a0 *this)

{
  char cVar1;
  undefined uVar2;
  int iVar3;
  undefined3 extraout_var;
  undefined4 uVar4;
  undefined3 extraout_var_00;
  uint uVar5;
  char *pcVar6;
  char *in_stack_00000004;
  undefined *puVar7;
  int *piVar8;
  int local_1b0;
  undefined *local_1ac;
  int local_1a8;
  cls_0x5a36f8__vftable_5a36f8 **local_1a4;
  undefined4 local_1a0;
  undefined4 local_19c;
  undefined4 local_198;
  undefined4 local_190;
  undefined4 local_18c;
  undefined *local_188;
  undefined local_184;
  undefined4 local_180;
  undefined auStack_17c [12];
  int local_170;
  int local_16c;
  undefined4 local_168;
  undefined4 *local_164;
  undefined4 local_160;
  undefined4 local_15c;
  undefined4 local_158;
  undefined4 local_150;
  undefined4 local_14c;
  undefined *local_148;
  undefined local_144;
  undefined4 local_140;
  undefined auStack_13c [12];
  undefined4 local_130;
  int local_12c;
  undefined *local_128;
  undefined4 local_124;
  cls_0x5a36f8__vftable_5a36f8 *local_120;
  char *local_11c;
  char *local_114;
  undefined local_10c [252];
  void *pvStack_10;
  void *local_c;
  undefined4 uStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  uStack_8 = &LAB_0059c644;
  local_c = ExceptionList;
  uVar5 = 0xffffffff;
  pcVar6 = in_stack_00000004;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  local_120 = &cls_0x5a36f8__vftable_5a36f8_005a36f8;
  local_114 = in_stack_00000004 + (~uVar5 - 1);
  local_11c = s_String_005c6140;
  ExceptionList = &local_c;
  cls_0x478720::cls_0x478720((cls_0x478720 *)(auStack_17c + 4));
  local_1a4 = &local_120;
  local_4 = 0;
  local_1b0 = 0;
  local_1ac = (undefined *)0x0;
  local_1a8 = 0;
  local_1a0 = 0;
  local_19c = 0;
  local_198 = 0;
  local_190 = 0;
  local_18c = 0;
  local_184 = 0;
  local_180 = 1;
  local_188 = (undefined *)FUN_00482fb0(0x2000);
  *local_188 = 0;
  local_4 = 1;
  FUN_00479580();
  iVar3 = FUN_00479700(s_filename_005c6148,0);
  if (iVar3 == 0) {
    piVar8 = &local_1b0;
    uVar2 = (*this->vftptr_0x0->virt_meth_0x40d3e0_0)(this);
    if (CONCAT31(extraout_var_00,uVar2) != 1) {
      uStack_8 = (undefined *)0xffffffff;
      FUN_00411490(piVar8);
      ExceptionList = pvStack_10;
      return 0;
    }
LAB_0040daf2:
    uStack_8 = (undefined *)0xffffffff;
    FUN_00411490(piVar8);
    uVar4 = 1;
  }
  else {
    FUN_00479580();
    iVar3 = FUN_00479700(&DAT_005c6154,0);
    if (iVar3 != 0) {
      FUN_00479580();
    }
    FUN_0058b100(local_10c,s__s_s_s_005c6160,0x65bd48,local_188,&DAT_005c6158);
    FUN_00479580();
    puVar7 = local_10c;
    iVar3 = FUN_004a1240(puVar7,&DAT_005c6168,0);
    if (iVar3 == 0) {
      local_4 = 2;
      FUN_004830f0(local_188);
      if (local_1ac == (undefined *)0x0) {
        if (local_1a8 != 0) {
          FUN_004830f0(local_1a4);
          iVar3 = local_1a8;
LAB_0040da66:
          FUN_004a1540(puVar7,iVar3);
        }
      }
      else {
        FUN_004830f0(local_1a4);
        FUN_004830f0(local_1ac);
      }
    }
    else {
      local_128 = local_10c;
      local_130 = 0x5a36e4;
      local_124 = 0;
      local_12c = iVar3;
      cls_0x478720::cls_0x478720((cls_0x478720 *)(auStack_13c + 4));
      local_164 = &local_130;
      local_4._0_1_ = 3;
      local_170 = 0;
      local_16c = 0;
      local_168 = 0;
      local_160 = 0;
      local_15c = 0;
      local_158 = 0;
      local_150 = 0;
      local_14c = 0;
      local_144 = 0;
      local_140 = 1;
      local_148 = (undefined *)FUN_00482fb0(0x2000);
      *local_148 = 0;
      local_4 = CONCAT31(local_4._1_3_,4);
      FUN_00479580();
      piVar8 = &local_170;
      uVar2 = (*this->vftptr_0x0->virt_meth_0x40d3e0_0)(this);
      if (CONCAT31(extraout_var,uVar2) == 1) {
        FUN_004a1540(iVar3,piVar8);
        uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,7);
        FUN_004830f0(local_14c);
        if (local_170 == 0) {
          if (local_16c != 0) {
            FUN_004830f0(local_168);
            FUN_004a1540(local_16c);
          }
        }
        else {
          FUN_004830f0(local_168);
          FUN_004830f0(local_170);
        }
        uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,1);
        cls_0x478720::meth_0x478730((cls_0x478720 *)auStack_13c);
        goto LAB_0040daf2;
      }
      FUN_004a1540(iVar3);
      uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,5);
      FUN_004830f0(local_14c);
      if (local_170 == 0) {
        if (local_16c != 0) {
          FUN_004830f0(local_168);
          FUN_004a1540(local_16c);
        }
      }
      else {
        FUN_004830f0(local_168);
        FUN_004830f0(local_170);
      }
      uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,1);
      cls_0x478720::meth_0x478730((cls_0x478720 *)auStack_13c);
      uStack_8 = (undefined *)0x6;
      FUN_004830f0(local_18c);
      if (local_1b0 == 0) {
        if (local_1ac != (undefined *)0x0) {
          iVar3 = local_1a8;
          FUN_004830f0(local_1a8);
          puVar7 = local_1ac;
          goto LAB_0040da66;
        }
      }
      else {
        FUN_004830f0(local_1a8);
        FUN_004830f0(local_1b0);
      }
    }
    uStack_8 = (undefined *)0xffffffff;
    cls_0x478720::meth_0x478730((cls_0x478720 *)auStack_17c);
    uVar4 = 0;
  }
  ExceptionList = pvStack_10;
  return uVar4;
}



