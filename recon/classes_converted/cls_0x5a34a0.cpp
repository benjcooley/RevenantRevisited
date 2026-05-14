#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a34a0



// Function at 00401c50

void cls_0x5a34a0::virt_meth_0x401c50()

{
  int iVar1;
  
  iVar1 = 0;
  if (0 < (int)this->mbr_0x18) {
    do {
      FUN_004830f0(*(LPCVOID *)(this->mbr_0x40 + iVar1 * 4));
      iVar1 = iVar1 + 1;
    } while (iVar1 < (int)this->mbr_0x18);
  }
  FUN_004830f0((LPCVOID)this->mbr_0x40);
  this->virt_meth_0x40db50();
  return;
}



// Function at 00405b40

undefined4 * cls_0x5a34a0::virt_meth_0x405b40(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4)

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
  puVar1 = FUN_00482fb0(0x44);
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

cls_0x5a34a0 * cls_0x5a34a0::virt_meth_0x405bc0()

{
  byte in_stack_00000004;
  
  ~cls_0x5a34a0(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00405be0

cls_0x5a34a0 * __thiscall cls_0x5a34a0::~cls_0x5a34a0(cls_0x5a34a0 *this)

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
      FUN_004830f0(*(LPCVOID *)(this->mbr_0x40 + iVar2 * 4));
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x18);
  }
  FUN_004830f0((LPCVOID)this->mbr_0x40);
  this->virt_meth_0x40db50();
  this->vftptr_0x0 = &cls_0x5a34a0__vftable_5a34a0_005a34a0;
  local_4 = 1;
  this->virt_meth_0x40db50();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a34a0 *)FUN_004830f0((LPCVOID)this->mbr_0x28);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00406100

cls_0x5a34a0 * cls_0x5a34a0::virt_meth_0x406100()

