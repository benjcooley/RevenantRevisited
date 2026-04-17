#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x579bb0



// Function at 00579bb0

void cls_0x579bb0::meth_0x579bb0()

{
  int iVar1;
  bool bVar2;
  cls_0x579bb0 *local_4;
  
  bVar2 = DAT_00676828 != 0;
  this->mbr_0x138 = 0;
  if (bVar2) {
    local_4 = this;
    if (this->mbr_0x144 != 0) {
      cls_0x5756d0::meth_0x578210((cls_0x5756d0 *)&DAT_00676738);
      return;
    }
    while ((iVar1 = (**(code **)(*(int *)this->mbr_0x0 + 200))
                              ((int *)this->mbr_0x0,0,0,1,0,&local_4), iVar1 == 0 &&
           ((cls_0x579bb0 *)0x708 < local_4))) {
      Sleep(0x50);
      this->mbr_0x138 = 1;
    }
    cls_0x5756d0::meth_0x578b80((cls_0x5756d0 *)&DAT_00676738);
    cls_0x5ba0a0::virt_meth_0x57c7c0((cls_0x5ba0a0 *)&UNK_006768d5.field_0x5b);
    cls_0x5ba0a0::virt_meth_0x57c7c0((cls_0x5ba0a0 *)&UNK_00676268.field_0x400);
    cls_0x57dd50::meth_0x57dd50((cls_0x57dd50 *)&UNK_00676e04.field_0x4);
    iVar1 = ReleaseMutex((HANDLE)this->mbr_0x12c);
    while (iVar1 != 0) {
      iVar1 = ReleaseMutex((HANDLE)this->mbr_0x12c);
    }
  }
  return;
}



