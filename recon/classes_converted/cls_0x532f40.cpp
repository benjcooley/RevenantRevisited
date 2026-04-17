#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x532f40



// Function at 00532f40

void cls_0x532f40::meth_0x532f40()

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (short)this->mbr_0x194) {
    iVar1 = 0;
    do {
      FUN_0052f310((LPCVOID *)(this->mbr_0x198 + iVar1));
      iVar2 = iVar2 + 1;
      iVar1 = iVar1 + 0x48;
    } while (iVar2 < (short)this->mbr_0x194);
  }
  if ((LPCVOID)this->mbr_0x198 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x198);
  }
  this->mbr_0x198 = 0;
  this->mbr_0x194 = 0;
  this->mbr_0x196 = 0;
  return;
}