{
  byte in_stack_00000004;
  
  cls_0x406070::~cls_0x406070((cls_0x406070 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0040d4a0

undefined4 cls_0x5a34a0::virt_meth_0x40d4a0(byte *param_1)

{
  byte bVar1;
  uint *puVar2;
  dword dVar3;
  cls_0x5a35ac *this_00;
  bool bVar4;
  uint uVar5;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  byte *pbVar6;
  int iVar7;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int iVar8;
  uint *puVar9;
  dword *this_01;
  byte *in_stack_00000008;
  
  uVar5 = FUN_0059a530_stricmp((uint)this,param_1,&DAT_005c612c);
  if (uVar5 != 0) {
    return 0;
  }
  bVar4 = FUN_00479700((uint)in_stack_00000008,&DAT_005c6130,0);
  if (CONCAT31(extraout_var,bVar4) == 0) {
    if ((*(int *)(in_stack_00000008 + 0x10) != 4) && (*(int *)(in_stack_00000008 + 0x10) != 2)) {
      return 0;
    }
    puVar2 = *(uint **)(in_stack_00000008 + 0x28);
    dVar3 = this->mbr_0xc;
    FUN_0059bd3e(puVar2);
    iVar8 = 0;
    if (0 < *(int *)(dVar3 + 0x44)) {
      do {
        iVar7 = *(int *)(*(int *)(dVar3 + 0x54) + iVar8 * 4);
        if (iVar7 != 0) {
          this_00 = *(cls_0x5a35ac **)(dVar3 + 8);
          iVar7 = *(int *)(iVar7 + 4);
          if ((this_00->cls_0x5a486c).mbr_0xc == 0) {
            cls_0x5a35ac::meth_0x4109d0(this_00);
            cls_0x5a35ac::meth_0x407510(this_00);
          }
          if (((-1 < iVar7) &&
              (this_01 = &(this_00->cls_0x5a486c).mbr_0x64,
              iVar7 <= (int)(this_00->cls_0x5a486c).mbr_0x64)) &&
             (iVar7 = cls_0x45f7c0::meth_0x410160((cls_0x45f7c0 *)this_01), iVar7 != 0)) {
            cls_0x45f7c0::meth_0x4110a0((cls_0x45f7c0 *)this_01);
          }
          pbVar6 = (byte *)cls_0x5a486c::meth_0x409200(*(cls_0x5a486c **)(dVar3 + 8));
          puVar9 = puVar2;
          do {
            bVar1 = *pbVar6;
            bVar4 = bVar1 < *(byte *)puVar9;
            if (bVar1 != *(byte *)puVar9) {
LAB_0040d6d4:
              iVar7 = (1 - (uint)bVar4) - (uint)(bVar4 != 0);
              goto LAB_0040d6d9;
            }
            if (bVar1 == 0) break;
            bVar1 = pbVar6[1];
            bVar4 = bVar1 < *(byte *)((int)puVar9 + 1);
            if (bVar1 != *(byte *)((int)puVar9 + 1)) goto LAB_0040d6d4;
            pbVar6 = pbVar6 + 2;
            puVar9 = (uint *)((int)puVar9 + 2);
          } while (bVar1 != 0);
          iVar7 = 0;
LAB_0040d6d9:
          if (iVar7 == 0) {
            if (-1 < iVar8) {
              cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x18);
            }
            break;
          }
        }
        iVar8 = iVar8 + 1;
        if (*(int *)(dVar3 + 0x44) <= iVar8) {
          FUN_00479580(in_stack_00000008);
          return 1;
        }
      } while( true );
    }
    FUN_00479580(in_stack_00000008);
  }
  else {
    FUN_00479580(in_stack_00000008);
    iVar8 = *(int *)(in_stack_00000008 + 0x10);
    while ((iVar8 != 10 &&
           (bVar4 = FUN_00479700((uint)in_stack_00000008,&DAT_005c6134,0),
           CONCAT31(extraout_var_00,bVar4) == 0))) {
      if ((*(int *)(in_stack_00000008 + 0x10) != 4) && (*(int *)(in_stack_00000008 + 0x10) != 2)) {
        return 0;
      }
      puVar2 = *(uint **)(in_stack_00000008 + 0x28);
      dVar3 = this->mbr_0xc;
      FUN_0059bd3e(puVar2);
      iVar8 = 0;
      if (0 < *(int *)(dVar3 + 0x44)) {
        do {
          if (*(int *)(*(int *)(dVar3 + 0x54) + iVar8 * 4) != 0) {
            cls_0x5a486c::meth_0x409200(*(cls_0x5a486c **)(dVar3 + 8));
            pbVar6 = (byte *)cls_0x5a486c::meth_0x409200(*(cls_0x5a486c **)(dVar3 + 8));
            puVar9 = puVar2;
            do {
              bVar1 = *pbVar6;
              bVar4 = bVar1 < *(byte *)puVar9;
              if (bVar1 != *(byte *)puVar9) {
LAB_0040d586:
                iVar7 = (1 - (uint)bVar4) - (uint)(bVar4 != 0);
                goto LAB_0040d58b;
              }
              if (bVar1 == 0) break;
              bVar1 = pbVar6[1];
              bVar4 = bVar1 < *(byte *)((int)puVar9 + 1);
              if (bVar1 != *(byte *)((int)puVar9 + 1)) goto LAB_0040d586;
              pbVar6 = pbVar6 + 2;
              puVar9 = (uint *)((int)puVar9 + 2);
            } while (bVar1 != 0);
            iVar7 = 0;
LAB_0040d58b:
            if (iVar7 == 0) {
              if (-1 < iVar8) {
                cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x18);
              }
              break;
            }
          }
          iVar8 = iVar8 + 1;
        } while (iVar8 < *(int *)(dVar3 + 0x44));
      }
      FUN_00479580(in_stack_00000008);
      bVar4 = FUN_00479700((uint)in_stack_00000008,&DAT_005c6138,0);
      if (CONCAT31(extraout_var_01,bVar4) != 0) {
        FUN_00479580(in_stack_00000008);
      }
      iVar8 = *(int *)(in_stack_00000008 + 0x10);
    }
    bVar4 = FUN_00479700((uint)in_stack_00000008,&DAT_005c613c,0);
    if (CONCAT31(extraout_var_02,bVar4) != 0) {
      FUN_00479580(in_stack_00000008);
      return 1;
    }
  }
  return 1;
}



// Function at 0040d750

undefined4 cls_0x5a34a0::virt_meth_0x40d750()

{
  char cVar1;
  bool bVar2;
  undefined uVar3;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  FILE **ppFVar4;
  undefined3 extraout_var_01;
  undefined4 uVar5;
  undefined3 extraout_var_02;
  uint uVar6;
  char *pcVar7;
  char *in_stack_00000004;
  FILE **ppFVar8;
  LPCVOID local_1b0;
  FILE **local_1ac;
  LPCVOID local_1a8;
  cls_0x5a36f8__vftable_5a36f8 **local_1a4;
  undefined4 local_1a0;
  undefined4 local_19c;
  undefined4 local_198;
  undefined4 local_190;
  LPCVOID local_18c;
  undefined4 *local_188;
  undefined local_184;
  undefined4 local_180;
  undefined auStack_17c [12];
  LPCVOID local_170;
  FILE **local_16c;
  LPCVOID local_168;
  undefined4 *local_164;
  undefined4 local_160;
  undefined4 local_15c;
  undefined4 local_158;
  undefined4 local_150;
  LPCVOID local_14c;
  undefined4 *local_148;
  undefined local_144;
  undefined4 local_140;
  undefined auStack_13c [12];
  undefined4 local_130;
  FILE **local_12c;
  FILE **local_128;
  undefined4 local_124;
  cls_0x5a36f8__vftable_5a36f8 *local_120;
  char *local_11c;
  char *local_114;
  FILE *local_10c [63];
  void *pvStack_10;
  void *local_c;
  undefined4 uStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  uStack_8 = &LAB_0059c644;
  local_c = ExceptionList;
  uVar6 = 0xffffffff;
  pcVar7 = in_stack_00000004;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  local_120 = &cls_0x5a36f8__vftable_5a36f8_005a36f8;
  local_114 = in_stack_00000004 + (~uVar6 - 1);
  local_11c = s_String_005c6140;
  ExceptionList = &local_c;
  cls_0x478720::cls_0x478720((cls_0x478720 *)(auStack_17c + 4));
  local_1a4 = &local_120;
  local_4 = 0;
  local_1b0 = (LPCVOID)0x0;
  local_1ac = (FILE **)0x0;
  local_1a8 = (LPCVOID)0x0;
  local_1a0 = 0;
  local_19c = 0;
  local_198 = 0;
  local_190 = 0;
  local_18c = (LPCVOID)0x0;
  local_184 = 0;
  local_180 = 1;
  local_188 = FUN_00482fb0(0x2000);
  *(undefined *)local_188 = 0;
  local_4 = 1;
  FUN_00479580((byte *)&local_1b0);
  bVar2 = FUN_00479700((uint)&local_1b0,(byte *)s_filename_005c6148,0);
  if (CONCAT31(extraout_var,bVar2) == 0) {
    uVar3 = (*this->vftptr_0x0->virt_meth_0x40d3e0_0)(this);
    if (CONCAT31(extraout_var_02,uVar3) != 1) {
      uStack_8 = (undefined *)0xffffffff;
      FUN_00411490((int)&stack0xfffffe4c);
      ExceptionList = pvStack_10;
      return 0;
    }
LAB_0040daf2:
    uStack_8 = (undefined *)0xffffffff;
    FUN_00411490((int)&stack0xfffffe4c);
    uVar5 = 1;
  }
  else {
    FUN_00479580((byte *)&local_1b0);
    bVar2 = FUN_00479700((uint)&local_1b0,&DAT_005c6154,0);
    if (CONCAT31(extraout_var_00,bVar2) != 0) {
      FUN_00479580((byte *)&local_1b0);
    }
    FUN_0058b100((char *)local_10c,(byte *)s__s_s_s_005c6160);
    FUN_00479580((byte *)&local_1b0);
    ppFVar8 = local_10c;
    ppFVar4 = (FILE **)FUN_004a1240(ppFVar8,(uint *)&DAT_005c6168,0);
    if (ppFVar4 == (FILE **)0x0) {
      local_4 = 2;
      FUN_004830f0(local_188);
      if (local_1ac == (FILE **)0x0) {
        if (local_1a8 != (LPCVOID)0x0) {
          FUN_004830f0(local_1a4);
LAB_0040da66:
          FUN_004a1540(ppFVar8);
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
      local_12c = ppFVar4;
      cls_0x478720::cls_0x478720((cls_0x478720 *)(auStack_13c + 4));
      local_164 = &local_130;
      local_4._0_1_ = 3;
      local_170 = (LPCVOID)0x0;
      local_16c = (FILE **)0x0;
      local_168 = (LPCVOID)0x0;
      local_160 = 0;
      local_15c = 0;
      local_158 = 0;
      local_150 = 0;
      local_14c = (LPCVOID)0x0;
      local_144 = 0;
      local_140 = 1;
      local_148 = FUN_00482fb0(0x2000);
      *(undefined *)local_148 = 0;
      local_4 = CONCAT31(local_4._1_3_,4);
      FUN_00479580((byte *)&local_170);
      uVar3 = (*this->vftptr_0x0->virt_meth_0x40d3e0_0)(this);
      if (CONCAT31(extraout_var_01,uVar3) == 1) {
        FUN_004a1540(ppFVar4);
        uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,7);
        FUN_004830f0(local_14c);
        if (local_170 == (LPCVOID)0x0) {
          if (local_16c != (FILE **)0x0) {
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
      FUN_004a1540(ppFVar4);
      uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,5);
      FUN_004830f0(local_14c);
      if (local_170 == (LPCVOID)0x0) {
        if (local_16c != (FILE **)0x0) {
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
      if (local_1b0 == (LPCVOID)0x0) {
        if (local_1ac != (FILE **)0x0) {
          FUN_004830f0(local_1a8);
          ppFVar8 = local_1ac;
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
    uVar5 = 0;
  }
  ExceptionList = pvStack_10;
  return uVar5;
}



