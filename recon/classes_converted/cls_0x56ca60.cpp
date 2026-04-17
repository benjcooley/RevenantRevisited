#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x56ca60



// Function at 0056ca60

void cls_0x56ca60::meth_0x56ca60()

{
  undefined4 *puVar1;
  int iVar2;
  int in_stack_00000004;
  
  if ((LPCVOID)this->mbr_0x0 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x0);
  }
  puVar1 = FUN_00482fb0(0x20000);
  this->mbr_0x0 = (dword)puVar1;
  if (in_stack_00000004 == 0x10000) {
    iVar2 = 0;
    do {
      *(ushort *)puVar1 =
           ((((ushort)(iVar2 >> 7) & 0x1e) * 0x20 + ((ushort)(iVar2 >> 3) & 0x1e)) * 0x20 +
           ((ushort)iVar2 & 0xf)) * 2;
      iVar2 = iVar2 + 1;
      puVar1 = (undefined4 *)((int)puVar1 + 2);
    } while (iVar2 < 0x10000);
    return;
  }
  iVar2 = 0;
  do {
    *(ushort *)puVar1 =
         ((((ushort)(iVar2 >> 7) & 0x1e) * 0x20 + ((ushort)(iVar2 >> 3) & 0x1e)) * 0x10 +
         ((ushort)iVar2 & 0xf)) * 2;
    iVar2 = iVar2 + 1;
    puVar1 = (undefined4 *)((int)puVar1 + 2);
  } while (iVar2 < 0x10000);
  return;
}



