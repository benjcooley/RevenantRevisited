#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x42aa60



// Function at 0042aa60

undefined4 cls_0x42aa60::meth_0x42aa60()

{
  undefined4 uVar1;
  
  if ((byte *)this->mbr_0x5c != (byte *)0x0) {
    uVar1 = FUN_0058b42c((undefined *)this,(byte *)this->mbr_0x5c);
    return uVar1;
  }
  return 0;
}



// Function at 0042aa80

void cls_0x42aa60::meth_0x42aa80()

{
  uint in_stack_00000004;
  char local_14 [20];
  
  FUN_0058d252(in_stack_00000004,local_14,10);
  (**(code **)(this->mbr_0x0 + 0x18))(local_14);
  return;
}



