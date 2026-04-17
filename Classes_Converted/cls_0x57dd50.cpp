#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x57dd50



// Function at 0057dd50

void cls_0x57dd50::meth_0x57dd50()

{
  dword **ppdVar1;
  dword *pdVar2;
  dword **ppdVar3;
  int **ppiVar4;
  dword dVar5;
  
  dVar5 = TCharacter::meth_0x47e940((TCharacter *)&DAT_0065caf0);
  if ((int)dVar5 < (int)this->mbr_0x98) {
    while ((*(int *)this->mbr_0x8c != 0 &&
           (ppdVar1 = (dword **)((int *)this->mbr_0x8c)[2], ppdVar1 != (dword **)0x0))) {
      pdVar2 = *ppdVar1;
      if (pdVar2 != (dword *)0x0) {
        ppdVar3 = (dword **)ppdVar1[1];
        pdVar2[1] = (dword)ppdVar3;
        *ppdVar3 = pdVar2;
        *ppdVar1 = (dword *)0x0;
      }
      *ppdVar1 = &this->mbr_0x84;
      ppiVar4 = (int **)this->mbr_0x88;
      ppdVar1[1] = (dword *)ppiVar4;
      this->mbr_0x88 = (dword)ppdVar1;
      *ppiVar4 = (int *)ppdVar1;
    }
  }
  else {
    while (((*(int *)this->mbr_0x8c != 0 &&
            (ppdVar1 = (dword **)((int *)this->mbr_0x8c)[2], ppdVar1 != (dword **)0x0)) &&
           ((int)ppdVar1[5] < (int)dVar5))) {
      pdVar2 = *ppdVar1;
      if (pdVar2 != (dword *)0x0) {
        ppdVar3 = (dword **)ppdVar1[1];
        pdVar2[1] = (dword)ppdVar3;
        *ppdVar3 = pdVar2;
        *ppdVar1 = (dword *)0x0;
      }
      *ppdVar1 = &this->mbr_0x84;
      ppiVar4 = (int **)this->mbr_0x88;
      ppdVar1[1] = (dword *)ppiVar4;
      this->mbr_0x88 = (dword)ppdVar1;
      *ppiVar4 = (int *)ppdVar1;
    }
  }
  this->mbr_0x98 = dVar5;
  this->mbr_0x9c = 0;
  this->mbr_0x55 = 0;
  this->mbr_0x7c = 0;
  (**(code **)(this->mbr_0x0 + 8))();
  return;
}



