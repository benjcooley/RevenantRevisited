#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a4d6c



// Function at 00465a50

undefined4 cls_0x5a4d6c::virt_meth_0x465a50()

{
  char cVar1;
  int iVar2;
  int *piVar3;
  uint uVar4;
  undefined4 *puVar5;
  dword dVar6;
  undefined4 uVar7;
  cls_0x5b4f30_TPlayer *this_00;
  uint uVar8;
  int iVar9;
  undefined4 *puVar10;
  char *pcVar11;
  char *pcVar12;
  undefined4 *puVar13;
  bool bVar14;
  
  piVar3 = FUN_004a1ec0(100,100,*(uint *)(PTR_DAT_005d79e0 + 0x38) & 0x3001f,0);
  uVar8 = DAT_0065a258_TObjectClass_numclasses;
  this->mbr_0x27c = (dword)piVar3;
  iVar9 = 0;
  uVar4 = 0;
  if (0 < (int)uVar8) {
    piVar3 = &DAT_0065a148_TObjectClass_classes;
    bVar14 = uVar8 != 0;
    do {
      if (((bVar14) && (*piVar3 != 0)) && (iVar2 = *(int *)(*piVar3 + 0x24), iVar9 < iVar2)) {
        iVar9 = iVar2;
      }
      uVar4 = uVar4 + 1;
      piVar3 = piVar3 + 1;
      bVar14 = uVar4 < uVar8;
    } while ((int)uVar4 < (int)uVar8);
  }
  puVar5 = FUN_00482fb0(iVar9 * 4);
  this->mbr_0x288 = (dword)puVar5;
  dVar6 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x1b8 = dVar6;
  dVar6 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x1bc = dVar6;
  dVar6 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x1c0 = dVar6;
  dVar6 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x1d4 = dVar6;
  dVar6 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x1d8 = dVar6;
  dVar6 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x1dc = dVar6;
  if ((this->mbr_0x190 == 1) || (this->mbr_0x190 == 2)) {
    uVar7 = 0x11;
  }
  else {
    uVar7 = 0;
  }
  if (DAT_0065c890 == 0) {
    iVar9 = this->virt_meth_0x435150(s_createchar_005d3300,0x659c80,uVar7,0,0,0x280,0x1e0,
                       0x1c2,0xa0,s_widgets_005d32f8);
  }
  else {
    iVar9 = this->virt_meth_0x435230(s_createchar_005d32e0,0x659c7c,uVar7,0,0,0x280,0x1e0,
                       0x1c2,0xa0,DAT_0066733c);
  }
  if (iVar9 == 0) {
    return 0;
  }
  this->mbr_0x1f4 = 1;
  this->mbr_0x1f0 = 1;
  if (this->mbr_0x194 == 2) {
    this_00 = (cls_0x5b4f30_TPlayer *)FUN_00474e20_NewObjectByName((byte *)s_Locke_005d330c,(undefined4 *)0x0,0xffffffff);
    this->mbr_0x1f8 = (dword)this_00;
    cls_0x5b4f30_TPlayer::meth_0x46e6f0(this_00);
  }
  (*this->vftptr_0x0->FUN_00435de0_80)();
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  if (DAT_00667fd0 != &DAT_0065caf0) {
    this->mbr_0x19c = 0;
    this->mbr_0x198 = 0;
    return 1;
  }
  uVar8 = 0xffffffff;
  pcVar11 = *(char **)(this->mbr_0x1f8 + 0x38);
  pcVar12 = pcVar11;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar1 = *pcVar12;
    pcVar12 = pcVar12 + 1;
  } while (cVar1 != '\0');
  puVar5 = FUN_00482ef0(~uVar8);
  uVar8 = 0xffffffff;
  do {
    pcVar12 = pcVar11;
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    pcVar12 = pcVar11 + 1;
    cVar1 = *pcVar11;
    pcVar11 = pcVar12;
  } while (cVar1 != '\0');
  uVar8 = ~uVar8;
  puVar10 = (undefined4 *)(pcVar12 + -uVar8);
  puVar13 = puVar5;
  for (uVar4 = uVar8 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar13 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar13 = puVar13 + 1;
  }
  for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *(undefined *)puVar13 = *(undefined *)puVar10;
    puVar10 = (undefined4 *)((int)puVar10 + 1);
    puVar13 = (undefined4 *)((int)puVar13 + 1);
  }
  uVar8 = 0xffffffff;
  this->mbr_0x198 = (dword)puVar5;
  pcVar11 = (char *)(this->mbr_0x1f8 + 0x378);
  pcVar12 = pcVar11;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar1 = *pcVar12;
    pcVar12 = pcVar12 + 1;
  } while (cVar1 != '\0');
  puVar5 = FUN_00482ef0(~uVar8);
  uVar8 = 0xffffffff;
  do {
    pcVar12 = pcVar11;
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    pcVar12 = pcVar11 + 1;
    cVar1 = *pcVar11;
    pcVar11 = pcVar12;
  } while (cVar1 != '\0');
  uVar8 = ~uVar8;
  puVar10 = (undefined4 *)(pcVar12 + -uVar8);
  puVar13 = puVar5;
  for (uVar4 = uVar8 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar13 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar13 = puVar13 + 1;
  }
  for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *(undefined *)puVar13 = *(undefined *)puVar10;
    puVar10 = (undefined4 *)((int)puVar10 + 1);
    puVar13 = (undefined4 *)((int)puVar13 + 1);
  }
  this->mbr_0x19c = (dword)puVar5;
  return 1;
}



