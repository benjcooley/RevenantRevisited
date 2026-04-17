#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x56fc70



// Function at 0056fc70

uint cls_0x56fc70::meth_0x56fc70(byte *param_1, undefined param_2)

{
  byte bVar1;
  byte bVar2;
  uint uVar3;
  uint uVar4;
  undefined3 in_stack_00000009;
  char in_stack_0000000c;
  
  if (in_stack_0000000c != '\0') {
    this->mbr_0x0 = 0xffffffff;
  }
  if (7 < (int)_param_2) {
    uVar4 = _param_2 >> 3;
    _param_2 = _param_2 + uVar4 * -8;
    do {
      bVar1 = param_1[1];
      uVar3 = (&DAT_00676264)[this->mbr_0x0 & 0xff ^ (uint)*param_1] ^ this->mbr_0x0 >> 8;
      this->mbr_0x0 = uVar3;
      bVar2 = param_1[2];
      uVar3 = (&DAT_00676264)[uVar3 & 0xff ^ (uint)bVar1] ^ uVar3 >> 8;
      this->mbr_0x0 = uVar3;
      bVar1 = param_1[3];
      uVar3 = (&DAT_00676264)[uVar3 & 0xff ^ (uint)bVar2] ^ uVar3 >> 8;
      this->mbr_0x0 = uVar3;
      bVar2 = param_1[4];
      uVar3 = (&DAT_00676264)[uVar3 & 0xff ^ (uint)bVar1] ^ uVar3 >> 8;
      this->mbr_0x0 = uVar3;
      bVar1 = param_1[5];
      uVar3 = (&DAT_00676264)[uVar3 & 0xff ^ (uint)bVar2] ^ uVar3 >> 8;
      this->mbr_0x0 = uVar3;
      bVar2 = param_1[6];
      uVar3 = (&DAT_00676264)[uVar3 & 0xff ^ (uint)bVar1] ^ uVar3 >> 8;
      this->mbr_0x0 = uVar3;
      bVar1 = param_1[7];
      uVar3 = (&DAT_00676264)[uVar3 & 0xff ^ (uint)bVar2] ^ uVar3 >> 8;
      this->mbr_0x0 = uVar3;
      param_1 = param_1 + 8;
      this->mbr_0x0 = (&DAT_00676264)[uVar3 & 0xff ^ (uint)bVar1] ^ uVar3 >> 8;
      uVar4 = uVar4 - 1;
    } while (uVar4 != 0);
  }
  for (; _param_2 != 0; _param_2 = _param_2 - 1) {
    bVar1 = *param_1;
    param_1 = param_1 + 1;
    this->mbr_0x0 = (&DAT_00676264)[this->mbr_0x0 & 0xff ^ (uint)bVar1] ^ this->mbr_0x0 >> 8;
  }
  return ~this->mbr_0x0;
}



