#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x51fe50



// Function at 0051fe50

void cls_0x51fe50::meth_0x51fe50()

{
  int iVar1;
  uint uVar2;
  int in_stack_00000004;
  
  iVar1 = (**(code **)(this->mbr_0x0 + 0xdc))(in_stack_00000004 + 0x22);
  if (iVar1 < 0x1e) {
    if ((in_stack_00000004 == *(int *)(&this->field_0x360 + this->mbr_0x368 * 4)) &&
       (iVar1 = (**(code **)(this->mbr_0x0 + 0xdc))(in_stack_00000004 + 0x22), 0x1c < iVar1)) {
      return;
    }
    uVar2 = (uint)(this->mbr_0x368 == 0);
    this->mbr_0x368 = uVar2;
    *(int *)(&this->field_0x360 + uVar2 * 4) = in_stack_00000004;
    (**(code **)(this->mbr_0x0 + 0xd0))(s_trainstat_005e2bc4);
  }
  return;
}



