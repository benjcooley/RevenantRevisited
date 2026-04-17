#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x534a40



// Function at 00534a40

void cls_0x534a40::meth_0x534a40()

{
  int iVar1;
  dword *pdVar2;
  
  iVar1 = 0;
  this->mbr_0x50 = 1;
  this->mbr_0x58 = 0;
  if (0 < (int)this->mbr_0x5c) {
    pdVar2 = &this->mbr_0x100;
    do {
      if (*pdVar2 != 0) {
        cls_0x4367d0::meth_0x4367d0((cls_0x4367d0 *)this->mbr_0x0);
        *pdVar2 = 0;
      }
      iVar1 = iVar1 + 1;
      pdVar2 = pdVar2 + 1;
    } while (iVar1 < (int)this->mbr_0x5c);
  }
  return;
}



