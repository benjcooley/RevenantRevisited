#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x53f680



// Function at 0053f680

void cls_0x53f680::meth_0x53f680()

{
  int *piVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  
  piVar1 = (int *)this->mbr_0x4;
  iVar5 = *(int *)(this->mbr_0x120 + 0x4c);
  if ((piVar1 != (int *)0x0) && ((DAT_0066810c == 0 || (*(short *)(piVar1 + 1) != 0xb)))) {
    if (*(short *)(piVar1 + 1) == 0xb) {
      iVar2 = (**(code **)(*piVar1 + 0x3c4))();
      iVar2 = (int)((ulonglong)((longlong)(iVar2 * iVar5) * -0x51eb851f) >> 0x20);
      iVar5 = iVar5 + ((iVar2 >> 5) - (iVar2 >> 0x1f));
    }
    iVar2 = *(int *)this->mbr_0x4;
    iVar3 = (**(code **)(iVar2 + 0x1d0))();
    (**(code **)(iVar2 + 0x1d4))(iVar3 - iVar5);
    piVar1 = (int *)this->mbr_0x4;
    iVar5 = (**(code **)(*piVar1 + 0x1e8))();
    iVar2 = (**(code **)(*piVar1 + 0x1d0))();
    if (iVar5 < iVar2) {
      iVar5 = *(int *)this->mbr_0x4;
      uVar4 = (**(code **)(iVar5 + 0x1e8))();
      (**(code **)(iVar5 + 0x1d4))(uVar4);
    }
  }
  return;
}



