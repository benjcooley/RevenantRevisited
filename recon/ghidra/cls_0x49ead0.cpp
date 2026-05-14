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
  LPCVOID pvVar1;
  int iVar2;
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
  iVar2 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar2) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar2 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this_00->mbr_0x0);
  }
  this_00->mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  pvVar1 = (LPCVOID)this->mbr_0x1f0;
  if (pvVar1 != (LPCVOID)0x0) {
    if (*(LPCVOID *)((int)pvVar1 + 0x18) != (LPCVOID)0x0) {
      FUN_004830f0(*(LPCVOID *)((int)pvVar1 + 0x18));
    }
    FUN_004830f0(pvVar1);
  }
  local_4._0_1_ = 2;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  local_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  local_4 = (uint)local_4._1_3_ << 8;
  FUN_004830f0(*(LPCVOID *)&this->field_0x1b8);
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 0049ee20

undefined4 __thiscall
OOAnalyzer::cls_0x49ead0::meth_0x49ee20(cls_0x49ead0 *this,cls_0x49ead0 *param_1)

{
  char cVar1;
  dword dVar2;
  uint uVar3;
  char *pcVar4;
  int iVar5;
  void *extraout_ECX;
  void *extraout_ECX_00;
  void *this_00;
  uint uVar6;
  undefined4 *puVar7;
  dword *pdVar8;
  dword *pdVar9;
  cls_0x49ead0 *pcVar10;
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
    puVar7 = local_104;
    for (iVar5 = 0x41; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar7 = 0;
      puVar7 = puVar7 + 1;
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
  pdVar9 = &this->mbr_0x110;
  _strncpy((char *)pdVar9,pcVar4 + 1,0x7f);
  uVar3 = 0xffffffff;
  this->mbr_0x18f = 0;
  pdVar8 = pdVar9;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *(char *)pdVar8;
    pdVar8 = (dword *)((int)pdVar8 + 1);
  } while (cVar1 != '\0');
  uVar6 = 0xffffffff;
  pcVar10 = param_1;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    pdVar8 = &pcVar10->mbr_0x0;
    pcVar10 = (cls_0x49ead0 *)((int)&pcVar10->mbr_0x0 + 1);
  } while (*(char *)pdVar8 != '\0');
  if (~uVar6 - (~uVar3 - 1) < 0x104) {
    uVar3 = 0xffffffff;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *(char *)pdVar9;
      pdVar9 = (dword *)((int)pdVar9 + 1);
    } while (cVar1 != '\0');
    uVar6 = 0xffffffff;
    pcVar10 = param_1;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      pdVar9 = &pcVar10->mbr_0x0;
      pcVar10 = (cls_0x49ead0 *)((int)&pcVar10->mbr_0x0 + 1);
    } while (*(char *)pdVar9 != '\0');
    iVar5 = ~uVar6 - (~uVar3 - 1);
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
  LPCVOID pvVar1;
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
        if ((-1 < iVar5) &&
           (pvVar1 = *(LPCVOID *)(*(int *)&this->field_0x1b8 + iVar5 * 4), pvVar1 != (LPCVOID)0x0))
        {
          FUN_004a1e30((int)pvVar1);
          FUN_004830f0(pvVar1);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar3);
        iVar5 = iVar5 + 1;
      } while (iVar5 < (int)*pdVar3);
    }
    *pdVar3 = 0;
    *(undefined4 *)&this->field_0x1ac = 0;
    pvVar1 = (LPCVOID)this->mbr_0x1f0;
    if (pvVar1 != (LPCVOID)0x0) {
      if (*(LPCVOID *)((int)pvVar1 + 0x18) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)((int)pvVar1 + 0x18));
      }
      FUN_004830f0(pvVar1);
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
    FUN_0058b4f1((FILE *)this->mbr_0x190);
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
           (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar5 * 4), pvVar1 != (LPCVOID)0x0
           )) {
          FUN_004830f0(pvVar1);
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
    FUN_00481c10((byte *)s_Unable_to_allocate_zip_mem_block_005dad10);
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

undefined4 __thiscall
OOAnalyzer::cls_0x49ead0::meth_0x49f3d0(cls_0x49ead0 *this,undefined4 *param_1)

{
  uint uVar1;
  uint in_stack_00000008;
  
  uVar1 = FUN_0058c45e(param_1,in_stack_00000008,1,(byte **)this->mbr_0x190);
  if (uVar1 == 0) {
    if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
      meth_0x49eff0(this);
    }
    this->mbr_0x4 = 4;
    return 0;
  }
  return 1;
}



// Function at 0049f470

