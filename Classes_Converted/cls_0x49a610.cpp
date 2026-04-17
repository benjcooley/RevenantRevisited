#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x49a610



// Function at 0049a610

void cls_0x49a610::meth_0x49a610()

{
  dword dVar1;
  int in_stack_00000004;
  
  if (this->mbr_0x0 != 0) {
    if (in_stack_00000004 < 0x60) {
      if (in_stack_00000004 < 0) {
        in_stack_00000004 = 0;
      }
    }
    else {
      in_stack_00000004 = 0x60;
    }
    if (this->mbr_0x8 != 0) {
      _AIL_redbook_set_volume_8(this->mbr_0x0,in_stack_00000004);
      dVar1 = _AIL_redbook_volume_4(this->mbr_0x0);
      this->mbr_0xc = dVar1;
    }
  }
  return;
}



