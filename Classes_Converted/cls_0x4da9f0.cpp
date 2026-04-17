#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4da9f0



// Function at 004d7030

void cls_0x4da9f0::meth_0x4d7030()

{
  if ((LPCVOID)this->mbr_0x5c != (LPCVOID)0x0) {
    FUN_00482f80((LPCVOID)this->mbr_0x5c);
  }
  return;
}



// Function at 004da9f0

cls_0x4da9f0::cls_0x4da9f0 *this,char *param_1)

{
  char cVar1;
  uint uVar2;
  uint uVar3;
  dword *pdVar4;
  char *pcVar5;
  dword *pdVar6;
  dword in_stack_00000008;
  
  uVar2 = 0xffffffff;
  this->mbr_0x24 = 0xffffffff;
  do {
    pcVar5 = param_1;
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    pcVar5 = param_1 + 1;
    cVar1 = *param_1;
    param_1 = pcVar5;
  } while (cVar1 != '\0');
  this->mbr_0x28 = 0;
  uVar2 = ~uVar2;
  *(undefined *)&this->mbr_0x4 = 0;
  this->mbr_0x30 = 0;
  this->mbr_0x2c = 0;
  this->mbr_0x40 = 0;
  this->mbr_0x3c = 0;
  this->mbr_0x38 = 0;
  this->mbr_0x44 = 0;
  this->mbr_0x5c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x0 = 1;
  this->mbr_0x34 = 0x10;
  this->mbr_0x60 = 1;
  pdVar4 = (dword *)(pcVar5 + -uVar2);
  pdVar6 = &this->mbr_0x4;
  for (uVar3 = uVar2 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pdVar6 = *pdVar4;
    pdVar4 = pdVar4 + 1;
    pdVar6 = pdVar6 + 1;
  }
  for (uVar2 = uVar2 & 3; uVar2 != 0; uVar2 = uVar2 - 1) {
    *(undefined *)pdVar6 = *(undefined *)pdVar4;
    pdVar4 = (dword *)((int)pdVar4 + 1);
    pdVar6 = (dword *)((int)pdVar6 + 1);
  }
  this->mbr_0x0 = in_stack_00000008;
  return this;
}



