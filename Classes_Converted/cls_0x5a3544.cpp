#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a3544



// Function at 00401900

void cls_0x5a3544::virt_meth_0x401900()

{
  int iVar1;
  LPCVOID pvVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = 0;
  if (0 < (int)this->mbr_0x18) {
    do {
      FUN_004830f0(*(LPCVOID *)(this->mbr_0x34 + iVar4 * 4));
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)this->mbr_0x18);
  }
  FUN_004830f0((LPCVOID)this->mbr_0x34);
  iVar4 = 0;
  if (0 < (int)this->mbr_0x38) {
    do {
      iVar3 = 0;
      iVar1 = *(int *)(this->mbr_0x48 + iVar4 * 4);
      if (0 < *(int *)(iVar1 + 0x10)) {
        do {
          FUN_004830f0(*(LPCVOID *)(*(int *)(iVar1 + 0x20) + iVar3 * 4));
          iVar3 = iVar3 + 1;
        } while (iVar3 < *(int *)(iVar1 + 0x10));
      }
      pvVar2 = *(LPCVOID *)(this->mbr_0x48 + iVar4 * 4);
      if (pvVar2 != (LPCVOID)0x0) {
        FUN_00405b30((int)pvVar2);
        FUN_004830f0(pvVar2);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)this->mbr_0x38);
  }
  virt_meth_0x40db50(this);
  return;
}



// Function at 004036a0

void cls_0x5a3544::virt_meth_0x4036a0()

{
  LPCVOID pvVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar2 = 0;
  if (0 < (int)this->mbr_0xf8) {
    iVar3 = 0;
    do {
      iVar4 = this->mbr_0x2c + iVar3;
      if (*(LPCVOID *)(iVar4 + 0xf4) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)(iVar4 + 0xf4));
      }
      iVar2 = iVar2 + 1;
      *(undefined4 *)(iVar4 + 0xf4) = 0;
      *(undefined4 *)(iVar4 + 0xf8) = 0;
      iVar3 = iVar3 + 0xfc;
    } while (iVar2 < (int)this->mbr_0xf8);
  }
  FUN_004830f0((LPCVOID)this->mbr_0x2c);
  iVar2 = 0;
  this->mbr_0x2c = 0;
  if (0 < (int)this->mbr_0x30) {
    do {
      iVar4 = 0;
      iVar3 = *(int *)(this->mbr_0x40 + iVar2 * 4);
      if (0 < *(int *)(iVar3 + 0x10)) {
        do {
          FUN_004830f0(*(LPCVOID *)(*(int *)(iVar3 + 0x20) + iVar4 * 4));
          iVar4 = iVar4 + 1;
        } while (iVar4 < *(int *)(iVar3 + 0x10));
      }
      pvVar1 = *(LPCVOID *)(this->mbr_0x40 + iVar2 * 4);
      if (pvVar1 != (LPCVOID)0x0) {
        FUN_00405b30((int)pvVar1);
        FUN_004830f0(pvVar1);
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x30);
  }
  virt_meth_0x40db50(this);
  return;
}



// Function at 00404270

void cls_0x5a3544::virt_meth_0x404270()

{
  int iVar1;
  int iVar2;
  
  if ((uint)*(ushort *)(this->mbr_0x14 + 0xc) == this->mbr_0x4) {
    iVar1 = 0;
    *(uint *)this->mbr_0x164 = *(uint *)this->mbr_0x164 & 0xfffffffe;
    if (0 < (int)this->mbr_0xf8) {
      iVar2 = 0;
      do {
        if (*(int *)(this->mbr_0x2c + 0xe4 + iVar2) != 0) {
          FUN_004026d0(this->mbr_0x2c + iVar2);
        }
        iVar1 = iVar1 + 1;
        iVar2 = iVar2 + 0xfc;
      } while (iVar1 < (int)this->mbr_0xf8);
    }
    *(uint *)this->mbr_0x164 = *(uint *)this->mbr_0x164 | 1;
  }
  return;
}



// Function at 00405770

undefined4 cls_0x5a3544::virt_meth_0x405770(byte *param_1)

