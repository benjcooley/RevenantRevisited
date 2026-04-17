#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x51a320



// Function at 0051a320

int cls_0x51a320::meth_0x51a320(int param_1)

{
  uint uVar1;
  int iVar2;
  int in_stack_00000008;
  
  if (param_1 + 0x22 < (int)(short)this->mbr_0x34c) {
    uVar1 = *(uint *)(this->mbr_0x350 + 4 + (param_1 + 0x22) * 8);
    if (0x1d < uVar1) {
      iVar2 = ((-1 < param_1) - 1 & 0xfffffff6) + 5;
      goto LAB_0051a39d;
    }
  }
  else {
    uVar1 = 0;
  }
  switch(uVar1) {
  default:
    iVar2 = -4;
    break;
  case 3:
  case 4:
  case 5:
  case 6:
    iVar2 = -3;
    break;
  case 7:
  case 8:
  case 9:
    iVar2 = -2;
    break;
  case 10:
  case 0xb:
    iVar2 = -1;
    break;
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    iVar2 = 0;
    break;
  case 0x10:
  case 0x11:
    iVar2 = 1;
    break;
  case 0x12:
  case 0x13:
  case 0x14:
    iVar2 = 2;
    break;
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
    iVar2 = 3;
    break;
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
    iVar2 = 4;
  }
LAB_0051a39d:
  if (in_stack_00000008 != 2) {
    if (in_stack_00000008 == 1) {
      iVar2 = iVar2 / 2;
    }
    return iVar2;
  }
  if (iVar2 < 1) {
    return (iVar2 + -1) / 2;
  }
  return (iVar2 + 1) / 2;
}



