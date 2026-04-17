#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x573c20



// Function at 00573c20

void cls_0x573c20::meth_0x573c20()

{
  int *piVar1;
  char cVar2;
  DWORD DVar3;
  int *piVar4;
  
  DVar3 = timeGetTime();
  if (*(int *)this->mbr_0x4 != 0) {
    piVar4 = (int *)((int *)this->mbr_0x4)[2];
    while (piVar1 = piVar4, piVar1 != (int *)0x0) {
      if (*(int *)piVar1[0x10] == 0) {
        piVar4 = (int *)0x0;
      }
      else {
        piVar4 = (int *)((int *)piVar1[0x10])[2];
      }
      cVar2 = (**(code **)(*piVar1 + 8))(DVar3);
      if ((cVar2 == '\0') && (piVar1 != (int *)0x0)) {
        (**(code **)*piVar1)(1);
      }
    }
  }
  if (*(int *)this->mbr_0x10 != 0) {
    piVar4 = (int *)((int *)this->mbr_0x10)[2];
    while (piVar1 = piVar4, piVar1 != (int *)0x0) {
      if (*(int *)piVar1[0x10] == 0) {
        piVar4 = (int *)0x0;
      }
      else {
        piVar4 = (int *)((int *)piVar1[0x10])[2];
      }
      cVar2 = (**(code **)(*piVar1 + 8))(DVar3);
      if ((cVar2 == '\0') && (piVar1 != (int *)0x0)) {
        (**(code **)*piVar1)(1);
      }
    }
  }
  return;
}



