#include "SLabel.h"

// Decompiled methods and structure for class: SLabel



// Function at 00492c40

void SLabel::meth_0x492c40()

{
  short sVar1;
  int *piVar2;
  byte in_stack_00000004;
  
  this->label = in_stack_00000004;
  this->isgroup = 0;
  if ((((this->used == 3) || (this->used == 8)) &&
      (piVar2 = (int *)this->index, piVar2 != (int *)0x0)) && ((piVar2[2] & 0x20000U) != 0)) {
    piVar2 = (int *)(**(code **)(*piVar2 + 0x1fc))();
    if (*piVar2 == 0x10) {
      this->used = 0;
      sVar1 = *(short *)&((cls_0x4d6000 *)this->index)->field_0x4;
      if ((sVar1 == 0xc) || (sVar1 == 0xb)) {
        cls_0x4d6000::meth_0x4d6000((cls_0x4d6000 *)this->index);
      }
      this->index = 0;
    }
  }
  return;
}



