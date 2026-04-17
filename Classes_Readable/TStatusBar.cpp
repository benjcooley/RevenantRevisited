#include "TStatusBar.h"

// Decompiled methods and structure for class: TStatusBar



// Function at 00579bb0

void TStatusBar::meth_0x579bb0()

{
  int iVar1;
  bool bVar2;
  TStatusBar *local_4;
  
  bVar2 = DAT_00676828 != 0;
  this->animating = 0;
  if (bVar2) {
    local_4 = this;
    if (this->mbr_0x144 != 0) {
      cls_0x5756d0::meth_0x578210((cls_0x5756d0 *)&DAT_00676738);
      return;
    }
    while ((iVar1 = (**(code **)(*(int *)this->level + 200))
                              ((int *)this->level,0,0,1,0,&local_4), iVar1 == 0 &&
           ((TStatusBar *)0x708 < local_4))) {
      Sleep(0x50);
      this->animating = 1;
    }
    cls_0x5756d0::meth_0x578b80((cls_0x5756d0 *)&DAT_00676738);
    cls_0x5ba0a0::virt_meth_0x57c7c0((cls_0x5ba0a0 *)&UNK_006768d5.field_0x5b);
    cls_0x5ba0a0::virt_meth_0x57c7c0((cls_0x5ba0a0 *)&UNK_00676268.field_0x400);
    cls_0x57dd50::meth_0x57dd50((cls_0x57dd50 *)&UNK_00676e04.field_0x4);
    iVar1 = ReleaseMutex((HANDLE)this->targetlevel);
    while (iVar1 != 0) {
      iVar1 = ReleaseMutex((HANDLE)this->targetlevel);
    }
  }
  return;
}



