#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x492c40



// Function at 00492c40

void cls_0x492c40::meth_0x492c40()

{
  short sVar1;
  int *piVar2;
  byte in_stack_00000004;
  
  this->mbr_0xb6 = in_stack_00000004;
  this->mbr_0xb5 = 0;
  if ((((this->mbr_0xb4 == 3) || (this->mbr_0xb4 == 8)) &&
      (piVar2 = (int *)this->mbr_0xbc, piVar2 != (int *)0x0)) && ((piVar2[2] & 0x20000U) != 0)) {
    piVar2 = (int *)(**(code **)(*piVar2 + 0x1fc))();
    if (*piVar2 == 0x10) {
      this->mbr_0xb4 = 0;
      sVar1 = *(short *)&((cls_0x4d6000 *)this->mbr_0xbc)->field_0x4;
      if ((sVar1 == 0xc) || (sVar1 == 0xb)) {
        cls_0x4d6000::meth_0x4d6000((cls_0x4d6000 *)this->mbr_0xbc);
      }
      this->mbr_0xbc = 0;
    }
  }
  return;
}



