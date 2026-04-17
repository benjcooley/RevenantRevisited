#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a50e4



// Function at 0046df00

cls_0x5a50e4::cls_0x5a50e4()

{
  int iVar1;
  dword in_stack_00000004;
  
  iVar1 = DAT_0065a248;
  this->vftptr_0x0 = &cls_0x5a50e4__vftable_5a50e4_005a50e4;
  if (iVar1 < 0x100) {
    (&DAT_00659d48)[iVar1] = this;
    DAT_0065a248 = iVar1 + 1;
    this->mbr_0x4 = in_stack_00000004;
    return this;
  }
  this->mbr_0x4 = in_stack_00000004;
  return this;
}



