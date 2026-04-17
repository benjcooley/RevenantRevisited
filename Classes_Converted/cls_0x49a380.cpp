#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x49a380



// Function at 0049a380

void cls_0x49a380::meth_0x49a380()

{
  dword dVar1;
  uint uVar2;
  int iVar3;
  dword dVar4;
  
  dVar4 = this->mbr_0x10;
  if (dVar4 == 0) {
    if (this->mbr_0x14 != 0) {
      this->mbr_0x10 = this->mbr_0x14;
      this->mbr_0x14 = 0;
    }
    goto LAB_0049a408;
  }
  if ((int)dVar4 < 1) {
    uVar2 = dVar4 + this->mbr_0xc;
    uVar2 = uVar2 & ((int)uVar2 < 1) - 1;
    this->mbr_0xc = uVar2;
    if (uVar2 == 0) goto LAB_0049a3c6;
  }
  else {
    dVar4 = this->mbr_0xc + dVar4;
    dVar1 = this->mbr_0x4;
    if ((int)dVar1 <= (int)dVar4) {
      dVar4 = dVar1;
    }
    this->mbr_0xc = dVar4;
    if (dVar4 == dVar1) {
LAB_0049a3c6:
      this->mbr_0x10 = 0;
    }
  }
  dVar4 = this->mbr_0xc;
  if (this->mbr_0x0 != 0) {
    if ((int)dVar4 < 0x60) {
      if ((int)dVar4 < 0) {
        dVar4 = 0;
      }
    }
    else {
      dVar4 = 0x60;
    }
    if (this->mbr_0x8 != 0) {
      _AIL_redbook_set_volume_8(this->mbr_0x0,dVar4);
      dVar4 = _AIL_redbook_volume_4(this->mbr_0x0);
      this->mbr_0xc = dVar4;
    }
  }
LAB_0049a408:
  if (0 < (int)this->mbr_0x40) {
    this->mbr_0x40 = this->mbr_0x40 - 1;
  }
  if ((this->mbr_0x40 == 0) && (this->mbr_0x40 = 1000, this->mbr_0x10 == 0)) {
    if (this->mbr_0x0 == 0) {
      this->mbr_0x8 = 0;
    }
    else if (this->mbr_0x8 != 0) {
      iVar3 = _AIL_redbook_status_4(this->mbr_0x0);
      this->mbr_0x8 = (uint)(iVar3 == 1);
    }
    if (((this->mbr_0x8 != 0) && (iVar3 = _AIL_redbook_volume_4(this->mbr_0x0), iVar3 == 0)) &&
       (this->mbr_0x0 != 0)) {
      if ((this->mbr_0x8 != 0) && (iVar3 = _AIL_redbook_status_4(this->mbr_0x0), iVar3 == 1)) {
        _AIL_redbook_stop_4(this->mbr_0x0);
      }
      this->mbr_0x8 = 0;
    }
  }
  return;
}



