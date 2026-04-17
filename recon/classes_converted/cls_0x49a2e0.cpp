#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x49a2e0



// Function at 0049a2e0

void cls_0x49a2e0::meth_0x49a2e0()

{
  if (this->mbr_0x0 != 0) {
    _AIL_redbook_close_4(this->mbr_0x0);
  }
  this->mbr_0x0 = 0;
  return;
}



// Function at 0049a560

void cls_0x49a2e0::meth_0x49a560()

{
  int iVar1;
  
  if (this->mbr_0x0 != 0) {
    if (this->mbr_0x8 != 0) {
      iVar1 = _AIL_redbook_status_4(this->mbr_0x0);
      if (iVar1 == 1) {
        _AIL_redbook_stop_4(this->mbr_0x0);
      }
    }
    this->mbr_0x8 = 0;
  }
  return;
}