undefined4 * __thiscall OOAnalyzer::cls_0x49ead0::meth_0x49f470(cls_0x49ead0 *this,byte *param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  byte bVar3;
  char cVar4;
  byte bVar5;
  dword dVar6;
  int iVar7;
  uint uVar8;
  undefined4 *puVar9;
  uint uVar10;
  undefined4 extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  int iVar11;
  undefined4 *puVar12;
  byte *pbVar13;
  byte *pbVar14;
  undefined4 *puVar15;
  undefined4 *in_stack_00000008;
  uint local_5c;
  undefined4 *local_58;
  int local_48;
  undefined4 local_44;
  short local_3e;
  undefined4 local_30;
  ushort local_2c;
  ushort uStack_2a;
  ushort local_28;
  undefined2 uStack_1e;
  undefined2 local_1c;
  LPCVOID local_18;
  LPCVOID local_14;
  LPCVOID local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059dd70;
  local_c = ExceptionList;
  local_10 = (LPCVOID)0x0;
  local_14 = (LPCVOID)0x0;
  local_18 = (LPCVOID)0x0;
  local_4 = 0;
  ExceptionList = &local_c;
  iVar7 = FUN_0058e9d1((char **)this->mbr_0x190);
  uVar8 = FUN_0058c45e(&local_48,4,1,(byte **)this->mbr_0x190);
  if (uVar8 == 0) {
    if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
      meth_0x49eff0(this);
    }
    this->mbr_0x4 = 4;
  }
  else if (local_48 == 0x2014b50) {
    uVar8 = FUN_0058c45e(&local_44,0x2a,1,(byte **)this->mbr_0x190);
    if (uVar8 != 0) {
      if (local_3e != 0) {
        if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
          meth_0x49eff0(this);
        }
        this->mbr_0x4 = 10;
        local_4 = 0xffffffff;
        if (local_18 != (LPCVOID)0x0) {
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
      puVar1 = (undefined4 *)(param_1 + (int)in_stack_00000008);
      puVar2 = (undefined4 *)
               (this->mbr_0x1ec +
               *(int *)((this->cls_0x41c7f0).mbr_0x10 + (this->cls_0x41c7f0).mbr_0x14 * 4));
      this->mbr_0x1ec = this->mbr_0x1ec + 0x14;
      uVar8 = FUN_0058c45e(puVar1,(uint)local_2c,1,(byte **)this->mbr_0x190);
      if (uVar8 == 0) {
        meth_0x49f180(this);
        local_4 = 0xffffffff;
        if (local_18 != (LPCVOID)0x0) {
          FUN_004830f0(local_18);
        }
        if (local_14 != (LPCVOID)0x0) {
          FUN_004830f0(local_14);
        }
        if (local_10 == (LPCVOID)0x0) {
          ExceptionList = local_c;
          return (undefined4 *)0x0;
        }
        FUN_004830f0(local_10);
        ExceptionList = local_c;
        return (undefined4 *)0x0;
      }
      param_1[(int)in_stack_00000008 + (uint)local_2c] = 0;
      cVar4 = *(char *)puVar1;
      uVar8 = CONCAT31((int3)((uint)extraout_ECX >> 8),cVar4);
      puVar9 = puVar1;
      puVar12 = puVar1;
      local_58 = puVar1;
      local_5c = (uint)in_stack_00000008;
      while (cVar4 != '\0') {
        puVar12 = (undefined4 *)((int)puVar12 + 1);
        if ((char)uVar8 == '/') {
          *(undefined *)puVar9 = 0x5c;
          uVar8 = (int)puVar12 - (int)param_1;
          local_5c = uVar8;
          local_58 = puVar12;
        }
        cVar4 = *(char *)((int)puVar9 + 1);
        uVar8 = CONCAT31((int3)(uVar8 >> 8),cVar4);
        puVar9 = (undefined4 *)((int)puVar9 + 1);
      }
      if (local_5c == 0) {
        in_stack_00000008 = (undefined4 *)&UNK_00668b20.field_0x170;
        goto LAB_0049f802;
      }
      bVar5 = param_1[local_5c];
      param_1[local_5c] = 0;
      pbVar13 = (byte *)(this->cls_0x41c7f0).mbr_0x14;
      if ((pbVar13 == (byte *)0x0) ||
         (uVar10 = FUN_0059a530_stricmp(uVar8,param_1,pbVar13), uVar8 = extraout_ECX_00, uVar10 != 0)) {
        iVar11 = 0;
        if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
          do {
            in_stack_00000008 = *(undefined4 **)((this->cls_0x41c7f0).mbr_0x10 + iVar11 * 4);
            uVar8 = FUN_0059a530_stricmp(uVar8,param_1,(byte *)in_stack_00000008);
            if (uVar8 == 0) {
              (this->cls_0x41c7f0).mbr_0x14 = (dword)in_stack_00000008;
              goto LAB_0049f799;
            }
            iVar11 = iVar11 + 1;
            uVar8 = extraout_ECX_01;
          } while (iVar11 < (int)(this->cls_0x41c7f0).mbr_0x0);
        }
LAB_0049f7a1:
        iVar11 = -1;
        pbVar13 = param_1;
        do {
          if (iVar11 == 0) break;
          iVar11 = iVar11 + -1;
          bVar3 = *pbVar13;
          pbVar13 = pbVar13 + 1;
        } while (bVar3 != 0);
        in_stack_00000008 = (undefined4 *)meth_0x49f1b0(this);
        uVar8 = 0xffffffff;
        pbVar13 = param_1;
        do {
          pbVar14 = pbVar13;
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          pbVar14 = pbVar13 + 1;
          bVar3 = *pbVar13;
          pbVar13 = pbVar14;
        } while (bVar3 != 0);
        uVar8 = ~uVar8;
        puVar9 = (undefined4 *)(pbVar14 + -uVar8);
        puVar12 = in_stack_00000008;
        for (uVar10 = uVar8 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
          *puVar12 = *puVar9;
          puVar9 = puVar9 + 1;
          puVar12 = puVar12 + 1;
        }
        for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
          *(undefined *)puVar12 = *(undefined *)puVar9;
          puVar9 = (undefined4 *)((int)puVar9 + 1);
          puVar12 = (undefined4 *)((int)puVar12 + 1);
        }
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->cls_0x41c7f0);
      }
      else {
        in_stack_00000008 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x14;
LAB_0049f799:
        if (in_stack_00000008 == (undefined4 *)0x0) goto LAB_0049f7a1;
      }
      param_1[local_5c] = bVar5;
