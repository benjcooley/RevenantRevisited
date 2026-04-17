// Decompiled methods and structure for class: cls_0x49ead0

/*
/OOAnalyzer/cls_0x49ead0
pack(disabled)
Structure cls_0x49ead0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
   399   byte   1   mbr_0x18f   "Unsigned Byte (db)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   404   cls_0x41c7f0   24   cls_0x41c7f0   "Component (member) class."
   444   cls_0x41c7f0   24   cls_0x41c7f0   "Component (member) class."
   468   cls_0x41c7f0   24   cls_0x41c7f0   "Component (member) class."
   492   dword   4   mbr_0x1ec   "Unsigned Double-Word (ddw, 4-bytes)"
   496   dword   4   mbr_0x1f0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 500 Alignment: 1

*/

// Function at 0049ead0

cls_0x49ead0 * __thiscall
OOAnalyzer::cls_0x49ead0::cls_0x49ead0(cls_0x49ead0 *this,undefined4 param_1)

{
  int iVar1;
  uint uVar2;
  dword *pdVar3;
  undefined4 *puVar4;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059dd1c;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x100);
  local_4 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->cls_0x41c7f0).mbr_0x14,0x200);
  local_4._0_1_ = 1;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x200);
  local_4._0_1_ = 2;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x200);
  this->mbr_0x0 = 0;
  this->mbr_0x4 = 0;
  this->mbr_0x8 = 0xffffffff;
  pdVar3 = &this->mbr_0x110;
  for (iVar1 = 0x20; iVar1 != 0; iVar1 = iVar1 + -1) {
    *pdVar3 = 0;
    pdVar3 = pdVar3 + 1;
  }
  pdVar3 = &this->mbr_0xc;
  for (iVar1 = 0x41; iVar1 != 0; iVar1 = iVar1 + -1) {
    *pdVar3 = 0;
    pdVar3 = pdVar3 + 1;
  }
  puVar4 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
  local_4 = CONCAT31(local_4._1_3_,3);
  this->mbr_0x190 = 0;
  this->mbr_0x1f0 = 0;
  if (puVar4 != (undefined4 *)0x0) {
    for (uVar2 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
      *(undefined *)puVar4 = 0;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
  }
  puVar4 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  if (puVar4 != (undefined4 *)0x0) {
    for (uVar2 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
      *(undefined *)puVar4 = 0;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
  }
  puVar4 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  if (puVar4 != (undefined4 *)0x0) {
    for (uVar2 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
      *(undefined *)puVar4 = 0;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
  }
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x14 = 0;
  this->mbr_0x1ec = 0;
  meth_0x49ee20(this,param_1);
  ExceptionList = local_c;
  return this;
}



// Function at 0049ec40

void __thiscall OOAnalyzer::cls_0x49ead0::meth_0x49ec40(cls_0x49ead0 *this)

{
  cls_0x41c7f0 *this_00;
  int iVar1;
  dword dVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059dd5e;
  local_c = ExceptionList;
  local_4 = 3;
  ExceptionList = &local_c;
  if (this->mbr_0x190 != 0) {
    ExceptionList = &local_c;
    meth_0x49eff0(this);
  }
  this_00 = &this->cls_0x41c7f0;
  iVar3 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar3) && (iVar1 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4), iVar1 != 0))
      {
        FUN_004830f0(iVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)this_00->mbr_0x0);
  }
  this_00->mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  dVar2 = this->mbr_0x1f0;
  if (dVar2 != 0) {
    if (*(int *)(dVar2 + 0x18) != 0) {
      FUN_004830f0(*(int *)(dVar2 + 0x18));
    }
    FUN_004830f0(dVar2);
  }
  local_4._0_1_ = 2;
  FUN_004830f0((this->cls_0x41c7f0).mbr_0x10);
  local_4._0_1_ = 1;
  FUN_004830f0((this->cls_0x41c7f0).mbr_0x10);
  local_4 = (uint)local_4._1_3_ << 8;
  FUN_004830f0(*(undefined4 *)&this->field_0x1b8);
  local_4 = 0xffffffff;
  FUN_004830f0((this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 0049ee20

undefined4 __thiscall
OOAnalyzer::cls_0x49ead0::meth_0x49ee20(cls_0x49ead0 *this,cls_0x49ead0 *param_1)

{
  char cVar1;
  dword dVar2;
  undefined4 uVar3;
  char *pcVar4;
  int iVar5;
  void *extraout_ECX;
  void *extraout_ECX_00;
  void *this_00;
  uint uVar6;
  uint uVar7;
  undefined4 *puVar8;
  dword *pdVar9;
  dword *pdVar10;
  cls_0x49ead0 *pcVar11;
  uint in_stack_00000008;
  undefined4 local_104 [65];
  
  if (this->mbr_0x190 != 0) {
    meth_0x49eff0(this);
  }
  this->mbr_0x4 = 0;
  this->mbr_0x0 = in_stack_00000008 | 4;
  if ((in_stack_00000008 & 1) == 0) {
    if ((in_stack_00000008 & 2) == 0) {
      this->mbr_0x4 = 6;
      return 0;
    }
    dVar2 = FUN_0058b5db(param_1,&DAT_005dad0c);
    this->mbr_0x190 = dVar2;
    this_00 = extraout_ECX_00;
    if (dVar2 == 0) {
      this->mbr_0x4 = 3;
      return 0;
    }
  }
  else {
    dVar2 = FUN_0058b5db(param_1,&DAT_005dad08);
    this->mbr_0x190 = dVar2;
    if (dVar2 == 0) {
      this->mbr_0x4 = 2;
      return 0;
    }
    puVar8 = local_104;
    for (iVar5 = 0x41; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar8 = 0;
      puVar8 = puVar8 + 1;
    }
    uVar3 = FUN_0059bfec(dVar2);
    FUN_0058e947(uVar3);
    iVar5 = meth_0x49fb80(this,0,local_104);
    this_00 = extraout_ECX;
    if (iVar5 == 0) {
      meth_0x49eff0(this);
      return 0;
    }
  }
  pcVar4 = _strrchr(this_00,param_1,0x5c);
  if (pcVar4 == (char *)0x0) {
    this->mbr_0x4 = 1;
    meth_0x49eff0(this);
    return 0;
  }
  pdVar10 = &this->mbr_0x110;
  _strncpy((char *)pdVar10,pcVar4 + 1,0x7f);
  uVar6 = 0xffffffff;
  this->mbr_0x18f = 0;
  pdVar9 = pdVar10;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    cVar1 = *(char *)pdVar9;
    pdVar9 = (dword *)((int)pdVar9 + 1);
  } while (cVar1 != '\0');
  uVar7 = 0xffffffff;
  pcVar11 = param_1;
  do {
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    pdVar9 = &pcVar11->mbr_0x0;
    pcVar11 = (cls_0x49ead0 *)((int)&pcVar11->mbr_0x0 + 1);
  } while (*(char *)pdVar9 != '\0');
  if (~uVar7 - (~uVar6 - 1) < 0x104) {
    uVar6 = 0xffffffff;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *(char *)pdVar10;
      pdVar10 = (dword *)((int)pdVar10 + 1);
    } while (cVar1 != '\0');
    uVar7 = 0xffffffff;
    pcVar11 = param_1;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      pdVar10 = &pcVar11->mbr_0x0;
      pcVar11 = (cls_0x49ead0 *)((int)&pcVar11->mbr_0x0 + 1);
    } while (*(char *)pdVar10 != '\0');
    iVar5 = ~uVar7 - (~uVar6 - 1);
  }
  else {
    iVar5 = 0x104;
  }
  _strncpy((char *)&this->mbr_0xc,(char *)param_1,iVar5 - 1);
  *(undefined *)((int)&this->mbr_0x8 + iVar5 + 3) = 0;
  this->mbr_0x0 = this->mbr_0x0 & 0xfffffffb;
  dVar2 = cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&DAT_006687f8);
  this->mbr_0x8 = dVar2;
  return 1;
}



