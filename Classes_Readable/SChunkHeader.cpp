#include "SChunkHeader.h"

// Decompiled methods and structure for class: SChunkHeader



// Function at 004a0d20

undefined4 SChunkHeader::meth_0x4a0d20(dword *param_1, uint param_2)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  int in_stack_0000000c;
  
  if ((((param_1 != (dword *)0x0) && (*param_1 == this->width)) && (this->height != 0)) &&
     (((param_1[1] < this->block && ((this->type & 2) == 0)) &&
      (puVar1 = *(uint **)(this->mbr_0x1a4 + param_1[1] * 4), puVar1 != (uint *)0x0)))) {
    if (in_stack_0000000c == 0) {
      uVar2 = param_2 & ((int)param_2 < 1) - 1;
      if ((int)*puVar1 <= (int)uVar2) {
        uVar2 = *puVar1;
      }
      param_1[3] = uVar2;
      return 0;
    }
    if (in_stack_0000000c == 1) {
      uVar3 = param_1[3] + param_2;
      uVar2 = *puVar1;
      if ((int)(((int)uVar3 < 1) - 1 & uVar3) < (int)uVar2) {
        param_1[3] = ((int)uVar3 < 1) - 1 & uVar3;
        return 0;
      }
    }
    else {
      if (in_stack_0000000c != 2) {
        return 0;
      }
      uVar2 = *puVar1;
      uVar3 = param_2 + uVar2;
      if ((int)(((int)uVar3 < 1) - 1 & uVar3) < (int)uVar2) {
        uVar2 = ((int)uVar3 < 1) - 1 & uVar3;
      }
    }
    param_1[3] = uVar2;
    return 0;
  }
  return 1;
}



