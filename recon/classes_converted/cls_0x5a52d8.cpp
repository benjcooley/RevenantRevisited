#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a52d8



// Function at 004742e0

cls_0x5a52d8::cls_0x5a52d8 *this,dword param_1,undefined2 param_2,undefined4 param_3)

{
  cls_0x41c7f0 *this_00;
  undefined4 *puVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  undefined2 in_stack_0000000a;
  dword in_stack_00000010;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d2e0;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  *(undefined2 *)&this->mbr_0x14 = 0;
  *(undefined2 *)((int)&this->mbr_0x14 + 2) = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x1c = 0;
  this->mbr_0x1e = 0;
  this->mbr_0x20 = 0;
  this_00 = &this->cls_0x41c7f0;
  local_4._0_1_ = 1;
  local_4._1_3_ = 0;
  cls_0x41c7f0::cls_0x41c7f0(this_00,0x40);
  local_4._0_1_ = 2;
  puVar1 = FUN_00482fb0(0x28);
  if (puVar1 == (undefined4 *)0x0) {
    puVar1 = (undefined4 *)0x0;
  }
  else {
    *(undefined2 *)(puVar1 + 3) = 0;
    *(undefined2 *)((int)puVar1 + 0xe) = 0;
    puVar1[4] = 0;
    local_4._0_1_ = 4;
    cls_0x478320::cls_0x478320((cls_0x478320 *)(puVar1 + 5));
    local_4._0_1_ = 5;
    *puVar1 = 0;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[7] = 0;
    *(undefined *)((int)puVar1 + 0x22) = 0;
    *(undefined2 *)(puVar1 + 8) = 0;
    cls_0x478320::meth_0x478340((cls_0x478320 *)(puVar1 + 3));
    cls_0x478320::meth_0x478340((cls_0x478320 *)(puVar1 + 5));
    puVar1[9] = 0;
    *(undefined *)((int)puVar1 + 0x23) = 0;
  }
  (this->cls_0x41c7f0).mbr_0x14 = (dword)puVar1;
  for (iVar4 = 10; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  }
  this->mbr_0x4 = param_1;
  local_4 = CONCAT31(local_4._1_3_,6);
  this->mbr_0x10 = in_stack_00000010;
  iVar4 = DAT_0065a258;
  this->vftptr_0x0 = &cls_0x5a52d8__vftable_5a52d8_005a52d8;
  this->mbr_0x8 = _param_2;
  this->mbr_0xc = (word)param_3;
  (&DAT_0065a148)[_param_2] = this;
  if (iVar4 <= (int)_param_2) {
    DAT_0065a258 = _param_2 + 1;
  }
  dVar2 = this_00->mbr_0x0;
  iVar4 = 0;
  if (0 < (int)dVar2) {
    do {
      if ((-1 < iVar4) && (iVar4 < (int)dVar2)) {
        iVar3 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)this_00);
        FUN_00446500(*(uint *)(iVar3 + 8));
        cls_0x45f7c0::meth_0x478680((cls_0x45f7c0 *)this_00);
        DAT_0065a25c = 1;
      }
      dVar2 = this_00->mbr_0x0;
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)dVar2);
  }
  cls_0x4782a0::meth_0x4782a0((cls_0x4782a0 *)&this->mbr_0x14);
  if ((LPCVOID)this->mbr_0x20 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x20);
  }
  this->mbr_0x1c = 0;
  this->mbr_0x1e = 0;
  dVar2 = this_00->mbr_0x0;
  iVar4 = 0;
  this->mbr_0x20 = 0;
  if (0 < (int)dVar2) {
    do {
      cls_0x45f7c0::meth_0x478680((cls_0x45f7c0 *)this_00);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)this_00->mbr_0x0);
  }
  this_00->mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  ExceptionList = local_c;
  return this;
}



// Function at 00474480

cls_0x5a52d8 * __thiscall cls_0x5a52d8::~cls_0x5a52d8(cls_0x5a52d8 *this)

{
  cls_0x41c7f0 *this_00;
  LPCVOID *ppvVar1;
  dword dVar2;
  int iVar3;
  cls_0x5a52d8 *pcVar4;
  int iVar5;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d313;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a52d8__vftable_5a52d8_005a52d8;
  dVar2 = (this->cls_0x41c7f0).mbr_0x0;
  this_00 = &this->cls_0x41c7f0;
  iVar5 = 0;
  local_4 = 2;
  if (0 < (int)dVar2) {
    do {
      if ((-1 < iVar5) && (iVar5 < (int)dVar2)) {
        iVar3 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)this_00);
        FUN_00446500(*(uint *)(iVar3 + 8));
        cls_0x45f7c0::meth_0x478680((cls_0x45f7c0 *)this_00);
        DAT_0065a25c = 1;
      }
      dVar2 = this_00->mbr_0x0;
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)dVar2);
  }
  cls_0x4782a0::meth_0x4782a0((cls_0x4782a0 *)&this->mbr_0x14);
  if ((LPCVOID)this->mbr_0x20 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x20);
  }
  this->mbr_0x1c = 0;
  this->mbr_0x1e = 0;
  dVar2 = this_00->mbr_0x0;
  iVar5 = 0;
  this->mbr_0x20 = 0;
  if (0 < (int)dVar2) {
    do {
      cls_0x45f7c0::meth_0x478680((cls_0x45f7c0 *)this_00);
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)this_00->mbr_0x0);
  }
  this_00->mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  iVar5 = 0;
  local_4 = CONCAT31(local_4._1_3_,3);
  if (0 < (int)this_00->mbr_0x0) {
    do {
      cls_0x45f7c0::meth_0x478680((cls_0x45f7c0 *)this_00);
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)this_00->mbr_0x0);
  }
  ppvVar1 = (LPCVOID *)(this->cls_0x41c7f0).mbr_0x14;
  this_00->mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  if (ppvVar1 != (LPCVOID *)0x0) {
    FUN_00477fe0(ppvVar1);
    FUN_004830f0(ppvVar1);
  }
  local_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  local_4 = (uint)local_4._1_3_ << 8;
  if ((LPCVOID)this->mbr_0x20 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x20);
  }
  pcVar4 = (cls_0x5a52d8 *)this->mbr_0x18;
  local_4 = 0xffffffff;
  if (pcVar4 != (cls_0x5a52d8 *)0x0) {
    pcVar4 = (cls_0x5a52d8 *)FUN_004830f0(pcVar4);
  }
  ExceptionList = local_c;
  return pcVar4;
}