// Function at 00465ce0

void cls_0x5a4d6c::virt_meth_0x465ce0()

{
  FUN_004830f0((LPCVOID)this->mbr_0x27c);
  FUN_004830f0((LPCVOID)this->mbr_0x288);
  if ((LPCVOID)this->mbr_0x198 == (LPCVOID)0x0) {
    if (this->mbr_0x19c == 0) goto LAB_00465d46;
  }
  else {
    FUN_00482f80((LPCVOID)this->mbr_0x198);
    this->mbr_0x198 = 0;
  }
  if ((LPCVOID)this->mbr_0x19c != (LPCVOID)0x0) {
    FUN_00482f80((LPCVOID)this->mbr_0x19c);
    this->mbr_0x19c = 0;
  }
LAB_00465d46:
  this->virt_meth_0x434f30();
  return;
}



// Function at 00469340

void cls_0x5a4d6c::virt_meth_0x469340()

{
  this->virt_meth_0x435d70();
  if (DAT_00676e84 != '\0') {
    cls_0x5b98b8::meth_0x437ca0((cls_0x5b98b8 *)this);
    cls_0x5b98b8::meth_0x465d50((cls_0x5b98b8 *)this);
    cls_0x5b98b8::meth_0x465ea0((cls_0x5b98b8 *)this);
  }
  return;
}



// Function at 0046d160

cls_0x5a4d6c::cls_0x5a4d6c()

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->vftptr_0x0 = (cls_0x5a4d6c__vftable_5a4d6c *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a4d6c__vftable_5a4d6c_005a4d6c;
  this->mbr_0x190 = 0xffffffff;
  this->mbr_0x194 = 0xffffffff;
  this->mbr_0x1f8 = 0;
  this->mbr_0x1fc = 0;
  return this;
}



// Function at 0046d1d0

cls_0x5a4d6c * cls_0x5a4d6c::virt_meth_0x46d1d0()

{
  byte in_stack_00000004;
  
  ~cls_0x5a4d6c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0046d1f0

cls_0x5a4d6c * __thiscall cls_0x5a4d6c::~cls_0x5a4d6c(cls_0x5a4d6c *this)

{
  cls_0x5a4d6c *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d19e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a4d6c__vftable_5a4d6c *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a4d6c *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



