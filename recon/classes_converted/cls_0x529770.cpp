#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x529770



// Function at 00529770

void cls_0x529770::meth_0x529770()

{
  undefined2 *puVar1;
  cls_0x49ce00 *pcVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  int iVar5;
  cls_0x49ce00 *in_stack_00000004;
  
  pcVar2 = in_stack_00000004;
  cls_0x49ce00::cls_0x49ce00(in_stack_00000004);
  iVar5 = *(int *)in_stack_00000004->mbr_0x4;
  in_stack_00000004->mbr_0x4 = (dword)((int *)in_stack_00000004->mbr_0x4 + 1);
  in_stack_00000004 = (cls_0x49ce00 *)iVar5;
  if (0 < iVar5) {
    do {
      puVar3 = FUN_00482fb0(0xc);
      if (puVar3 == (undefined4 *)0x0) {
        puVar3 = (undefined4 *)0x0;
      }
      else {
        *puVar3 = 0xffffffff;
        puVar3[2] = 0;
        puVar3[1] = 0;
      }
      puVar4 = (undefined4 *)pcVar2->mbr_0x4;
      iVar5 = puVar4[1];
      *puVar3 = *puVar4;
      pcVar2->mbr_0x4 = (dword)(puVar4 + 2);
      puVar3[1] = iVar5;
      puVar4 = FUN_00482ef0(iVar5 << 1);
      iVar5 = puVar3[1];
      puVar3[2] = puVar4;
      if (0 < iVar5) {
        do {
          puVar1 = (undefined2 *)pcVar2->mbr_0x4;
          *(undefined2 *)puVar4 = *puVar1;
          iVar5 = iVar5 + -1;
          pcVar2->mbr_0x4 = (dword)(puVar1 + 1);
          puVar4 = (undefined4 *)((int)puVar4 + 2);
        } while (iVar5 != 0);
      }
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x24);
      in_stack_00000004 = (cls_0x49ce00 *)((int)in_stack_00000004 + -1);
    } while (in_stack_00000004 != (cls_0x49ce00 *)0x0);
  }
  return;
}



