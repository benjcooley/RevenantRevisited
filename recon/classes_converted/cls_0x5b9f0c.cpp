#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b9f0c



// Function at 00572610

cls_0x5b9f0c::cls_0x5b9f0c *this,dword param_1,undefined param_2,char *param_3)

{
  char cVar1;
  DWORD DVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  undefined4 *puVar6;
  char *pcVar7;
  undefined3 in_stack_00000009;
  dword in_stack_00000010;
  dword in_stack_00000014;
  
  this->vftptr_0x0 = &cls_0x5b9f0c__vftable_5b9f0c_005b9f0c;
  this->mbr_0x4 = param_1;
  this->mbr_0x8 = _param_2;
  this->mbr_0x10 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x18 = 0x200;
  this->mbr_0x1c = 0;
  this->mbr_0x20 = 0;
  this->mbr_0x24 = 0;
  this->mbr_0x38 = 0;
  this->mbr_0xc = in_stack_00000010;
  DVar2 = timeGetTime();
  this->mbr_0x2c = 0;
  this->mbr_0x28 = DVar2 + 5000;
  this->mbr_0x30 = in_stack_00000014;
  uVar4 = 0xffffffff;
  this->mbr_0x3c = 1;
  pcVar7 = param_3;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  puVar3 = FUN_00482fb0(~uVar4);
  uVar4 = 0xffffffff;
  this->mbr_0x34 = (dword)puVar3;
  do {
    pcVar7 = param_3;
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    pcVar7 = param_3 + 1;
    cVar1 = *param_3;
    param_3 = pcVar7;
  } while (cVar1 != '\0');
  uVar4 = ~uVar4;
  puVar6 = (undefined4 *)(pcVar7 + -uVar4);
  for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
    *puVar3 = *puVar6;
    puVar6 = puVar6 + 1;
    puVar3 = puVar3 + 1;
  }
  for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined *)puVar3 = *(undefined *)puVar6;
    puVar6 = (undefined4 *)((int)puVar6 + 1);
    puVar3 = (undefined4 *)((int)puVar3 + 1);
  }
  return this;
}



// Function at 00573ce0

cls_0x5b9f0c * __thiscall cls_0x5b9f0c::~cls_0x5b9f0c(cls_0x5b9f0c *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5b9f0c__vftable_5b9f0c_005b9f0c;
  if ((LPCVOID)this->mbr_0x38 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x38);
  }
  if ((HANDLE)this->mbr_0x10 != (HANDLE)0x0) {
    CloseHandle((HANDLE)this->mbr_0x10);
  }
  if ((LPCVOID)this->mbr_0x34 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x34);
  }
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00573d70

cls_0x5b9f0c * __thiscall cls_0x5b9f0c::~cls_0x5b9f0c(cls_0x5b9f0c *this)

{
  dword dVar1;
  dword *pdVar2;
  dword *pdVar3;
  cls_0x5b9f0c *hObject;
  
  if (this == (cls_0x5b9f0c *)0x0) {
    pdVar3 = (dword *)0x0;
  }
  else {
    pdVar3 = &this->mbr_0x40;
  }
  dVar1 = *pdVar3;
  if (dVar1 != 0) {
    pdVar2 = (dword *)pdVar3[1];
    *pdVar3 = 0;
    *(dword **)(dVar1 + 4) = pdVar2;
    *pdVar2 = dVar1;
  }
  this->vftptr_0x0 = &cls_0x5b9f0c__vftable_5b9f0c_005b9f0c;
  if ((LPCVOID)this->mbr_0x38 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x38);
  }
  hObject = (cls_0x5b9f0c *)this->mbr_0x10;
  if (hObject != (cls_0x5b9f0c *)0x0) {
    hObject = (cls_0x5b9f0c *)CloseHandle(hObject);
  }
  if ((LPCVOID)this->mbr_0x34 != (LPCVOID)0x0) {
    hObject = (cls_0x5b9f0c *)FUN_004830f0((LPCVOID)this->mbr_0x34);
  }
  return hObject;
}



