#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5ba088



// Function at 00579530

undefined4 cls_0x5ba088::virt_meth_0x579530(undefined4 param_1)

{
  undefined uVar1;
  undefined uVar2;
  undefined uVar3;
  undefined uVar4;
  HANDLE hHandle;
  undefined *puVar5;
  int iVar6;
  undefined *puVar7;
  void *unaff_EBX;
  int iVar8;
  cls_0x588410 *in_stack_00000008;
  dword dStack_50;
  dword dStack_4c;
  undefined local_30 [4];
  dword dStack_2c;
  dword local_28;
  undefined *local_24;
  undefined *local_1c;
  void *pvStack_c;
  undefined *puStack_8;
  dword local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a26f0;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  cls_0x588410::cls_0x588410((cls_0x588410 *)&local_28,local_30);
  puVar7 = local_1c + 1;
  local_4 = 0;
  puVar5 = local_1c;
  local_1c = puVar7;
  if (local_24 < puVar7) {
    puVar5 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&local_28);
  }
  *puVar5 = 2;
  puVar5 = (undefined *)in_stack_00000008->mbr_0xc;
  in_stack_00000008->mbr_0xc = (dword)(puVar5 + 4);
  if ((undefined *)in_stack_00000008->mbr_0x4 < puVar5 + 4) {
    puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(in_stack_00000008);
  }
  uVar1 = *puVar5;
  uVar2 = puVar5[1];
  uVar3 = puVar5[2];
  uVar4 = puVar5[3];
  puVar7 = local_1c + 4;
  puVar5 = local_1c;
  local_1c = puVar7;
  if (local_24 < puVar7) {
    puVar5 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&local_28);
  }
  *puVar5 = uVar1;
  puVar5[1] = uVar2;
  puVar5[2] = uVar3;
  hHandle = DAT_00676864;
  puVar5[3] = uVar4;
  iVar8 = (int)local_1c - local_28;
  dStack_4c = 0x579607;
  WaitForSingleObject(hHandle,0xffffffff);
  dStack_4c = 0;
  dStack_50 = 1;
  iVar6 = (**(code **)(*DAT_00676738 + 0xc4))
                    (DAT_00676738,DAT_00676884,param_1,0x600,local_28,iVar8);
  ReleaseMutex(DAT_00676864);
  if (iVar6 == -0x7fffbfff) {
    WaitForSingleObject(DAT_00676864,0xffffffff);
    (**(code **)(*DAT_00676738 + 0xc4))
              (DAT_00676738,DAT_00676884,local_24,0x600,local_28,iVar8,0,1000,0,0);
    ReleaseMutex(DAT_00676864);
  }
  dStack_2c = 0xffffffff;
  cls_0x588410::meth_0x588480((cls_0x588410 *)&dStack_50);
  ExceptionList = unaff_EBX;
  return 1;
}



// Function at 0057baf0

cls_0x5ba088 * cls_0x5ba088::virt_meth_0x57baf0()

{
  byte in_stack_00000004;
  
  cls_0x5ba0a0::~cls_0x5ba0a0((cls_0x5ba0a0 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



