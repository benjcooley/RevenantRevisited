#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4f79d0



// Function at 004f79d0

cls_0x4f79d0 * __thiscall cls_0x4f79d0::~cls_0x4f79d0(cls_0x4f79d0 *this)

{
  cls_0x4f79d0 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f788;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x4f79b0_005aa89c;
  local_4 = 0;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if ((LPCVOID)this->mbr_0x18c != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x18c);
  }
  if ((LPCVOID)this->mbr_0x190 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x190);
  }
  if ((LPCVOID)this->mbr_0x188 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x188);
  }
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x4f79d0 *)TScreen::~TScreen((TScreen *)this);
  ExceptionList = local_c;
  return pcVar1;
}