// Function at 0049eff0

void __thiscall OOAnalyzer::cls_0x49ead0::meth_0x49eff0(cls_0x49ead0 *this)

{
  cls_0x41c7f0 *this_00;
  int iVar1;
  dword dVar2;
  dword *pdVar3;
  uint uVar4;
  int iVar5;
  undefined4 *puVar6;
  
  if ((this->mbr_0x190 != 0) &&
     ((((*(byte *)&this->mbr_0x0 & 2) == 0 || (this->mbr_0x4 != 0)) ||
      (meth_0x49ff40(this), this->mbr_0x4 == 0)))) {
    if (-1 < (int)this->mbr_0x8) {
      cls_0x45f7c0::meth_0x41cb80((cls_0x45f7c0 *)&DAT_006687f8);
    }
    puVar6 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
    this->mbr_0x8 = 0xffffffff;
    if (puVar6 != (undefined4 *)0x0) {
      for (uVar4 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar4 != 0; uVar4 = uVar4 - 1) {
        *puVar6 = 0;
        puVar6 = puVar6 + 1;
      }
      for (iVar5 = 0; iVar5 != 0; iVar5 = iVar5 + -1) {
        *(undefined *)puVar6 = 0;
        puVar6 = (undefined4 *)((int)puVar6 + 1);
      }
    }
    pdVar3 = &(this->cls_0x41c7f0).mbr_0x14;
    (this->cls_0x41c7f0).mbr_0x0 = 0;
    iVar5 = 0;
    (this->cls_0x41c7f0).mbr_0x4 = 0;
    if (0 < (int)*pdVar3) {
      do {
        if ((-1 < iVar5) && (iVar1 = *(int *)(*(int *)&this->field_0x1b8 + iVar5 * 4), iVar1 != 0))
        {
          FUN_004a1e30();
          FUN_004830f0(iVar1);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar3);
        iVar5 = iVar5 + 1;
      } while (iVar5 < (int)*pdVar3);
    }
    *pdVar3 = 0;
    *(undefined4 *)&this->field_0x1ac = 0;
    dVar2 = this->mbr_0x1f0;
    if (dVar2 != 0) {
      if (*(int *)(dVar2 + 0x18) != 0) {
        FUN_004830f0(*(int *)(dVar2 + 0x18));
      }
      FUN_004830f0(dVar2);
    }
    dVar2 = this->mbr_0x8;
    this->mbr_0x1f0 = 0;
    pdVar3 = &DAT_00668810;
    do {
      if (*pdVar3 == dVar2) {
        *pdVar3 = 0xffffffff;
      }
      pdVar3 = pdVar3 + 6;
    } while ((int)pdVar3 < 0x668b10);
    FUN_0058b4f1(this->mbr_0x190);
    puVar6 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
    this->mbr_0x190 = 0;
    if (puVar6 != (undefined4 *)0x0) {
      for (uVar4 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar4 != 0; uVar4 = uVar4 - 1) {
        *puVar6 = 0;
        puVar6 = puVar6 + 1;
      }
      for (iVar5 = 0; iVar5 != 0; iVar5 = iVar5 + -1) {
        *(undefined *)puVar6 = 0;
        puVar6 = (undefined4 *)((int)puVar6 + 1);
      }
    }
    this_00 = &this->cls_0x41c7f0;
    (this->cls_0x41c7f0).mbr_0x0 = 0;
    iVar5 = 0;
    (this->cls_0x41c7f0).mbr_0x4 = 0;
    dVar2 = this_00->mbr_0x0;
    (this->cls_0x41c7f0).mbr_0x14 = 0;
    if (0 < (int)dVar2) {
      do {
        if ((-1 < iVar5) &&
           (iVar1 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + iVar5 * 4), iVar1 != 0)) {
          FUN_004830f0(iVar1);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
        iVar5 = iVar5 + 1;
      } while (iVar5 < (int)this_00->mbr_0x0);
    }
    this_00->mbr_0x0 = 0;
    (this->cls_0x41c7f0).mbr_0x4 = 0;
    (this->cls_0x41c7f0).mbr_0x14 = 0;
    this->mbr_0x1ec = 0;
    this->mbr_0x0 = 0;
  }
  return;
}



// Function at 0049f180

void __thiscall OOAnalyzer::cls_0x49ead0::meth_0x49f180(cls_0x49ead0 *this)

{
  dword in_stack_00000004;
  
  if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
    meth_0x49eff0(this);
    this->mbr_0x4 = in_stack_00000004;
    return;
  }
  this->mbr_0x4 = in_stack_00000004;
  return;
}



// Function at 0049f1b0

int __thiscall OOAnalyzer::cls_0x49ead0::meth_0x49f1b0(cls_0x49ead0 *this)

{
  dword dVar1;
  int iVar2;
  uint uVar3;
  int in_stack_00000004;
  
  uVar3 = in_stack_00000004 + 3U & 0xfffffffc;
  if (0x4000 < (int)uVar3) {
    FUN_00481c10(s_Unable_to_allocate_zip_mem_block_005dad10,0);
  }
  if ((int)(0x4000 - this->mbr_0x1ec) < (int)uVar3) {
    dVar1 = (this->cls_0x41c7f0).mbr_0x14;
    this->mbr_0x1ec = 0;
    (this->cls_0x41c7f0).mbr_0x14 = dVar1 + 1;
  }
  if ((int)(this->cls_0x41c7f0).mbr_0x0 <= (int)(this->cls_0x41c7f0).mbr_0x14) {
    FUN_00482fb0(0x4000);
    cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->cls_0x41c7f0);
    this->mbr_0x1ec = 0;
  }
  dVar1 = this->mbr_0x1ec;
  iVar2 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + (this->cls_0x41c7f0).mbr_0x14 * 4);
  this->mbr_0x1ec = dVar1 + uVar3;
  return iVar2 + dVar1;
}



