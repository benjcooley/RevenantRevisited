#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4dac30



// Function at 004dac30

undefined4 cls_0x4dac30::meth_0x4dac30()

{
  char cVar1;
  uint uVar2;
  uint uVar3;
  char *pcVar4;
  char *in_stack_00000004;
  
  uVar3 = 0xffffffff;
  pcVar4 = in_stack_00000004;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar4;
    pcVar4 = pcVar4 + 1;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  uVar2 = FUN_0059a600((byte *)&this->mbr_0x4,in_stack_00000004,uVar3 - 1);
  if (((uVar2 == 0) && ((&this->field_0x3)[uVar3] == 'r')) &&
     (*(char *)((int)&this->mbr_0x4 + uVar3) == '\0')) {
    return 1;
  }
  return 0;
}



// Function at 004dac80

undefined4 cls_0x4dac30::meth_0x4dac80()

{
  char cVar1;
  uint uVar2;
  uint uVar3;
  char *pcVar4;
  char *in_stack_00000004;
  
  uVar3 = 0xffffffff;
  pcVar4 = in_stack_00000004;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar4;
    pcVar4 = pcVar4 + 1;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  uVar2 = FUN_0059a600((byte *)&this->mbr_0x4,in_stack_00000004,uVar3 - 1);
  if (((uVar2 == 0) && ((&this->field_0x3)[uVar3] == 'l')) &&
     (*(char *)((int)&this->mbr_0x4 + uVar3) == '\0')) {
    return 1;
  }
  return 0;
}



