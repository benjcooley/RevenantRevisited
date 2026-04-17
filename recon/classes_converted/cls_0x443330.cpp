#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x443330



// Function at 00443330

void cls_0x443330::meth_0x443330()

{
  int iVar1;
  dword dVar2;
  int in_stack_00000004;
  
  iVar1 = FUN_00452690(in_stack_00000004,0);
  if ((iVar1 != 0) &&
     ((this->mbr_0x17c != (int)*(short *)(iVar1 + 4) ||
      (this->mbr_0x180 != (int)*(short *)(iVar1 + 6))))) {
    this->mbr_0x17c = (int)*(short *)(iVar1 + 4);
    dVar2 = (dword)*(short *)(iVar1 + 6);
    this->mbr_0x180 = dVar2;
    if ((((int)dVar2 < (int)this->mbr_0x184) ||
        ((int)this->mbr_0x184 <= (int)(dVar2 - this->mbr_0x188))) &&
       (dVar2 = dVar2 - (int)this->mbr_0x188 / 2, this->mbr_0x184 = dVar2, (int)dVar2 < 0)) {
      this->mbr_0x184 = 0;
    }
    (**(code **)(this->mbr_0x0 + 0x2c))(1);
  }
  return;
}



