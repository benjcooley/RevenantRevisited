#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5726b0



// Function at 005726b0

cls_0x5726b0 * __thiscall cls_0x5726b0::~cls_0x5726b0(cls_0x5726b0 *this)

{
  cls_0x5726b0 *hObject;
  
  this->mbr_0x0 = (dword)&cls_0x5b9f0c__vftable_5b9f0c_005b9f0c;
  if ((LPCVOID)this->mbr_0x38 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x38);
  }
  hObject = (cls_0x5726b0 *)this->mbr_0x10;
  if (hObject != (cls_0x5726b0 *)0x0) {
    hObject = (cls_0x5726b0 *)CloseHandle(hObject);
  }
  if ((LPCVOID)this->mbr_0x34 != (LPCVOID)0x0) {
    hObject = (cls_0x5726b0 *)FUN_004830f0((LPCVOID)this->mbr_0x34);
  }
  return hObject;
}



