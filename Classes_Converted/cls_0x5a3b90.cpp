#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a3b90



// Function at 0042b090

cls_0x5a3b90::cls_0x5a3b90 *this,undefined4 param_1,undefined4 param_2)

{
  dword *pdVar1;
  dword dVar2;
  int iVar3;
  int in_stack_0000000c;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ca3a;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a3ab8::cls_0x5a3ab8(&this->cls_0x5a3ab8,param_1,2);
  local_4 = 0;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3b90__vftable_5a3b90_005a3b90;
  (this->cls_0x5a3ab8).mbr_0x14 = (this->cls_0x5a3ab8).mbr_0x14 | 1;
  if (in_stack_0000000c == 0) {
    in_stack_0000000c = 0x6560c8;
  }
  iVar3 = cls_0x42aed0::meth_0x42aed0((cls_0x42aed0 *)&this->mbr_0x90,in_stack_0000000c,param_1);
  if (iVar3 != 0) {
    pdVar1 = (dword *)this->mbr_0x90;
    if (pdVar1 != (dword *)0x0) {
      if ((this->cls_0x5a3ab8).mbr_0x60 == 0xffffd8f0) {
        dVar2 = pdVar1[3];
        (this->cls_0x5a3ab8).mbr_0x60 = -pdVar1[2];
        (this->cls_0x5a3ab8).mbr_0x64 = -dVar2;
      }
      if ((this->cls_0x5a3ab8).mbr_0x68 == 0xffffd8f0) {
        dVar2 = pdVar1[1];
        (this->cls_0x5a3ab8).mbr_0x68 = *pdVar1;
        (this->cls_0x5a3ab8).mbr_0x6c = dVar2;
      }
    }
    (this->cls_0x5a3ab8).mbr_0x14 = (this->cls_0x5a3ab8).mbr_0x14 & 0xfffffffe;
  }
  ExceptionList = local_c;
  return this;
}



// Function at 00437f80

cls_0x5a3b90 * cls_0x5a3b90::virt_meth_0x437f80()

{
  byte in_stack_00000004;
  
  ~cls_0x5a3b90(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00437fa0

cls_0x5a3b90 * __thiscall cls_0x5a3b90::~cls_0x5a3b90(cls_0x5a3b90 *this)

{
  cls_0x5a3b90 *pcVar1;
  
  pcVar1 = (cls_0x5a3b90 *)(this->cls_0x5a3ab8).mbr_0x5c;
  (this->cls_0x5a3ab8).vftptr_0x0 = &cls_0x5a3ab8__vftable_5a3ab8_005a3ab8;
  if (pcVar1 != (cls_0x5a3b90 *)0x0) {
    pcVar1 = (cls_0x5a3b90 *)FUN_00482f80(pcVar1);
  }
  (this->cls_0x5a3ab8).mbr_0x5c = 0;
  return pcVar1;
}



// Function at 00437fd0

cls_0x5a3bfc * cls_0x5a3b90::virt_meth_0x437fd0(undefined4 param_1)

{
  cls_0x5a3bfc *pcVar1;
  undefined4 in_stack_00000008;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059cc7b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  pcVar1 = (cls_0x5a3bfc *)FUN_00482fb0(0xe0);
  local_4 = 0;
  if (pcVar1 != (cls_0x5a3bfc *)0x0) {
    pcVar1 = cls_0x5a3bfc::cls_0x5a3bfc(pcVar1,param_1,in_stack_00000008);
    ExceptionList = local_c;
    return pcVar1;
  }
  ExceptionList = local_c;
  return (cls_0x5a3bfc *)0x0;
}