LAB_0049f802:
      iVar11 = -1;
      puVar9 = local_58;
      do {
        if (iVar11 == 0) break;
        iVar11 = iVar11 + -1;
        cVar4 = *(char *)puVar9;
        puVar9 = (undefined4 *)((int)puVar9 + 1);
      } while (cVar4 != '\0');
      puVar9 = (undefined4 *)meth_0x49f1b0(this);
      uVar8 = 0xffffffff;
      do {
        puVar12 = local_58;
        if (uVar8 == 0) break;
        uVar8 = uVar8 - 1;
        puVar12 = (undefined4 *)((int)local_58 + 1);
        cVar4 = *(char *)local_58;
        local_58 = puVar12;
      } while (cVar4 != '\0');
      uVar8 = ~uVar8;
      puVar12 = (undefined4 *)((int)puVar12 - uVar8);
      puVar15 = puVar9;
      for (uVar10 = uVar8 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
        *puVar15 = *puVar12;
        puVar12 = puVar12 + 1;
        puVar15 = puVar15 + 1;
      }
      for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
        *(undefined *)puVar15 = *(undefined *)puVar12;
        puVar12 = (undefined4 *)((int)puVar12 + 1);
        puVar15 = (undefined4 *)((int)puVar15 + 1);
      }
      if (((uStack_2a == 0) ||
          (iVar11 = FUN_0058c3a5((char **)this->mbr_0x190,(uint)uStack_2a,1), iVar11 == 0)) &&
         ((local_28 == 0 ||
          (iVar11 = FUN_0058c3a5((char **)this->mbr_0x190,(uint)local_28,1), iVar11 == 0)))) {
        local_4 = 0xffffffff;
        *puVar2 = local_30;
        puVar2[1] = CONCAT22(local_1c,uStack_1e);
        *(undefined *)puVar1 = 0;
        puVar2[2] = iVar7;
        puVar2[4] = puVar9;
        puVar2[3] = in_stack_00000008;
        FUN_004a1e30((int)&local_48);
        ExceptionList = local_c;
        return puVar2;
      }
      meth_0x49f180(this);
      local_4 = 0xffffffff;
      FUN_004a1e30((int)&local_48);
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
  if (local_18 != (LPCVOID)0x0) {
    local_4 = 0xffffffff;
    FUN_004830f0(local_18);
  }