// Function at 0049f3d0

undefined4 __thiscall OOAnalyzer::cls_0x49ead0::meth_0x49f3d0(cls_0x49ead0 *this,undefined4 param_1)

{
  int iVar1;
  
  iVar1 = FUN_0058c45e(param_1);
  if (iVar1 == 0) {
    if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
      meth_0x49eff0(this);
    }
    this->mbr_0x4 = 4;
    return 0;
  }
  return 1;
}



// Function at 0049f470

undefined4 * __thiscall OOAnalyzer::cls_0x49ead0::meth_0x49f470(cls_0x49ead0 *this,char *param_1)

{
  char *pcVar1;
  char *pcVar2;
  undefined4 *puVar3;
  char cVar4;
  char cVar5;
  dword dVar6;
  undefined4 uVar7;
  int iVar8;
  char *pcVar9;
  int iVar10;
  undefined4 *puVar11;
  uint uVar12;
  uint uVar13;
  char *pcVar14;
  undefined4 *puVar15;
  undefined4 *puVar16;
  undefined4 *in_stack_00000008;
  int local_5c;
  char *local_58;
  int local_48;
  undefined local_44 [6];
  short local_3e;
  undefined4 local_30;
  ushort local_2c;
  short sStack_2a;
  short local_28;
  undefined2 uStack_1e;
  undefined2 local_1c;
  int local_18;
  int local_14;
  int local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059dd70;
  local_c = ExceptionList;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  uVar7 = FUN_0058e9d1(this->mbr_0x190);
  iVar8 = FUN_0058c45e(&local_48,4,1,this->mbr_0x190);
  if (iVar8 == 0) {
    if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
      meth_0x49eff0(this);
    }
    this->mbr_0x4 = 4;
  }
  else if (local_48 == 0x2014b50) {
    iVar8 = FUN_0058c45e(local_44,0x2a,1,this->mbr_0x190);
    if (iVar8 != 0) {
      if (local_3e != 0) {
        if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
          meth_0x49eff0(this);
        }
        this->mbr_0x4 = 10;
        local_4 = 0xffffffff;
        if (local_18 != 0) {
          FUN_004830f0(local_18);
        }
        goto joined_r0x0049f5c3;
      }
      if ((int)(0x4000 - this->mbr_0x1ec) < 0x14) {
        dVar6 = (this->cls_0x41c7f0).mbr_0x14;
        this->mbr_0x1ec = 0;
        (this->cls_0x41c7f0).mbr_0x14 = dVar6 + 1;
      }
      if ((int)(this->cls_0x41c7f0).mbr_0x0 <= (int)(this->cls_0x41c7f0).mbr_0x14) {
        FUN_00482fb0(0x4000);
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->cls_0x41c7f0);
        this->mbr_0x1ec = 0;
      }
      pcVar2 = param_1 + (int)in_stack_00000008;
      puVar3 = (undefined4 *)
               (this->mbr_0x1ec +
               *(int *)((this->cls_0x41c7f0).mbr_0x10 + (this->cls_0x41c7f0).mbr_0x14 * 4));
      this->mbr_0x1ec = this->mbr_0x1ec + 0x14;
      iVar8 = FUN_0058c45e(pcVar2,local_2c,1,this->mbr_0x190);
      if (iVar8 == 0) {
        meth_0x49f180(this);
        local_4 = 0xffffffff;
        if (local_18 != 0) {
          FUN_004830f0(local_18);
        }
        if (local_14 != 0) {
          FUN_004830f0(local_14);
        }
        if (local_10 == 0) {
          ExceptionList = local_c;
          return (undefined4 *)0x0;
        }
        FUN_004830f0(local_10);
        ExceptionList = local_c;
        return (undefined4 *)0x0;
      }
      param_1[(int)in_stack_00000008 + (uint)local_2c] = '\0';
      cVar5 = *pcVar2;
      pcVar9 = pcVar2;
      pcVar14 = pcVar2;
      local_58 = pcVar2;
      local_5c = (int)in_stack_00000008;
      while (cVar5 != '\0') {
        pcVar14 = pcVar14 + 1;
        if (cVar5 == '/') {
          *pcVar9 = '\\';
          local_5c = (int)pcVar14 - (int)param_1;
          local_58 = pcVar14;
        }
        pcVar1 = pcVar9 + 1;
        pcVar9 = pcVar9 + 1;
        cVar5 = *pcVar1;
      }
      if (local_5c == 0) {
        in_stack_00000008 = (undefined4 *)&UNK_00668b20.field_0x170;
        goto LAB_0049f802;
      }
      cVar5 = param_1[local_5c];
      param_1[local_5c] = '\0';
      dVar6 = (this->cls_0x41c7f0).mbr_0x14;
      if ((dVar6 == 0) || (iVar8 = FUN_0059a530(param_1,dVar6), iVar8 != 0)) {
        iVar8 = 0;
        if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
          do {
            in_stack_00000008 = *(undefined4 **)((this->cls_0x41c7f0).mbr_0x10 + iVar8 * 4);
            iVar10 = FUN_0059a530(param_1,in_stack_00000008);
            if (iVar10 == 0) {
              (this->cls_0x41c7f0).mbr_0x14 = (dword)in_stack_00000008;
              goto LAB_0049f799;
            }
            iVar8 = iVar8 + 1;
          } while (iVar8 < (int)(this->cls_0x41c7f0).mbr_0x0);
        }
LAB_0049f7a1:
        iVar8 = -1;
        pcVar9 = param_1;
        do {
          if (iVar8 == 0) break;
          iVar8 = iVar8 + -1;
          cVar4 = *pcVar9;
          pcVar9 = pcVar9 + 1;
        } while (cVar4 != '\0');
        in_stack_00000008 = (undefined4 *)meth_0x49f1b0(this);
        uVar12 = 0xffffffff;
        pcVar9 = param_1;
        do {
          pcVar14 = pcVar9;
          if (uVar12 == 0) break;
          uVar12 = uVar12 - 1;
          pcVar14 = pcVar9 + 1;
          cVar4 = *pcVar9;
          pcVar9 = pcVar14;
        } while (cVar4 != '\0');
        uVar12 = ~uVar12;
        puVar11 = (undefined4 *)(pcVar14 + -uVar12);
        puVar15 = in_stack_00000008;
        for (uVar13 = uVar12 >> 2; uVar13 != 0; uVar13 = uVar13 - 1) {
          *puVar15 = *puVar11;
          puVar11 = puVar11 + 1;
          puVar15 = puVar15 + 1;
        }
        for (uVar12 = uVar12 & 3; uVar12 != 0; uVar12 = uVar12 - 1) {
          *(undefined *)puVar15 = *(undefined *)puVar11;
          puVar11 = (undefined4 *)((int)puVar11 + 1);
          puVar15 = (undefined4 *)((int)puVar15 + 1);
        }
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->cls_0x41c7f0);
      }
      else {
        in_stack_00000008 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x14;
LAB_0049f799:
        if (in_stack_00000008 == (undefined4 *)0x0) goto LAB_0049f7a1;
      }
      param_1[local_5c] = cVar5;
