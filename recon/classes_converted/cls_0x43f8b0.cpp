#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x43f8b0



// Function at 0043f8b0

void cls_0x43f8b0::meth_0x43f8b0()

{
  char cVar1;
  int iVar2;
  char *pcVar3;
  uint uVar4;
  uint uVar5;
  undefined4 *puVar6;
  char *pcVar7;
  undefined4 *puVar8;
  dword in_stack_00000004;
  
  this->mbr_0xa0 = in_stack_00000004;
  iVar2 = cls_0x497370::meth_0x497370_TScriptManager_ObjectScript((cls_0x497370 *)&DAT_0065def0);
  if (*(int *)(iVar2 + 8) == 0) {
    pcVar3 = (char *)0x0;
  }
  else {
    pcVar3 = *(char **)(*(int *)(iVar2 + 8) + 4);
  }
  uVar4 = 0xffffffff;
  do {
    pcVar7 = pcVar3;
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    pcVar7 = pcVar3 + 1;
    cVar1 = *pcVar3;
    pcVar3 = pcVar7;
  } while (cVar1 != '\0');
  uVar4 = ~uVar4;
  puVar6 = (undefined4 *)(pcVar7 + -uVar4);
  puVar8 = (undefined4 *)this->mbr_0x60;
  for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
    *puVar8 = *puVar6;
    puVar6 = puVar6 + 1;
    puVar8 = puVar8 + 1;
  }
  for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined *)puVar8 = *(undefined *)puVar6;
    puVar6 = (undefined4 *)((int)puVar6 + 1);
    puVar8 = (undefined4 *)((int)puVar8 + 1);
  }
  this->mbr_0x70 = 0;
  this->mbr_0x68 = 0;
  this->mbr_0x90 = 0;
  this->mbr_0x78 = 0;
  this->mbr_0x8c = 0;
  this->mbr_0x74 = 0;
  return;
}



