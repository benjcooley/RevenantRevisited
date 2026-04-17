#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4c58f0



// Function at 004c58f0

undefined4 cls_0x4c58f0::meth_0x4c58f0()

{
  int iVar1;
  dword *pdVar2;
  int iVar3;
  dword in_stack_00000004;
  
  iVar3 = 0;
  pdVar2 = &this->mbr_0x1c4;
  do {
    if (pdVar2[-1] == in_stack_00000004) {
      iVar1 = TCharacter::meth_0x47e920((TCharacter *)&DAT_0065caf0);
      if ((int)(iVar1 - *pdVar2) < 0x438) {
        return 1;
      }
    }
    iVar3 = iVar3 + 1;
    pdVar2 = pdVar2 + 3;
  } while (iVar3 < 8);
  return 0;
}