joined_r0x0049f5c3:
  if (local_14 != (LPCVOID)0x0) {
    FUN_004830f0(local_14);
  }
  if (local_10 == (LPCVOID)0x0) {
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
  byte **ppbVar2;
  dword dVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  char *pcVar9;
  int in_stack_0000000c;
  int local_8;
  int local_4;
  
  FUN_0058e9d1((char **)this->mbr_0x190);
  this_00 = &this->cls_0x41c7f0;
  iVar7 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      cls_0x41c7f0::meth_0x4a1e90(this_00);
      iVar7 = iVar7 + 1;
    } while (iVar7 < (int)this_00->mbr_0x0);
  }
  this_00->mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  puVar4 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
  (this->cls_0x41c7f0).mbr_0x14 = 0;
  this->mbr_0x1ec = 0;
  if (puVar4 != (undefined4 *)0x0) {
    for (uVar6 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar6 != 0; uVar6 = uVar6 - 1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    for (iVar7 = 0; iVar7 != 0; iVar7 = iVar7 + -1) {
      *(undefined *)puVar4 = 0;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
  }
  uVar6 = 0xffffffff;
  pcVar9 = param_2;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar9 + 1;
  } while (cVar1 != '\0');
  ppbVar2 = (byte **)this->mbr_0x190;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  local_4 = ~uVar6 - 1;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x14 = 0;
  uVar6 = FUN_0058c45e(&local_8,4,1,ppbVar2);
  if (uVar6 == 0) {
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
    iVar7 = in_stack_0000000c + -0x16;
    FUN_0058c3a5((char **)this->mbr_0x190,iVar7,0);
    uVar6 = FUN_0058c45e(&local_8,4,1,(byte **)this->mbr_0x190);
    if (uVar6 != 0) {
      iVar8 = 0;
      do {
        if (local_8 == 0x6054b50) {
LAB_0049fd47:
          if (param_1 == 0) {
            FUN_0058c3a5((char **)this->mbr_0x190,-4,1);
            puVar4 = FUN_00482fb0(0x1c);
            if (puVar4 == (undefined4 *)0x0) {
              puVar4 = (undefined4 *)0x0;
            }
            else {
              puVar4[6] = 0;
            }
            this->mbr_0x1f0 = (dword)puVar4;
            puVar5 = puVar4;
            for (iVar7 = 7; iVar7 != 0; iVar7 = iVar7 + -1) {
              *puVar5 = 0;
              puVar5 = puVar5 + 1;
            }
            uVar6 = FUN_0058c45e(puVar4,0x16,1,(byte **)this->mbr_0x190);
            if (uVar6 == 0) {
              meth_0x49f180(this);
            }
            else if (*(ushort *)(puVar4 + 5) == 0) {
              puVar4[6] = 0;
            }
            else {
              puVar5 = FUN_00482fb0(*(ushort *)(puVar4 + 5) + 1);
              puVar4[6] = puVar5;
              iVar7 = meth_0x49f3d0(this,puVar5);
              if (iVar7 != 0) {
                *(undefined *)((uint)*(ushort *)(puVar4 + 5) + puVar4[6]) = 0;
              }
            }
            if (this->mbr_0x4 != 0) {
              return 0;
            }
          }
          FUN_0058c3a5((char **)this->mbr_0x190,*(int *)(this->mbr_0x1f0 + 0x10),0);
          uVar6 = FUN_0058c45e(&local_8,4,1,(byte **)this->mbr_0x190);
          if (uVar6 == 0) {
            if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
              meth_0x49eff0(this);
            }
            this->mbr_0x4 = 4;
            return 0;
          }
          if (local_8 == 0x2014b50) {
            FUN_0058c3a5((char **)this->mbr_0x190,-4,1);
            iVar7 = meth_0x49f470(this,param_2);
            dVar3 = this->mbr_0x4;
            while( true ) {
              if (dVar3 != 0) {
                return 0;
              }
              if (iVar7 == 0) break;
              cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->cls_0x41c7f0);
              iVar7 = meth_0x49f470(this,param_2);
              dVar3 = this->mbr_0x4;
            }
            FUN_0058c9ff((int *)(this->cls_0x41c7f0).mbr_0x10,
                         (undefined4 *)(this->cls_0x41c7f0).mbr_0x0,4,&LAB_0049ff00);
            return 1;
          }
LAB_0049fe5a:
          if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
            meth_0x49eff0(this);
          }
          this->mbr_0x4 = 8;
          return 0;
        }
        if (0x417 < iVar8) {
          if (local_8 == 0x6054b50) goto LAB_0049fd47;
          goto LAB_0049fe5a;
        }
        iVar8 = iVar8 + 1;
        iVar7 = iVar7 + -1;
        FUN_0058c3a5((char **)this->mbr_0x190,iVar7,0);
        uVar6 = FUN_0058c45e(&local_8,4,1,(byte **)this->mbr_0x190);
        if (uVar6 == 0) {
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
  undefined4 *puVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  undefined4 local_28;
  undefined2 local_24;
  undefined2 local_22;
  short local_20;
  short local_1e;
  int local_1c;
  int local_18;
  uint local_14;
  LPCVOID local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059dd82;
  local_c = ExceptionList;
  if ((((char **)this->mbr_0x190 != (char **)0x0) && ((*(byte *)&this->mbr_0x0 & 2) != 0)) &&
     (0 < (int)(this->cls_0x41c7f0).mbr_0x0)) {
    ExceptionList = &local_c;
    FUN_0058c3a5((char **)this->mbr_0x190,0,2);
    iVar3 = FUN_0058e9d1((char **)this->mbr_0x190);
    local_20 = 0;
    iVar5 = 0;
    sVar2 = 0;
    if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
      do {
        puVar1 = *(undefined4 **)(*(int *)&this->field_0x1b8 + iVar5 * 4);
        local_20 = sVar2;
        if (puVar1 != (undefined4 *)0x0) {
          uVar4 = FUN_0058beb8(puVar1,0x2e,1,(char **)this->mbr_0x190);
          if ((((uVar4 == 0) ||
               (uVar4 = FUN_0058beb8((undefined4 *)puVar1[0xc],(uint)*(ushort *)(puVar1 + 7),1,
                                     (char **)this->mbr_0x190), uVar4 == 0)) ||
              ((*(ushort *)((int)puVar1 + 0x1e) != 0 &&
               (((undefined4 *)puVar1[0xd] != (undefined4 *)0x0 &&
                (uVar4 = FUN_0058beb8((undefined4 *)puVar1[0xd],
                                      (uint)*(ushort *)((int)puVar1 + 0x1e),1,
                                      (char **)this->mbr_0x190), uVar4 == 0)))))) ||
             ((*(ushort *)(puVar1 + 8) != 0 &&
              (((undefined4 *)puVar1[0xe] != (undefined4 *)0x0 &&
               (uVar4 = FUN_0058beb8((undefined4 *)puVar1[0xe],(uint)*(ushort *)(puVar1 + 8),1,
                                     (char **)this->mbr_0x190), uVar4 == 0)))))) {
            if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
              meth_0x49eff0(this);
            }
            this->mbr_0x4 = 5;
            ExceptionList = local_c;
            return 0;
          }
          local_20 = sVar2 + 1;
        }
        iVar5 = iVar5 + 1;
        sVar2 = local_20;
      } while (iVar5 < (int)(this->cls_0x41c7f0).mbr_0x0);
    }
    local_10 = (LPCVOID)0x0;
    local_4 = 0;
    local_28 = 0x6054b50;
    local_24 = 0;
    local_22 = 0;
    local_1e = local_20;
    iVar5 = FUN_0058e9d1((char **)this->mbr_0x190);
    uVar4 = local_14 & 0xffff;
    local_14 = local_14 & 0xffff0000;
    local_1c = iVar5 + 0x16 + (uVar4 - iVar3);
    local_18 = iVar3;
    uVar4 = FUN_0058beb8(&local_28,0x16,1,(char **)this->mbr_0x190);
    if (uVar4 != 0) {
      local_4 = 0xffffffff;
      if (local_10 != (LPCVOID)0x0) {
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
    if (local_10 != (LPCVOID)0x0) {
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
  int iVar4;
  char *pcVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  cls_0x49ead0 *pcVar9;
  int in_stack_00000008;
  undefined4 local_108;
  char local_104 [260];
  
  if (in_stack_00000008 < 0) {
    in_stack_00000008 = -1;
  }
  pcVar5 = _strrchr(this,param_1,0x5c);
  pcVar9 = param_1;
  if (pcVar5 != (char *)0x0) {
    pcVar9 = (cls_0x49ead0 *)(pcVar5 + 1);
  }
  pcVar5 = (char *)((int)pcVar9 + (1 - (int)param_1));
  if (0x103 < (int)pcVar5) {
    pcVar5 = (char *)0x104;
  }
  local_108 = pcVar9;
  _strncpy(local_104,(char *)param_1,(size_t)(pcVar5 + -1));
  iVar8 = in_stack_00000008 + 1;
  local_104[(int)(pcVar5 + -1)] = '\0';
  if (iVar8 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      iVar4 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + iVar8 * 4);
      if ((iVar4 != 0) &&
         (uVar6 = FUN_0059a530_stricmp((uint)local_104,(byte *)local_104,*(byte **)(iVar4 + 0xc)),
         uVar6 == 0)) {
        pcVar5 = *(char **)(iVar4 + 0x10);
        cVar2 = *pcVar5;
        while ((cVar2 != '\0' && (cVar3 = *(char *)&pcVar9->mbr_0x0, cVar3 != '\0'))) {
          if (cVar3 == '?') {
LAB_004a0270:
            pcVar5 = pcVar5 + 1;
            pcVar9 = (cls_0x49ead0 *)((int)&pcVar9->mbr_0x0 + 1);
          }
          else {
            if (cVar3 != '*') {
              uVar6 = FUN_0058b266((int)cVar2);
              uVar7 = FUN_0058b266((int)*(char *)&pcVar9->mbr_0x0);
              if (uVar7 == uVar6) goto LAB_004a0270;
              goto LAB_004a0282;
            }
            do {
              cVar3 = *(char *)((int)&pcVar9->mbr_0x0 + 1);
              pcVar9 = (cls_0x49ead0 *)((int)&pcVar9->mbr_0x0 + 1);
            } while (cVar3 == '*');
            while ((cVar2 != '\0' && (cVar2 != cVar3))) {
              pcVar1 = pcVar5 + 1;
              pcVar5 = pcVar5 + 1;
              cVar2 = *pcVar1;
            }
          }
          cVar2 = *pcVar5;
        }
        if ((*pcVar5 == '\0') && (*(char *)&pcVar9->mbr_0x0 == '\0')) {
          return iVar8;
        }
      }
LAB_004a0282:
      iVar8 = iVar8 + 1;
      pcVar9 = local_108;
    } while (iVar8 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  return -1;
}



// Function at 004a0380

int __thiscall OOAnalyzer::cls_0x49ead0::meth_0x4a0380(cls_0x49ead0 *this,byte *param_1)

{
  char cVar1;
  byte bVar2;
  uint *puVar3;
  int iVar4;
  char *pcVar5;
  uint uVar6;
  uint uVar7;
  void *this_00;
  byte *pbVar8;
  cls_0x49ead0 *_Str;
  dword *pdVar9;
  undefined *local_11c;
  undefined local_118 [12];
  char *local_10c;
  undefined4 local_108;
  char local_104 [260];
  
  uVar6 = 0xffffffff;
  pdVar9 = &this->mbr_0xc;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    cVar1 = *(char *)pdVar9;
    pdVar9 = (dword *)((int)pdVar9 + 1);
  } while (cVar1 != '\0');
  uVar7 = ~uVar6 - 1;
  uVar6 = FUN_0059a600(param_1,(char *)&this->mbr_0xc,uVar7);
  if (uVar6 == 0) {
    bVar2 = param_1[uVar7];
    pbVar8 = param_1 + uVar7;
    for (pdVar9 = &this->mbr_0x110; ((bVar2 != 0 && (bVar2 != 0x5c)) && (*(char *)pdVar9 != '\0'));
        pdVar9 = (dword *)((int)pdVar9 + 1)) {
      uVar6 = FUN_0058b266((int)*(char *)pdVar9);
      uVar7 = FUN_0058b266((int)(char)*pbVar8);
      if (uVar7 != uVar6) {
        return -1;
      }
      bVar2 = pbVar8[1];
      pbVar8 = pbVar8 + 1;
    }
    if ((*pbVar8 == 0x5c) && ((*(char *)pdVar9 == '\0' || (*(char *)pdVar9 == '.')))) {
      _Str = (cls_0x49ead0 *)(pbVar8 + 1);
      puVar3 = FUN_0058ade0((uint *)_Str,'?');
      local_11c = (undefined *)(uint)(puVar3 != (uint *)0x0);
      if (local_11c == (undefined *)0x0) {
        puVar3 = FUN_0058ade0((uint *)_Str,'*');
        local_11c = (undefined *)(uint)(puVar3 != (uint *)0x0);
        if (local_11c == (undefined *)0x0) {
          pcVar5 = _strrchr(this_00,_Str,0x5c);
          local_108 = _Str;
          if (pcVar5 != (char *)0x0) {
            local_108 = (cls_0x49ead0 *)(pcVar5 + 1);
          }
          pcVar5 = (char *)((int)local_108 + (1 - (int)_Str));
          if (0x103 < (int)pcVar5) {
            pcVar5 = (char *)0x104;
          }
          _strncpy(local_104,(char *)_Str,(size_t)(pcVar5 + -1));
          uVar6 = (this->cls_0x41c7f0).mbr_0x0;
          local_10c = local_104;
          local_11c = local_118;
          uVar7 = (this->cls_0x41c7f0).mbr_0x10;
          local_104[(int)(pcVar5 + -1)] = '\0';
          uVar6 = FUN_0058e8bb(&local_11c,uVar7,uVar6,4,&LAB_0049ff00);
          if (uVar6 == 0) {
            return -1;
          }
          return (int)(uVar6 - (this->cls_0x41c7f0).mbr_0x10) >> 2;
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
  uint uVar4;
  int *piVar5;
  int iVar6;
  dword *pdVar7;
  bool bVar8;
  dword *in_stack_00000004;
  int local_34 [6];
  ushort uStack_1a;
  ushort local_18;
  LPCVOID local_14;
  LPCVOID local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059dd94;
  local_c = ExceptionList;
  if ((char **)this->mbr_0x190 == (char **)0x0) {
    return (dword *)0x0;
  }
  if ((dword *)(this->cls_0x41c7f0).mbr_0x0 <= in_stack_00000004) {
    return (dword *)0x0;
  }
  pdVar1 = *(dword **)((this->cls_0x41c7f0).mbr_0x10 + (int)in_stack_00000004 * 4);
  if (pdVar1 == (dword *)0x0) {
    return (dword *)0x0;
  }
  local_10 = (LPCVOID)0x0;
  local_14 = (LPCVOID)0x0;
  local_4 = 0;
  ExceptionList = &local_c;
  FUN_0058c3a5((char **)this->mbr_0x190,pdVar1[1],0);
  uVar4 = FUN_0058c45e(local_34,0x1e,1,(byte **)this->mbr_0x190);
  if (uVar4 == 0) {
    if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
      meth_0x49eff0(this);
    }
    this->mbr_0x4 = 4;
    local_4 = 0xffffffff;
    if (local_14 != (LPCVOID)0x0) {
      FUN_004830f0(local_14);
    }
  }
  else {
    if (local_34[0] == 0x4034b50) {
      iVar6 = 0;
      piVar5 = &DAT_00668814;
      do {
        if (*piVar5 < 0) {
          pdVar7 = &DAT_00668810 + iVar6 * 6;
          break;
        }
        piVar5 = piVar5 + 6;
        iVar6 = iVar6 + 1;
        pdVar7 = in_stack_00000004;
      } while ((int)piVar5 < 0x668b14);
      if (pdVar7 != (dword *)0x0) {
        *pdVar7 = this->mbr_0x8;
        dVar2 = pdVar1[1];
        dVar3 = *pdVar1;
        pdVar7[1] = (dword)in_stack_00000004;
        pdVar7[2] = (uint)local_18 + (uint)uStack_1a + 0x1e + dVar2;
        pdVar7[4] = dVar3;
        pdVar7[3] = 0;
        cls_0x56fe20::meth_0x56fe20((cls_0x56fe20 *)(pdVar7 + 5));
        FUN_0058c3a5((char **)this->mbr_0x190,pdVar7[2],0);
        local_4 = 0xffffffff;
        if (local_14 != (LPCVOID)0x0) {
          FUN_004830f0(local_14);
        }
        if (local_10 != (LPCVOID)0x0) {
          FUN_004830f0(local_10);
          ExceptionList = local_c;
          return pdVar7;
        }
        ExceptionList = local_c;
        return pdVar7;
      }
      local_4 = 0xffffffff;
      if (local_14 != (LPCVOID)0x0) {
        FUN_004830f0(local_14);
      }
      bVar8 = local_10 == (LPCVOID)0x0;
      goto LAB_004a07e3;
    }
    if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
      meth_0x49eff0(this);
    }
    this->mbr_0x4 = 8;
    local_4 = 0xffffffff;
    if (local_14 != (LPCVOID)0x0) {
      FUN_004830f0(local_14);
      bVar8 = local_10 == (LPCVOID)0x0;
      goto LAB_004a07e3;
    }
  }
  bVar8 = local_10 == (LPCVOID)0x0;
LAB_004a07e3:
  if (!bVar8) {
    FUN_004830f0(local_10);
  }
  ExceptionList = local_c;
  return (dword *)0x0;
}



// Function at 004a0890

dword * __thiscall OOAnalyzer::cls_0x49ead0::meth_0x4a0890(cls_0x49ead0 *this)

{
  char cVar1;
  byte bVar2;
  char **ppcVar3;
  cls_0x49ead0 *this_00;
  int *piVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  dword dVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  undefined4 *puVar11;
  dword *pdVar12;
  undefined4 *puVar13;
  byte *in_stack_00000004;
  WORD WStack_64;
  WORD WStack_62;
  dword *local_60;
  int iStack_5c;
  cls_0x49ead0 *local_58;
  uint local_54;
  undefined4 *puStack_50;
  _FILETIME _Stack_4c;
  _SYSTEMTIME local_44;
  undefined4 auStack_34 [6];
  undefined4 uStack_1a;
  LPCVOID pvStack_14;
  LPCVOID pvStack_10;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059dda6;
  pvStack_c = ExceptionList;
  if (this->mbr_0x190 != 0) {
    uVar8 = 0xffffffff;
    pdVar12 = &this->mbr_0xc;
    do {
      if (uVar8 == 0) break;
      uVar8 = uVar8 - 1;
      cVar1 = *(char *)pdVar12;
      pdVar12 = (dword *)((int)pdVar12 + 1);
    } while (cVar1 != '\0');
    uVar9 = ~uVar8 - 1;
    ExceptionList = &pvStack_c;
    local_58 = this;
    local_54 = uVar9;
    uVar8 = FUN_0059a600(in_stack_00000004,(char *)&this->mbr_0xc,uVar9);
    if (uVar8 == 0) {
      iVar10 = 0;
      piVar4 = &DAT_00668814;
      do {
        if (*piVar4 < 0) {
          local_60 = &DAT_00668810 + iVar10 * 6;
          break;
        }
        piVar4 = piVar4 + 6;
        iVar10 = iVar10 + 1;
      } while ((int)piVar4 < 0x668b14);
      if (local_60 != (dword *)0x0) {
        GetSystemTime(&local_44);
        SystemTimeToFileTime(&local_44,&_Stack_4c);
        FileTimeToDosDateTime(&_Stack_4c,&WStack_62,&WStack_64);
        FUN_0058c3a5((char **)this->mbr_0x190,0,2);
        iStack_5c = FUN_0058e9d1((char **)this->mbr_0x190);
        puVar5 = FUN_00482fb0(0x3c);
        if (puVar5 == (undefined4 *)0x0) {
          puVar5 = (undefined4 *)0x0;
        }
        else {
          puVar5[0xe] = 0;
          puVar5[0xd] = 0;
          puVar5[0xc] = 0;
        }
        puVar11 = puVar5;
        for (iVar10 = 0xf; iVar10 != 0; iVar10 = iVar10 + -1) {
          *puVar11 = 0;
          puVar11 = puVar11 + 1;
        }
        *puVar5 = 0x2014b50;
        *(undefined2 *)(puVar5 + 1) = 0x14;
        *(undefined2 *)((int)puVar5 + 6) = 0x14;
        *(undefined2 *)(puVar5 + 2) = 0;
        *(undefined2 *)((int)puVar5 + 10) = 0;
        *(WORD *)(puVar5 + 3) = WStack_64;
        puVar11 = (undefined4 *)(in_stack_00000004 + uVar9);
        *(WORD *)((int)puVar5 + 0xe) = WStack_62;
        iVar10 = -1;
        puVar5[4] = 0;
        puVar6 = puVar11;
        do {
          if (iVar10 == 0) break;
          iVar10 = iVar10 + -1;
          bVar2 = *(byte *)puVar6;
          puVar6 = (undefined4 *)((int)puVar6 + 1);
        } while (bVar2 != 0);
        *(ushort *)(puVar5 + 7) = ~(ushort)iVar10 - 1;
        *(short *)((int)puVar5 + 0x2a) = (short)iStack_5c;
        uVar8 = 0xffffffff;
        puVar6 = puVar11;
        do {
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          bVar2 = *(byte *)puVar6;
          puVar6 = (undefined4 *)((int)puVar6 + 1);
        } while (bVar2 != 0);
        puVar5[5] = 0;
        puVar5[6] = 0;
        *(undefined2 *)((int)puVar5 + 0x1e) = 0;
        *(undefined2 *)(puVar5 + 8) = 0;
        *(undefined2 *)((int)puVar5 + 0x22) = 0;
        *(undefined2 *)(puVar5 + 9) = 0;
        *(undefined2 *)((int)puVar5 + 0x26) = 0;
        *(undefined2 *)(puVar5 + 10) = 0;
        *(short *)(puVar5 + 0xb) = (short)((uint)iStack_5c >> 0x10);
        puStack_50 = puVar11;
        puVar6 = FUN_00482ef0(~uVar8);
        this_00 = local_58;
        uVar8 = 0xffffffff;
        do {
          puVar13 = puVar11;
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          puVar13 = (undefined4 *)((int)puVar11 + 1);
          bVar2 = *(byte *)puVar11;
          puVar11 = puVar13;
        } while (bVar2 != 0);
        uVar8 = ~uVar8;
        puVar11 = (undefined4 *)((int)puVar13 - uVar8);
        puVar13 = puVar6;
        for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
          *puVar13 = *puVar11;
          puVar11 = puVar11 + 1;
          puVar13 = puVar13 + 1;
        }
        for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
          *(undefined *)puVar13 = *(undefined *)puVar11;
          puVar11 = (undefined4 *)((int)puVar11 + 1);
          puVar13 = (undefined4 *)((int)puVar13 + 1);
        }
        puVar5[0xc] = puVar6;
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(local_58->cls_0x41c7f0).mbr_0x14);
        pvStack_10 = (LPCVOID)0x0;
        pvStack_14 = (LPCVOID)0x0;
        puVar5 = auStack_34;
        for (iVar10 = 10; iVar10 != 0; iVar10 = iVar10 + -1) {
          *puVar5 = 0;
          puVar5 = puVar5 + 1;
        }
        iVar10 = -1;
        do {
          if (iVar10 == 0) break;
          iVar10 = iVar10 + -1;
          bVar2 = *in_stack_00000004;
          in_stack_00000004 = in_stack_00000004 + 1;
        } while (bVar2 != 0);
        uStack_4 = 0;
        auStack_34[0] = 0x4034b50;
        uStack_1a = CONCAT22(uStack_1a._2_2_,(~(ushort)iVar10 - 1) - (short)local_54);
        uVar8 = FUN_0058beb8(auStack_34,0x1e,1,(char **)this_00->mbr_0x190);
        if (uVar8 == 0) {
          if ((*(byte *)&this_00->mbr_0x0 & 4) != 0) {
            meth_0x49eff0(this_00);
          }
          this_00->mbr_0x4 = 5;
        }
        else {
          uVar8 = FUN_0058beb8(puStack_50,uStack_1a & 0xffff,1,(char **)this_00->mbr_0x190);
          pdVar12 = local_60;
          if (uVar8 != 0) {
            ppcVar3 = (char **)this_00->mbr_0x190;
            *local_60 = this_00->mbr_0x8;
            local_60[1] = 0;
            dVar7 = FUN_0058e9d1(ppcVar3);
            pdVar12[2] = dVar7;
            pdVar12[3] = 0;
            pdVar12[4] = 0;
            cls_0x56fe20::meth_0x56fe20((cls_0x56fe20 *)(pdVar12 + 5));
            uStack_4 = 0xffffffff;
            if (pvStack_14 != (LPCVOID)0x0) {
              FUN_004830f0(pvStack_14);
            }
            if (pvStack_10 == (LPCVOID)0x0) {
              ExceptionList = pvStack_c;
              return pdVar12;
            }
            FUN_004830f0(pvStack_10);
            ExceptionList = pvStack_c;
            return pdVar12;
          }
          if ((*(byte *)&this_00->mbr_0x0 & 4) != 0) {
            meth_0x49eff0(this_00);
          }
          this_00->mbr_0x4 = 5;
        }
        uStack_4 = 0xffffffff;
        if (pvStack_14 != (LPCVOID)0x0) {
          uStack_4 = 0xffffffff;
          FUN_004830f0(pvStack_14);
        }
        if (pvStack_10 != (LPCVOID)0x0) {
          FUN_004830f0(pvStack_10);
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
  uint uVar2;
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
  LPCVOID local_14;
  LPCVOID local_10;
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
      FUN_0058c3a5((char **)this->mbr_0x190,in_stack_00000004[2],0);
      local_10 = (LPCVOID)0x0;
      local_14 = (LPCVOID)0x0;
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
      uVar2 = FUN_0058beb8(&local_34,0x1e,1,(char **)this->mbr_0x190);
      if (uVar2 != 0) {
        FUN_0058c3a5((char **)this->mbr_0x190,0,2);
        local_4 = 0xffffffff;
        if (local_14 != (LPCVOID)0x0) {
          FUN_004830f0(local_14);
        }
        if (local_10 != (LPCVOID)0x0) {
          FUN_004830f0(local_10);
        }
        goto LAB_004a1032;
      }
      if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
        meth_0x49eff0(this);
      }
      this->mbr_0x4 = 5;
      local_4 = 0xffffffff;
      if (local_14 != (LPCVOID)0x0) {
        FUN_004830f0(local_14);
      }
      if (local_10 != (LPCVOID)0x0) {
        FUN_004830f0(local_10);
      }
    }
  }
  ExceptionList = local_c;
  return 0;
}



