#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x578e40



// Function at 00578e40

undefined4 cls_0x578e40::meth_0x578e40()

{
  int iVar1;
  undefined4 in_stack_00000004;
  undefined4 in_stack_00000008;
  undefined4 in_stack_0000000c;
  int in_stack_00000010;
  
  WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
  iVar1 = (**(code **)(*(int *)this->mbr_0x0 + 0xc4))
                    ((int *)this->mbr_0x0,this->mbr_0x14c,in_stack_00000004,
                     in_stack_00000010 != 0 | 0x600,in_stack_00000008,in_stack_0000000c,0,1000,0,0);
  ReleaseMutex((HANDLE)this->mbr_0x12c);
  if ((iVar1 != 0) && (iVar1 != -0x7ffffff6)) {
    return 0;
  }
  return 1;
}



// Function at 00579800

int cls_0x578e40::meth_0x579800()

{
  undefined *puVar1;
  void *pvVar2;
  int iVar3;
  undefined *puVar4;
  int iVar5;
  undefined4 in_stack_00000004;
  dword dStack_5c;
  int iStack_58;
  undefined4 uStack_54;
  dword dStack_50;
  undefined uStack_32;
  undefined uStack_31;
  undefined local_30 [4];
  dword dStack_2c;
  dword local_28;
  undefined *local_24 [2];
  undefined *local_1c;
  void *pvStack_c;
  undefined *puStack_8;
  dword local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2702;
  pvStack_c = ExceptionList;
  dStack_50 = 0x57982e;
  ExceptionList = &pvStack_c;
  cls_0x588410::cls_0x588410((cls_0x588410 *)&local_28,local_30);
  puVar4 = local_1c + 1;
  local_4 = 0;
  puVar1 = local_1c;
  local_1c = puVar4;
  if (local_24[0] < puVar4) {
    puVar1 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&local_28);
  }
  *puVar1 = 1;
  pvVar2 = (void *)GetTickCount();
  puVar4 = local_1c + 4;
  puVar1 = local_1c;
  local_1c = puVar4;
  if (local_24[0] < puVar4) {
    puVar1 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&local_28);
  }
  uStack_32 = (undefined)((uint)pvVar2 >> 0x10);
  *puVar1 = (char)pvVar2;
  uStack_31 = (undefined)((uint)pvVar2 >> 0x18);
  puVar1[1] = (char)((uint)pvVar2 >> 8);
  puVar1[2] = uStack_32;
  puVar1[3] = uStack_31;
  iVar5 = (int)local_1c - local_28;
  dStack_50 = 0x5798ad;
  WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
  dStack_50 = 0;
  uStack_54 = 1;
  dStack_5c = local_28;
  iStack_58 = iVar5;
  iVar5 = (**(code **)(*(int *)this->mbr_0x0 + 0xc4))
                    ((int *)this->mbr_0x0,this->mbr_0x14c,in_stack_00000004,0x600);
  ReleaseMutex((HANDLE)this->mbr_0x12c);
  puVar1 = local_24[0];
  if (iVar5 == -0x7fffbfff) {
    iVar5 = meth_0x578e40(this);
    if (iVar5 == 0) goto LAB_00579976;
  }
  else {
    if ((iVar5 != 0) && (iVar5 != -0x7ffffff6)) {
      iVar5 = 0;
      goto LAB_00579976;
    }
    iVar5 = 1;
  }
  WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
  local_24[0] = (undefined *)0x0;
  dStack_5c = 4;
  iVar3 = (**(code **)(*(int *)this->mbr_0x0 + 0x50))
                    ((int *)this->mbr_0x0,puVar1,local_24,&dStack_5c,1);
  puVar4 = local_24[0];
  if (iVar3 != 0) {
    puVar4 = (undefined *)FUN_00570ad0((int)puVar1);
  }
  if (puVar4 != (undefined *)0x0) {
    *(int *)(puVar4 + 0x2c) = *(int *)(puVar4 + 0x2c) + 1;
  }
  ReleaseMutex((HANDLE)this->mbr_0x12c);
LAB_00579976:
  dStack_2c = 0xffffffff;
  cls_0x588410::meth_0x588480((cls_0x588410 *)&dStack_50);
  ExceptionList = pvVar2;
  return iVar5;
}



