#include "TStatPane.h"

// Decompiled methods and structure for class: TStatPane



// Function at 0041b0b0

undefined4 TStatPane::meth_0x41b0b0_TArea_In(undefined4 *param_1)

{
  dword *this_00;
  undefined4 uVar1;
  cls_0x41c720 *this_01;
  int iVar2;
  int iVar3;
  dword in_stack_00000008;
  undefined4 uVar4;
  
  if ((this->expanded & 1) != 0) {
    return 1;
  }
  if ((this->startline == 0xffffffff) || (in_stack_00000008 == this->startline)) {
    this_00 = &this->mbr_0x48;
    iVar3 = 0;
    uVar1 = *param_1;
    if (0 < *(short *)this_00) {
      do {
        uVar4 = uVar1;
        this_01 = (cls_0x41c720 *)cls_0x41c780::meth_0x41c780((cls_0x41c780 *)this_00);
        iVar2 = cls_0x41c720::meth_0x41c720(this_01,uVar4);
        if (iVar2 != 0) {
          return 1;
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 < *(short *)this_00);
    }
  }
  return 0;
}