LAB_0049f802:
      iVar8 = -1;
      pcVar9 = local_58;
      do {
        if (iVar8 == 0) break;
        iVar8 = iVar8 + -1;
        cVar5 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar5 != '\0');
      puVar11 = (undefined4 *)meth_0x49f1b0(this);
      uVar12 = 0xffffffff;
      do {
        pcVar9 = local_58;
        if (uVar12 == 0) break;
        uVar12 = uVar12 - 1;
        pcVar9 = local_58 + 1;
        cVar5 = *local_58;
        local_58 = pcVar9;
      } while (cVar5 != '\0');
      uVar12 = ~uVar12;
      puVar15 = (undefined4 *)(pcVar9 + -uVar12);
      puVar16 = puVar11;
      for (uVar13 = uVar12 >> 2; uVar13 != 0; uVar13 = uVar13 - 1) {
        *puVar16 = *puVar15;
        puVar15 = puVar15 + 1;
        puVar16 = puVar16 + 1;
      }
      for (uVar12 = uVar12 & 3; uVar12 != 0; uVar12 = uVar12 - 1) {
        *(undefined *)puVar16 = *(undefined *)puVar15;
        puVar15 = (undefined4 *)((int)puVar15 + 1);
        puVar16 = (undefined4 *)((int)puVar16 + 1);
      }
      if (((sStack_2a == 0) || (iVar8 = FUN_0058c3a5(this->mbr_0x190,sStack_2a,1), iVar8 == 0)) &&
         ((local_28 == 0 || (iVar8 = FUN_0058c3a5(this->mbr_0x190,local_28,1), iVar8 == 0)))) {
        local_4 = 0xffffffff;
        *puVar3 = local_30;
        puVar3[1] = CONCAT22(local_1c,uStack_1e);
        *pcVar2 = '\0';
        puVar3[2] = uVar7;
        puVar3[4] = puVar11;
        puVar3[3] = in_stack_00000008;
        FUN_004a1e30();
        ExceptionList = local_c;
        return puVar3;
      }
      meth_0x49f180(this);
      local_4 = 0xffffffff;
      FUN_004a1e30();
      ExceptionList = local_c;
      return (undefined4 *)0x0;
    }
    if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
      meth_0x49eff0(this);
    }
    this->mbr_0x4 = 4;
  }
  else if (local_48 != 0x6054b50) {
    if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
      meth_0x49eff0(this);
    }
    this->mbr_0x4 = 8;
  }
  local_4 = 0xffffffff;
  if (local_18 != 0) {
    local_4 = 0xffffffff;
    FUN_004830f0(local_18);
  }
joined_r0x0049f5c3:
  if (local_14 != 0) {
    FUN_004830f0(local_14);
  }
  if (local_10 == 0) {
    ExceptionList = local_c;
    return (undefined4 *)0x0;
  }
  FUN_004830f0(local_10);
  ExceptionList = local_c;
  return (undefined4 *)0x0;
}



// Function at 0049fb80

undefined4 __thiscall
OOAnalyzer::cls_0x49ead0::meth_0x49fb80(cls_0x49ead0 *this,int param_1,char *param_2)

{
  cls_0x41c7f0 *this_00;
  char cVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 uVar6;
  uint uVar7;
  int iVar8;
  char *pcVar9;
  undefined4 *puVar10;
  int in_stack_0000000c;
  int local_8;
  int local_4;
  
  FUN_0058e9d1(this->mbr_0x190);
  this_00 = &this->cls_0x41c7f0;
  iVar8 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      cls_0x41c7f0::meth_0x4a1e90(this_00);
      iVar8 = iVar8 + 1;
    } while (iVar8 < (int)this_00->mbr_0x0);
  }
  this_00->mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  puVar5 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
  (this->cls_0x41c7f0).mbr_0x14 = 0;
  this->mbr_0x1ec = 0;
  if (puVar5 != (undefined4 *)0x0) {
    for (uVar7 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar7 != 0; uVar7 = uVar7 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar8 = 0; iVar8 != 0; iVar8 = iVar8 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  uVar7 = 0xffffffff;
  pcVar9 = param_2;
  do {
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar9 + 1;
  } while (cVar1 != '\0');
  dVar2 = this->mbr_0x190;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  local_4 = ~uVar7 - 1;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x14 = 0;
  iVar8 = FUN_0058c45e(&local_8,4,1,dVar2);
  if (iVar8 == 0) {
    if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
      meth_0x49eff0(this);
    }
  }
  else {
    if (local_8 != 0x4034b50) {
      if (param_1 != 0) {
        return 0;
      }
      if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
        meth_0x49eff0(this);
      }
      this->mbr_0x4 = 7;
      return 0;
    }
    iVar8 = in_stack_0000000c + -0x16;
    FUN_0058c3a5(this->mbr_0x190,iVar8,0);
    iVar3 = FUN_0058c45e(&local_8,4,1,this->mbr_0x190);
    if (iVar3 != 0) {
      iVar3 = 0;
      do {
        if (local_8 == 0x6054b50) {
LAB_0049fd47:
          if (param_1 == 0) {
            FUN_0058c3a5(this->mbr_0x190,0xfffffffc,1);
            puVar5 = (undefined4 *)FUN_00482fb0(0x1c);
            if (puVar5 == (undefined4 *)0x0) {
              puVar5 = (undefined4 *)0x0;
            }
            else {
              puVar5[6] = 0;
            }
            this->mbr_0x1f0 = (dword)puVar5;
            puVar10 = puVar5;
            for (iVar8 = 7; iVar8 != 0; iVar8 = iVar8 + -1) {
              *puVar10 = 0;
              puVar10 = puVar10 + 1;
            }
            iVar8 = FUN_0058c45e(puVar5,0x16,1,this->mbr_0x190);
            if (iVar8 == 0) {
              meth_0x49f180(this);
            }
            else if (*(ushort *)(puVar5 + 5) == 0) {
              puVar5[6] = 0;
            }
            else {
              uVar6 = FUN_00482fb0(*(ushort *)(puVar5 + 5) + 1);
              puVar5[6] = uVar6;
              iVar8 = meth_0x49f3d0(this,uVar6);
              if (iVar8 != 0) {
                *(undefined *)((uint)*(ushort *)(puVar5 + 5) + puVar5[6]) = 0;
              }
            }
            if (this->mbr_0x4 != 0) {
              return 0;
            }
          }
          FUN_0058c3a5(this->mbr_0x190,*(undefined4 *)(this->mbr_0x1f0 + 0x10),0);
          iVar8 = FUN_0058c45e(&local_8,4,1,this->mbr_0x190);
          if (iVar8 == 0) {
            if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
              meth_0x49eff0(this);
            }
            this->mbr_0x4 = 4;
            return 0;
          }
          if (local_8 == 0x2014b50) {
            FUN_0058c3a5(this->mbr_0x190,0xfffffffc,1);
            iVar8 = meth_0x49f470(this,param_2);
            dVar2 = this->mbr_0x4;
            while( true ) {
              if (dVar2 != 0) {
                return 0;
              }
              if (iVar8 == 0) break;
              cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->cls_0x41c7f0);
              iVar8 = meth_0x49f470(this,param_2);
              dVar2 = this->mbr_0x4;
            }
            FUN_0058c9ff((this->cls_0x41c7f0).mbr_0x10,(this->cls_0x41c7f0).mbr_0x0,4,&LAB_0049ff00)
            ;
            return 1;
          }
LAB_0049fe5a:
          if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
            meth_0x49eff0(this);
          }
          this->mbr_0x4 = 8;
          return 0;
        }
        if (0x417 < iVar3) {
          if (local_8 == 0x6054b50) goto LAB_0049fd47;
          goto LAB_0049fe5a;
        }
        iVar3 = iVar3 + 1;
        iVar8 = iVar8 + -1;
        FUN_0058c3a5(this->mbr_0x190,iVar8,0);
        iVar4 = FUN_0058c45e(&local_8,4,1,this->mbr_0x190);
        if (iVar4 == 0) {
          if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
            meth_0x49eff0(this);
          }
          this->mbr_0x4 = 4;
          return 0;
        }
      } while( true );
    }
    if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
      meth_0x49eff0(this);
      this->mbr_0x4 = 4;
      return 0;
    }
  }
  this->mbr_0x4 = 4;
  return 0;
}



