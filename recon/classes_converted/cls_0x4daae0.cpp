#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4daae0



// Function at 004daae0

cls_0x4daae0::cls_0x4daae0 *this,dword *param_1,char *param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  dword *pdVar5;
  cls_0x4daae0 *pcVar6;
  char *pcVar7;
  dword *pdVar8;
  dword in_stack_0000000c;
  
  pcVar6 = this;
  for (iVar2 = 0x19; iVar2 != 0; iVar2 = iVar2 + -1) {
    pcVar6->mbr_0x0 = *param_1;
    param_1 = param_1 + 1;
    pcVar6 = (cls_0x4daae0 *)&pcVar6->mbr_0x4;
  }
  if (param_2 != (char *)0x0) {
    uVar3 = 0xffffffff;
    do {
      pcVar7 = param_2;
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      pcVar7 = param_2 + 1;
      cVar1 = *param_2;
      param_2 = pcVar7;
    } while (cVar1 != '\0');
    uVar3 = ~uVar3;
    pdVar5 = (dword *)(pcVar7 + -uVar3);
    pdVar8 = &this->mbr_0x4;
    for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *pdVar8 = *pdVar5;
      pdVar5 = pdVar5 + 1;
      pdVar8 = pdVar8 + 1;
    }
    for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
      *(undefined *)pdVar8 = *(undefined *)pdVar5;
      pdVar5 = (dword *)((int)pdVar5 + 1);
      pdVar8 = (dword *)((int)pdVar8 + 1);
    }
  }
  if (in_stack_0000000c != 0) {
    this->mbr_0x0 = in_stack_0000000c;
  }
  return this;
}