{
  uint uVar1;
  undefined4 uVar2;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  uint extraout_ECX_04;
  uint extraout_ECX_05;
  uint extraout_ECX_06;
  uint extraout_ECX_07;
  byte *in_stack_00000008;
  
  uVar1 = FUN_0059a530((uint)this,param_1,&DAT_005c5c14);
  if (uVar1 == 0) {
    this->mbr_0x2c = 0;
    FUN_00479580(in_stack_00000008);
    return 1;
  }
  uVar1 = FUN_0059a530(extraout_ECX,param_1,(byte *)s_normal_005c5c1c);
  if (uVar1 == 0) {
    this->mbr_0x2c = 1;
    FUN_00479580(in_stack_00000008);
    return 1;
  }
  uVar1 = FUN_0059a530(extraout_ECX_00,param_1,(byte *)s_alpha_005c5c24);
  if (uVar1 == 0) {
    this->mbr_0x2c = 2;
    FUN_00479580(in_stack_00000008);
    return 1;
  }
  uVar1 = FUN_0059a530(extraout_ECX_01,param_1,(byte *)s_litalpha_005c5c2c);
  if (uVar1 == 0) {
    this->mbr_0x2c = 4;
    FUN_00479580(in_stack_00000008);
    return 1;
  }
  uVar1 = FUN_0059a530(extraout_ECX_02,param_1,(byte *)&PTR_null_005c5c38);
  if (uVar1 == 0) {
    this->mbr_0x2c = 8;
    FUN_00479580(in_stack_00000008);
    return 1;
  }
  uVar1 = FUN_0059a530(extraout_ECX_03,param_1,(byte *)s_litadd_005c5c3c);
  if (uVar1 == 0) {
    this->mbr_0x2c = 0x10;
    FUN_00479580(in_stack_00000008);
    return 1;
  }
  uVar1 = FUN_0059a530(extraout_ECX_04,param_1,(byte *)s_nocheckz_005c5c44);
  if (uVar1 == 0) {
    this->mbr_0x2c = 0x41;
    FUN_00479580(in_stack_00000008);
    return 1;
  }
  uVar1 = FUN_0059a530(extraout_ECX_05,param_1,(byte *)s_litalphaz_005c5c50);
  if (uVar1 == 0) {
    this->mbr_0x2c = 0x44;
    FUN_00479580(in_stack_00000008);
    return 1;
  }
  uVar1 = FUN_0059a530(extraout_ECX_06,param_1,(byte *)s_litaddz_005c5c5c);
  if (uVar1 == 0) {
    this->mbr_0x2c = 0x50;
    FUN_00479580(in_stack_00000008);
    return 1;
  }
  uVar1 = FUN_0059a530(extraout_ECX_07,param_1,(byte *)s_alphaadd_005c5c64);
  if (uVar1 == 0) {
    this->mbr_0x2c = 0x20;
    FUN_00479580(in_stack_00000008);
    return 1;
  }
  uVar2 = this->virt_meth_0x40d4a0(param_1);
  return uVar2;
}



// Function at 00405a80

cls_0x5a3544 * cls_0x5a3544::virt_meth_0x405a80()

{
  byte in_stack_00000004;
  
  cls_0x405aa0::~cls_0x405aa0((cls_0x405aa0 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00405d20

cls_0x5a3544 * cls_0x5a3544::virt_meth_0x405d20()

{
  byte in_stack_00000004;
  
  cls_0x405d40::~cls_0x405d40((cls_0x405d40 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00405ec0

cls_0x5a3544 * cls_0x5a3544::virt_meth_0x405ec0()

{
  byte in_stack_00000004;
  
  ~cls_0x5a3544(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00405ee0

cls_0x5a3544 * __thiscall cls_0x5a3544::~cls_0x5a3544(cls_0x5a3544 *this)

{
  cls_0x5a3544 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  uint local_4;
  
  puStack_8 = &LAB_0059c4be;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a3544__vftable_5a3544_005a3544;
  local_4 = 1;
  virt_meth_0x4036a0(this);
  local_4 = local_4 & 0xffffff00;
  FUN_004830f0((LPCVOID)this->mbr_0x40);
  this->vftptr_0x0 = (cls_0x5a3544__vftable_5a3544 *)&cls_0x5a34a0__vftable_5a34a0_005a34a0;
  local_4 = 2;
  virt_meth_0x40db50(this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a3544 *)FUN_004830f0((LPCVOID)this->mbr_0x28);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00405fe0

cls_0x5a3544 * cls_0x5a3544::virt_meth_0x405fe0()

{
  byte in_stack_00000004;
  
  cls_0x406000::~cls_0x406000((cls_0x406000 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0040db50

void cls_0x5a3544::virt_meth_0x40db50()

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  
  if ((undefined4 *)this->mbr_0x28 != (undefined4 *)0x0) {
    puVar3 = (undefined4 *)this->mbr_0x28;
    for (uVar1 = this->mbr_0x20 & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined *)puVar3 = 0;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
    this->mbr_0x1c = 0;
    this->mbr_0x18 = 0;
    return;
  }
  this->mbr_0x1c = 0;
  this->mbr_0x18 = 0;
  return;
}



