#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5ba038



// Function at 00579c70

cls_0x5ba038::cls_0x5ba038()

{
  HANDLE pvVar1;
  DWORD DVar2;
  
  cls_0x5ba0a0::cls_0x5ba0a0(&this->cls_0x5ba0a0);
  (this->cls_0x5ba0a0).vftptr_0x0 =
       (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5ba038__vftable_5ba038_005ba038;
  pvVar1 = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,0,0,(LPCSTR)0x0);
  (this->cls_0x5ba0a0).cls_0x588410.mbr_0x24 = (dword)pvVar1;
  (this->cls_0x5ba0a0).cls_0x588410.mbr_0x28 = 0;
  DVar2 = GetTickCount();
  *(short *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c = (short)DVar2;
  *(undefined4 *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x30 = 0;
  (this->cls_0x5ba0a0).cls_0x588410.mbr_0x34 = 1;
  return this;
}



// Function at 00579cc0

cls_0x5ba038 * __thiscall cls_0x5ba038::~cls_0x5ba038(cls_0x5ba038 *this)

{
  HANDLE hObject;
  cls_0x5ba0a0 *pcVar1;
  
  hObject = (HANDLE)(this->cls_0x5ba0a0).cls_0x588410.mbr_0x24;
  (this->cls_0x5ba0a0).vftptr_0x0 =
       (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5ba038__vftable_5ba038_005ba038;
  if (hObject != (HANDLE)0x0) {
    CloseHandle(hObject);
  }
  pcVar1 = cls_0x5ba0a0::~cls_0x5ba0a0(&this->cls_0x5ba0a0);
  return (cls_0x5ba038 *)pcVar1;
}



// Function at 0057bbf0

cls_0x5ba038 * cls_0x5ba038::virt_meth_0x57bbf0()

{
  byte in_stack_00000004;
  
  ~cls_0x5ba038(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0057c8f0

void cls_0x5ba038::meth_0x57c8f0()

{
  WaitForSingleObject((HANDLE)(this->cls_0x5ba0a0).cls_0x588410.mbr_0x1c,0xffffffff);
  return;
}



// Function at 0057c900

void cls_0x5ba038::meth_0x57c900()

{
  ReleaseMutex((HANDLE)(this->cls_0x5ba0a0).cls_0x588410.mbr_0x1c);
  return;
}



