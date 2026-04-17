#include "SCharData.h"

// Decompiled methods and structure for class: SCharData



// Function at 004d7030

void SCharData::meth_0x4d7030()

{
  if ((LPCVOID)this->bowwait != (LPCVOID)0x0) {
    FUN_00482f80((LPCVOID)this->bowwait);
  }
  return;
}



// Function at 004da9f0

SCharData::SCharData *this,char *param_1)

{
  char cVar1;
  uint uVar2;
  uint uVar3;
  dword *pdVar4;
  char *pcVar5;
  dword *pdVar6;
  dword in_stack_00000008;
  
  uVar2 = 0xffffffff;
  this->objtype = 0xffffffff;
  do {
    pcVar5 = param_1;
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    pcVar5 = param_1 + 1;
    cVar1 = *param_1;
    param_1 = pcVar5;
  } while (cVar1 != '\0');
  this->flags = 0;
  uVar2 = ~uVar2;
  *(undefined *)&this->groups = 0;
  this->maxattackrange = 0;
  this->damagemods = 0;
  this->armorvalue = 0;
  this->weapondamage = 0;
  this->weapontype = 0;
  this->defensemod = 0;
  this->bowwait = 0;
  this->arrowspeed = 0;
  this->attackmod = 0;
  this->arrowpos = 0;
  this->name = 1;
  this->swipecolor = 0x10;
  this->mbr_0x60 = 1;
  pdVar4 = (dword *)(pcVar5 + -uVar2);
  pdVar6 = &this->groups;
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
  this->name = in_stack_00000008;
  return this;
}