// Function at 0049ff40

undefined4 __thiscall OOAnalyzer::cls_0x49ead0::meth_0x49ff40(cls_0x49ead0 *this)

{
  short sVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  undefined4 local_28;
  undefined2 local_24;
  undefined2 local_22;
  short local_20;
  short local_1e;
  int local_1c;
  int local_18;
  uint local_14;
  int local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059dd82;
  local_c = ExceptionList;
  if (((this->mbr_0x190 != 0) && ((*(byte *)&this->mbr_0x0 & 2) != 0)) &&
     (0 < (int)(this->cls_0x41c7f0).mbr_0x0)) {
    ExceptionList = &local_c;
    FUN_0058c3a5(this->mbr_0x190,0,2);
    iVar2 = FUN_0058e9d1(this->mbr_0x190);
    local_20 = 0;
    iVar6 = 0;
    sVar1 = 0;
    if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
      do {
        iVar4 = *(int *)(*(int *)&this->field_0x1b8 + iVar6 * 4);
        local_20 = sVar1;
        if (iVar4 != 0) {
          iVar3 = FUN_0058beb8(iVar4,0x2e,1,this->mbr_0x190);
          if ((((iVar3 == 0) ||
               (iVar3 = FUN_0058beb8(*(undefined4 *)(iVar4 + 0x30),*(undefined2 *)(iVar4 + 0x1c),1,
                                     this->mbr_0x190), iVar3 == 0)) ||
              ((*(short *)(iVar4 + 0x1e) != 0 &&
               ((*(int *)(iVar4 + 0x34) != 0 &&
                (iVar3 = FUN_0058beb8(*(int *)(iVar4 + 0x34),*(short *)(iVar4 + 0x1e),1,
                                      this->mbr_0x190), iVar3 == 0)))))) ||
             ((*(short *)(iVar4 + 0x20) != 0 &&
              ((*(int *)(iVar4 + 0x38) != 0 &&
               (iVar4 = FUN_0058beb8(*(int *)(iVar4 + 0x38),*(short *)(iVar4 + 0x20),1,
                                     this->mbr_0x190), iVar4 == 0)))))) {
            if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
              meth_0x49eff0(this);
            }
            this->mbr_0x4 = 5;
            ExceptionList = local_c;
            return 0;
          }
          local_20 = sVar1 + 1;
        }
        iVar6 = iVar6 + 1;
        sVar1 = local_20;
      } while (iVar6 < (int)(this->cls_0x41c7f0).mbr_0x0);
    }
    local_10 = 0;
    local_4 = 0;
    local_28 = 0x6054b50;
    local_24 = 0;
    local_22 = 0;
    local_1e = local_20;
    iVar6 = FUN_0058e9d1(this->mbr_0x190);
    uVar5 = local_14 & 0xffff;
    local_14 = local_14 & 0xffff0000;
    local_1c = iVar6 + 0x16 + (uVar5 - iVar2);
    local_18 = iVar2;
    iVar2 = FUN_0058beb8(&local_28,0x16,1,this->mbr_0x190);
    if (iVar2 != 0) {
      local_4 = 0xffffffff;
      if (local_10 != 0) {
        FUN_004830f0(local_10);
      }
      ExceptionList = local_c;
      return 1;
    }
    if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
      meth_0x49eff0(this);
    }
    this->mbr_0x4 = 5;
    local_4 = 0xffffffff;
    if (local_10 != 0) {
      FUN_004830f0(local_10);
    }
  }
  ExceptionList = local_c;
  return 0;
}



// Function at 004a0180

int __thiscall OOAnalyzer::cls_0x49ead0::meth_0x4a0180(cls_0x49ead0 *this,cls_0x49ead0 *param_1)

