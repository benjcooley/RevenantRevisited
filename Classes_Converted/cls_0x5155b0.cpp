#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5155b0



// Function at 005155b0

cls_0x5155b0 * __thiscall cls_0x5155b0::~cls_0x5155b0(cls_0x5155b0 *this)

{
  undefined4 uVar1;
  TScreen *pcVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0f68;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&cls_0x5b45ac__vftable_5b45ac_005b45ac;
  local_4 = 0;
  uVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
  cls_0x41c7d0::meth_0x49b8e0((cls_0x41c7d0 *)&DAT_00667548,uVar1);
  local_4 = 0xffffffff;
  pcVar2 = TScreen::~TScreen((TScreen *)this);
  ExceptionList = local_c;
  return (cls_0x5155b0 *)pcVar2;
}



