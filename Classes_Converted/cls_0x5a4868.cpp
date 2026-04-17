#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a4868



// Function at 004461e0

cls_0x5a4868::cls_0x5a4868()

{
  int iVar1;
  dword in_stack_00000004;
  
  iVar1 = DAT_006582c4;
  this->vftptr_0x0 = &cls_0x5a4868__vftable_5a4868_005a4868;
  if (iVar1 <= (int)in_stack_00000004) {
    DAT_006582c4 = in_stack_00000004 + 1;
  }
  *(cls_0x5a4868 **)(&DAT_006581fc.field_0x0 + in_stack_00000004 * 4) = this;
  this->mbr_0x4 = in_stack_00000004;
  return this;
}