{
  char *pcVar1;
  char cVar2;
  char cVar3;
  char *pcVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  cls_0x49ead0 *pcVar8;
  int in_stack_00000008;
  undefined4 local_108;
  char local_104 [260];
  
  if (in_stack_00000008 < 0) {
    in_stack_00000008 = -1;
  }
  pcVar4 = _strrchr(this,param_1,0x5c);
  pcVar8 = param_1;
  if (pcVar4 != (char *)0x0) {
    pcVar8 = (cls_0x49ead0 *)(pcVar4 + 1);
  }
  pcVar4 = (char *)((int)pcVar8 + (1 - (int)param_1));
  if (0x103 < (int)pcVar4) {
    pcVar4 = (char *)0x104;
  }
  local_108 = pcVar8;
  _strncpy(local_104,(char *)param_1,(size_t)(pcVar4 + -1));
  iVar7 = in_stack_00000008 + 1;
  local_104[(int)(pcVar4 + -1)] = '\0';
  if (iVar7 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      iVar6 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + iVar7 * 4);
      if ((iVar6 != 0) && (iVar5 = FUN_0059a530(local_104,*(undefined4 *)(iVar6 + 0xc)), iVar5 == 0)
         ) {
        pcVar4 = *(char **)(iVar6 + 0x10);
        cVar2 = *pcVar4;
        while ((cVar2 != '\0' && (cVar3 = *(char *)&pcVar8->mbr_0x0, cVar3 != '\0'))) {
          if (cVar3 == '?') {
LAB_004a0270:
            pcVar4 = pcVar4 + 1;
            pcVar8 = (cls_0x49ead0 *)((int)&pcVar8->mbr_0x0 + 1);
          }
          else {
            if (cVar3 != '*') {
              iVar6 = FUN_0058b266((int)cVar2);
              iVar5 = FUN_0058b266((int)*(char *)&pcVar8->mbr_0x0);
              if (iVar5 == iVar6) goto LAB_004a0270;
              goto LAB_004a0282;
            }
            do {
              cVar3 = *(char *)((int)&pcVar8->mbr_0x0 + 1);
              pcVar8 = (cls_0x49ead0 *)((int)&pcVar8->mbr_0x0 + 1);
            } while (cVar3 == '*');
            while ((cVar2 != '\0' && (cVar2 != cVar3))) {
              pcVar1 = pcVar4 + 1;
              pcVar4 = pcVar4 + 1;
              cVar2 = *pcVar1;
            }
          }
          cVar2 = *pcVar4;
        }
        if ((*pcVar4 == '\0') && (*(char *)&pcVar8->mbr_0x0 == '\0')) {
          return iVar7;
        }
      }
LAB_004a0282:
      iVar7 = iVar7 + 1;
      pcVar8 = local_108;
    } while (iVar7 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  return -1;
}



// Function at 004a0380

int __thiscall OOAnalyzer::cls_0x49ead0::meth_0x4a0380(cls_0x49ead0 *this,int param_1)

{
  char cVar1;
  dword dVar2;
  dword dVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  void *this_00;
  char *pcVar7;
  cls_0x49ead0 *_Str;
  dword *pdVar8;
  undefined *local_11c;
  undefined local_118 [12];
  char *local_10c;
  undefined4 local_108;
  char local_104 [260];
  
  uVar5 = 0xffffffff;
  pdVar8 = &this->mbr_0xc;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar1 = *(char *)pdVar8;
    pdVar8 = (dword *)((int)pdVar8 + 1);
  } while (cVar1 != '\0');
  iVar6 = ~uVar5 - 1;
  iVar4 = FUN_0059a600(param_1,&this->mbr_0xc,iVar6);
  if (iVar4 == 0) {
    cVar1 = *(char *)(iVar6 + param_1);
    pcVar7 = (char *)(iVar6 + param_1);
    for (pdVar8 = &this->mbr_0x110;
        ((cVar1 != '\0' && (cVar1 != '\\')) && (*(char *)pdVar8 != '\0'));
        pdVar8 = (dword *)((int)pdVar8 + 1)) {
      iVar4 = FUN_0058b266((int)*(char *)pdVar8);
      iVar6 = FUN_0058b266((int)*pcVar7);
      if (iVar6 != iVar4) {
        return -1;
      }
      cVar1 = pcVar7[1];
      pcVar7 = pcVar7 + 1;
    }
    if ((*pcVar7 == '\\') && ((*(char *)pdVar8 == '\0' || (*(char *)pdVar8 == '.')))) {
      _Str = (cls_0x49ead0 *)(pcVar7 + 1);
      iVar4 = FUN_0058ade0(_Str,0x3f);
      local_11c = (undefined *)(uint)(iVar4 != 0);
      if (local_11c == (undefined *)0x0) {
        iVar4 = FUN_0058ade0(_Str,0x2a);
        local_11c = (undefined *)(uint)(iVar4 != 0);
        if (local_11c == (undefined *)0x0) {
          pcVar7 = _strrchr(this_00,_Str,0x5c);
          local_108 = _Str;
          if (pcVar7 != (char *)0x0) {
            local_108 = (cls_0x49ead0 *)(pcVar7 + 1);
          }
          pcVar7 = (char *)((int)local_108 + (1 - (int)_Str));
          if (0x103 < (int)pcVar7) {
            pcVar7 = (char *)0x104;
          }
          _strncpy(local_104,(char *)_Str,(size_t)(pcVar7 + -1));
          dVar2 = (this->cls_0x41c7f0).mbr_0x0;
          local_10c = local_104;
          local_11c = local_118;
          dVar3 = (this->cls_0x41c7f0).mbr_0x10;
          local_104[(int)(pcVar7 + -1)] = '\0';
          iVar4 = FUN_0058e8bb(&local_11c,dVar3,dVar2,4,&LAB_0049ff00);
          if (iVar4 == 0) {
            return -1;
          }
          return (int)(iVar4 - (this->cls_0x41c7f0).mbr_0x10) >> 2;
        }
      }
      iVar4 = meth_0x4a0180(this,_Str);
      return iVar4;
    }
  }
  return -1;
}



// Function at 004a06a0

dword * __thiscall OOAnalyzer::cls_0x49ead0::meth_0x4a06a0(cls_0x49ead0 *this)

