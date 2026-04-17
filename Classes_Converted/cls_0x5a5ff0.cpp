#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a5ff0



// Function at 004779d0

void cls_0x5a5ff0::meth_0x4779d0()

{
  int in_stack_00000004;
  
  if ((int)((this->mbr_0xc - this->mbr_0x8) + this->mbr_0x4) < in_stack_00000004) {
    meth_0x49cc70(this);
  }
  return;
}



// Function at 00477c90

cls_0x5a5ff0 * cls_0x5a5ff0::meth_0x477c90()

{
  undefined2 *puVar1;
  undefined2 in_stack_00000004;
  
  if ((int)((this->mbr_0xc + this->mbr_0x4) - this->mbr_0x8) < 2) {
    meth_0x49cc70(this);
  }
  puVar1 = (undefined2 *)this->mbr_0x8;
  *puVar1 = in_stack_00000004;
  this->mbr_0x8 = (dword)(puVar1 + 1);
  return this;
}



// Function at 00477cd0

cls_0x5a5ff0 * cls_0x5a5ff0::meth_0x477cd0()

{
  undefined *puVar1;
  undefined in_stack_00000004;
  
  if ((int)((this->mbr_0xc + this->mbr_0x4) - this->mbr_0x8) < 1) {
    meth_0x49cc70(this);
  }
  puVar1 = (undefined *)this->mbr_0x8;
  *puVar1 = in_stack_00000004;
  this->mbr_0x8 = (dword)(puVar1 + 1);
  return this;
}



// Function at 0049cc20

cls_0x5a5ff0::cls_0x5a5ff0 *this,uint param_1)

{
  undefined4 *puVar1;
  dword in_stack_00000008;
  
  this->vftptr_0x0 = &cls_0x5a5ff0__vftable_5a5ff0_005a5ff0;
  puVar1 = FUN_00482ef0(param_1);
  this->mbr_0x4 = (dword)puVar1;
  this->mbr_0xc = param_1;
  this->mbr_0x8 = (dword)puVar1;
  this->mbr_0x10 = in_stack_00000008;
  return this;
}



// Function at 0049cc50

cls_0x5a5ff0 * __thiscall cls_0x5a5ff0::~cls_0x5a5ff0(cls_0x5a5ff0 *this)

{
  cls_0x5a5ff0 *in_EAX;
  
  this->vftptr_0x0 = &cls_0x5a5ff0__vftable_5a5ff0_005a5ff0;
  if ((LPCVOID)this->mbr_0x4 != (LPCVOID)0x0) {
    in_EAX = (cls_0x5a5ff0 *)FUN_004830f0((LPCVOID)this->mbr_0x4);
  }
  return in_EAX;
}



// Function at 0049cc70

void cls_0x5a5ff0::meth_0x49cc70()

{
  dword dVar1;
  int iVar2;
  int in_stack_00000004;
  
  iVar2 = this->mbr_0x8 - (int)(int *)this->mbr_0x4;
  if ((int)(this->mbr_0xc - iVar2) < in_stack_00000004) {
    dVar1 = FUN_00482f40((int *)this->mbr_0x4,(uint *)(this->mbr_0xc + this->mbr_0x10));
    this->mbr_0x4 = dVar1;
    this->mbr_0x8 = dVar1 + iVar2;
    this->mbr_0xc = this->mbr_0xc + this->mbr_0x10;
  }
  return;
}



// Function at 0049ccc0

cls_0x5a5ff0 * cls_0x5a5ff0::meth_0x49ccc0()

{
  char cVar1;
  int *piVar2;
  undefined *puVar3;
  dword dVar4;
  uint uVar5;
  int iVar6;
  char *pcVar7;
  char *in_stack_00000004;
  
  uVar5 = 0xffffffff;
  piVar2 = (int *)this->mbr_0x4;
  pcVar7 = in_stack_00000004;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  uVar5 = ~uVar5;
  dVar4 = this->mbr_0xc;
  if (((int)((int)piVar2 + (dVar4 - this->mbr_0x8)) < (int)(uVar5 + 1)) &&
     (iVar6 = this->mbr_0x8 - (int)piVar2, (int)(dVar4 - iVar6) < (int)(uVar5 + 1))) {
    dVar4 = FUN_00482f40(piVar2,(uint *)(dVar4 + this->mbr_0x10));
    this->mbr_0x4 = dVar4;
    this->mbr_0x8 = dVar4 + iVar6;
    this->mbr_0xc = this->mbr_0xc + this->mbr_0x10;
  }
  puVar3 = (undefined *)this->mbr_0x8;
  *puVar3 = (char)(uVar5 - 1);
  _strncpy(puVar3 + 1,in_stack_00000004,uVar5 - 1 & 0xff);
  this->mbr_0x8 = this->mbr_0x8 + uVar5;
  return this;
}



// Function at 0049cd50

uint cls_0x5a5ff0::meth_0x49cd50(undefined4 *param_1)

{
  int *piVar1;
  int iVar2;
  dword dVar3;
  uint uVar4;
  undefined4 *puVar5;
  uint in_stack_00000008;
  
  piVar1 = (int *)this->mbr_0x4;
  dVar3 = this->mbr_0xc;
  if (((int)((int)piVar1 + (dVar3 - this->mbr_0x8)) < (int)in_stack_00000008) &&
     (iVar2 = this->mbr_0x8 - (int)piVar1, (int)(dVar3 - iVar2) < (int)in_stack_00000008)) {
    dVar3 = FUN_00482f40(piVar1,(uint *)(this->mbr_0x10 + dVar3));
    this->mbr_0x4 = dVar3;
    this->mbr_0x8 = dVar3 + iVar2;
    this->mbr_0xc = this->mbr_0xc + this->mbr_0x10;
  }
  puVar5 = (undefined4 *)this->mbr_0x8;
  for (uVar4 = in_stack_00000008 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar5 = *param_1;
    param_1 = param_1 + 1;
    puVar5 = puVar5 + 1;
  }
  for (uVar4 = in_stack_00000008 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined *)puVar5 = *(undefined *)param_1;
    param_1 = (undefined4 *)((int)param_1 + 1);
    puVar5 = (undefined4 *)((int)puVar5 + 1);
  }
  this->mbr_0x8 = this->mbr_0x8 + in_stack_00000008;
  return in_stack_00000008;
}



// Function at 0049cdd0

dword cls_0x5a5ff0::meth_0x49cdd0()

{
  dword dVar1;
  
  dVar1 = this->mbr_0x4;
  if ((int)this->mbr_0xc < (int)(this->mbr_0x8 - dVar1)) {
    FUN_0054d190((TCharacter *)&DAT_0065c5d0,1,s_Output_Stream_Overrun_005daa58);
    dVar1 = 0;
  }
  return dVar1;
}



// Function at 0049ce80

cls_0x5a5ff0 * __thiscall cls_0x5a5ff0::~cls_0x5a5ff0(cls_0x5a5ff0 *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5a5ff0__vftable_5a5ff0_005a5ff0;
  if ((LPCVOID)this->mbr_0x4 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x4);
  }
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