{
  dword *pdVar1;
  dword dVar2;
  dword dVar3;
  int iVar4;
  int *piVar5;
  dword *pdVar6;
  bool bVar7;
  dword *in_stack_00000004;
  int local_34 [6];
  ushort uStack_1a;
  ushort local_18;
  int local_14;
  int local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059dd94;
  local_c = ExceptionList;
  if (this->mbr_0x190 == 0) {
    return (dword *)0x0;
  }
  if ((dword *)(this->cls_0x41c7f0).mbr_0x0 <= in_stack_00000004) {
    return (dword *)0x0;
  }
  pdVar1 = *(dword **)((this->cls_0x41c7f0).mbr_0x10 + (int)in_stack_00000004 * 4);
  if (pdVar1 == (dword *)0x0) {
    return (dword *)0x0;
  }
  local_10 = 0;
  local_14 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  FUN_0058c3a5(this->mbr_0x190,pdVar1[1],0);
  iVar4 = FUN_0058c45e(local_34,0x1e,1,this->mbr_0x190);
  if (iVar4 == 0) {
    if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
      meth_0x49eff0(this);
    }
    this->mbr_0x4 = 4;
    local_4 = 0xffffffff;
    if (local_14 != 0) {
      FUN_004830f0(local_14);
    }
  }
  else {
    if (local_34[0] == 0x4034b50) {
      iVar4 = 0;
      piVar5 = &DAT_00668814;
      do {
        if (*piVar5 < 0) {
          pdVar6 = &DAT_00668810 + iVar4 * 6;
          break;
        }
        piVar5 = piVar5 + 6;
        iVar4 = iVar4 + 1;
        pdVar6 = in_stack_00000004;
      } while ((int)piVar5 < 0x668b14);
      if (pdVar6 != (dword *)0x0) {
        *pdVar6 = this->mbr_0x8;
        dVar2 = pdVar1[1];
        dVar3 = *pdVar1;
        pdVar6[1] = (dword)in_stack_00000004;
        pdVar6[2] = (uint)local_18 + (uint)uStack_1a + 0x1e + dVar2;
        pdVar6[4] = dVar3;
        pdVar6[3] = 0;
        cls_0x56fe20::meth_0x56fe20((cls_0x56fe20 *)(pdVar6 + 5));
        FUN_0058c3a5(this->mbr_0x190,pdVar6[2],0);
        local_4 = 0xffffffff;
        if (local_14 != 0) {
          FUN_004830f0(local_14);
        }
        if (local_10 != 0) {
          FUN_004830f0(local_10);
          ExceptionList = local_c;
          return pdVar6;
        }
        ExceptionList = local_c;
        return pdVar6;
      }
      local_4 = 0xffffffff;
      if (local_14 != 0) {
        FUN_004830f0(local_14);
      }
      bVar7 = local_10 == 0;
      goto LAB_004a07e3;
    }
    if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
      meth_0x49eff0(this);
    }
    this->mbr_0x4 = 8;
    local_4 = 0xffffffff;
    if (local_14 != 0) {
      FUN_004830f0(local_14);
      bVar7 = local_10 == 0;
      goto LAB_004a07e3;
    }
  }
  bVar7 = local_10 == 0;
LAB_004a07e3:
  if (!bVar7) {
    FUN_004830f0(local_10);
  }
  ExceptionList = local_c;
  return (dword *)0x0;
}



// Function at 004a0890

dword * __thiscall OOAnalyzer::cls_0x49ead0::meth_0x4a0890(cls_0x49ead0 *this)

{
  char cVar1;
  cls_0x49ead0 *this_00;
  int iVar2;
  int *piVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  dword dVar6;
  uint uVar7;
  uint uVar8;
  char *pcVar9;
  undefined4 *puVar10;
  dword *pdVar11;
  char *pcVar12;
  undefined4 *puVar13;
  char *in_stack_00000004;
  WORD WStack_64;
  WORD WStack_62;
  dword *local_60;
  undefined4 uStack_5c;
  cls_0x49ead0 *local_58;
  int local_54;
  char *pcStack_50;
  _FILETIME _Stack_4c;
  _SYSTEMTIME local_44;
  undefined4 auStack_34 [6];
  undefined4 uStack_1a;
  int iStack_14;
  int iStack_10;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059dda6;
  pvStack_c = ExceptionList;
  if (this->mbr_0x190 != 0) {
    uVar7 = 0xffffffff;
    pdVar11 = &this->mbr_0xc;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *(char *)pdVar11;
      pdVar11 = (dword *)((int)pdVar11 + 1);
    } while (cVar1 != '\0');
    ExceptionList = &pvStack_c;
    local_58 = this;
    local_54 = ~uVar7 - 1;
    iVar2 = FUN_0059a600();
    if (iVar2 == 0) {
      iVar2 = 0;
      piVar3 = &DAT_00668814;
      do {
        if (*piVar3 < 0) {
          local_60 = &DAT_00668810 + iVar2 * 6;
          break;
        }
        piVar3 = piVar3 + 6;
        iVar2 = iVar2 + 1;
      } while ((int)piVar3 < 0x668b14);
      if (local_60 != (dword *)0x0) {
        GetSystemTime(&local_44);
        SystemTimeToFileTime(&local_44,&_Stack_4c);
        FileTimeToDosDateTime(&_Stack_4c,&WStack_62,&WStack_64);
        FUN_0058c3a5(this->mbr_0x190,0,2);
        uStack_5c = FUN_0058e9d1(this->mbr_0x190);
        puVar4 = (undefined4 *)FUN_00482fb0(0x3c);
        if (puVar4 == (undefined4 *)0x0) {
          puVar4 = (undefined4 *)0x0;
        }
        else {
          puVar4[0xe] = 0;
          puVar4[0xd] = 0;
          puVar4[0xc] = 0;
        }
        puVar5 = puVar4;
        for (iVar2 = 0xf; iVar2 != 0; iVar2 = iVar2 + -1) {
          *puVar5 = 0;
          puVar5 = puVar5 + 1;
        }
        *puVar4 = 0x2014b50;
        *(undefined2 *)(puVar4 + 1) = 0x14;
        *(undefined2 *)((int)puVar4 + 6) = 0x14;
        *(undefined2 *)(puVar4 + 2) = 0;
        *(undefined2 *)((int)puVar4 + 10) = 0;
        *(WORD *)(puVar4 + 3) = WStack_64;
        pcVar9 = in_stack_00000004 + (~uVar7 - 1);
        *(WORD *)((int)puVar4 + 0xe) = WStack_62;
        iVar2 = -1;
        puVar4[4] = 0;
        pcVar12 = pcVar9;
        do {
          if (iVar2 == 0) break;
          iVar2 = iVar2 + -1;
          cVar1 = *pcVar12;
          pcVar12 = pcVar12 + 1;
        } while (cVar1 != '\0');
        *(ushort *)(puVar4 + 7) = ~(ushort)iVar2 - 1;
        *(short *)((int)puVar4 + 0x2a) = (short)uStack_5c;
        uVar7 = 0xffffffff;
        pcVar12 = pcVar9;
        do {
          if (uVar7 == 0) break;
          uVar7 = uVar7 - 1;
          cVar1 = *pcVar12;
          pcVar12 = pcVar12 + 1;
        } while (cVar1 != '\0');
        puVar4[5] = 0;
        puVar4[6] = 0;
        *(undefined2 *)((int)puVar4 + 0x1e) = 0;
        *(undefined2 *)(puVar4 + 8) = 0;
        *(undefined2 *)((int)puVar4 + 0x22) = 0;
        *(undefined2 *)(puVar4 + 9) = 0;
        *(undefined2 *)((int)puVar4 + 0x26) = 0;
        *(undefined2 *)(puVar4 + 10) = 0;
        *(short *)(puVar4 + 0xb) = (short)((uint)uStack_5c >> 0x10);
        pcStack_50 = pcVar9;
        puVar5 = (undefined4 *)FUN_00482ef0(~uVar7);
        this_00 = local_58;
        uVar7 = 0xffffffff;
        do {
          pcVar12 = pcVar9;
          if (uVar7 == 0) break;
          uVar7 = uVar7 - 1;
          pcVar12 = pcVar9 + 1;
          cVar1 = *pcVar9;
          pcVar9 = pcVar12;
        } while (cVar1 != '\0');
        uVar7 = ~uVar7;
        puVar10 = (undefined4 *)(pcVar12 + -uVar7);
        puVar13 = puVar5;
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
        puVar4[0xc] = puVar5;
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(local_58->cls_0x41c7f0).mbr_0x14);
        iStack_10 = 0;
        iStack_14 = 0;
        puVar4 = auStack_34;
        for (iVar2 = 10; iVar2 != 0; iVar2 = iVar2 + -1) {
          *puVar4 = 0;
          puVar4 = puVar4 + 1;
        }
        iVar2 = -1;
        do {
          if (iVar2 == 0) break;
          iVar2 = iVar2 + -1;
          cVar1 = *in_stack_00000004;
          in_stack_00000004 = in_stack_00000004 + 1;
        } while (cVar1 != '\0');
        uStack_4 = 0;
        auStack_34[0] = 0x4034b50;
        uStack_1a = CONCAT22(uStack_1a._2_2_,(~(ushort)iVar2 - 1) - (short)local_54);
        iVar2 = FUN_0058beb8(auStack_34,0x1e,1,this_00->mbr_0x190);
        if (iVar2 == 0) {
          if ((*(byte *)&this_00->mbr_0x0 & 4) != 0) {
            meth_0x49eff0(this_00);
          }
          this_00->mbr_0x4 = 5;
        }
        else {
          iVar2 = FUN_0058beb8(pcStack_50,uStack_1a & 0xffff,1,this_00->mbr_0x190);
          pdVar11 = local_60;
          if (iVar2 != 0) {
            dVar6 = this_00->mbr_0x190;
            *local_60 = this_00->mbr_0x8;
            local_60[1] = 0;
            dVar6 = FUN_0058e9d1(dVar6);
            pdVar11[2] = dVar6;
            pdVar11[3] = 0;
            pdVar11[4] = 0;
            cls_0x56fe20::meth_0x56fe20((cls_0x56fe20 *)(pdVar11 + 5));
            uStack_4 = 0xffffffff;
            if (iStack_14 != 0) {
              FUN_004830f0(iStack_14);
            }
            if (iStack_10 == 0) {
              ExceptionList = pvStack_c;
              return pdVar11;
            }
            FUN_004830f0(iStack_10);
            ExceptionList = pvStack_c;
            return pdVar11;
          }
          if ((*(byte *)&this_00->mbr_0x0 & 4) != 0) {
            meth_0x49eff0(this_00);
          }
          this_00->mbr_0x4 = 5;
        }
        uStack_4 = 0xffffffff;
        if (iStack_14 != 0) {
          uStack_4 = 0xffffffff;
          FUN_004830f0(iStack_14);
        }
        if (iStack_10 != 0) {
          FUN_004830f0(iStack_10);
        }
      }
    }
  }
  ExceptionList = pvStack_c;
  return (dword *)0x0;
}



// Function at 004a0e70

undefined4 __thiscall OOAnalyzer::cls_0x49ead0::meth_0x4a0e70(cls_0x49ead0 *this)

{
  int iVar1;
  dword *in_stack_00000004;
  undefined4 local_34;
  undefined2 local_30;
  undefined2 local_2e;
  undefined2 local_2c;
  undefined2 local_2a;
  undefined2 local_28;
  undefined2 local_26;
  undefined2 local_24;
  undefined2 local_22;
  undefined2 local_20;
  undefined2 local_1e;
  undefined2 local_1c;
  undefined2 local_1a;
  undefined2 local_18;
  int local_14;
  int local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ddb8;
  local_c = ExceptionList;
  if ((((in_stack_00000004 != (dword *)0x0) && (*in_stack_00000004 == this->mbr_0x8)) &&
      (this->mbr_0x190 != 0)) &&
     (((this->mbr_0x0 & 1) != 0 && (in_stack_00000004[1] < (this->cls_0x41c7f0).mbr_0x0)))) {
    if ((this->mbr_0x0 & 2) == 0) {
LAB_004a1032:
      *in_stack_00000004 = 0xffffffff;
      in_stack_00000004[1] = 0xffffffff;
      in_stack_00000004[2] = 0;
      in_stack_00000004[4] = 0;
      in_stack_00000004[3] = 0;
      ExceptionList = local_c;
      return 1;
    }
    iVar1 = *(int *)(*(int *)&this->field_0x1b8 + in_stack_00000004[1] * 4);
    if (iVar1 != 0) {
      ExceptionList = &local_c;
      FUN_0058c3a5(this->mbr_0x190,in_stack_00000004[2],0);
      local_10 = 0;
      local_14 = 0;
      local_34 = 0x4034b50;
      local_30 = *(undefined2 *)(iVar1 + 6);
      local_2e = *(undefined2 *)(iVar1 + 8);
      local_2c = *(undefined2 *)(iVar1 + 10);
      local_2a = *(undefined2 *)(iVar1 + 0xc);
      local_28 = *(undefined2 *)(iVar1 + 0xe);
      local_24 = (undefined2)((uint)*(undefined4 *)(iVar1 + 0x10) >> 0x10);
      local_26 = (undefined2)*(undefined4 *)(iVar1 + 0x10);
      local_4 = 0;
      local_20 = (undefined2)((uint)*(undefined4 *)(iVar1 + 0x14) >> 0x10);
      local_22 = *(undefined2 *)(iVar1 + 0x14);
      local_1c = (undefined2)((uint)*(undefined4 *)(iVar1 + 0x18) >> 0x10);
      local_1e = *(undefined2 *)(iVar1 + 0x18);
      local_1a = *(undefined2 *)(iVar1 + 0x1c);
      local_18 = 0;
      iVar1 = FUN_0058beb8(&local_34,0x1e,1,this->mbr_0x190);
      if (iVar1 != 0) {
        FUN_0058c3a5(this->mbr_0x190,0,2);
        local_4 = 0xffffffff;
        if (local_14 != 0) {
          FUN_004830f0(local_14);
        }
        if (local_10 != 0) {
          FUN_004830f0(local_10);
        }
        goto LAB_004a1032;
      }
      if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
        meth_0x49eff0(this);
      }
      this->mbr_0x4 = 5;
      local_4 = 0xffffffff;
      if (local_14 != 0) {
        FUN_004830f0(local_14);
      }
      if (local_10 != 0) {
        FUN_004830f0(local_10);
      }
    }
  }
  ExceptionList = local_c;
  return 0;
}



