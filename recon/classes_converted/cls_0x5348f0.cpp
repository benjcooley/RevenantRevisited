#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5348f0



// Function at 005348f0

void cls_0x5348f0::meth_0x5348f0()

{
  int *piVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  dword dVar5;
  dword *pdVar6;
  dword *pdVar7;
  
  if (this->mbr_0x20 == 0xffffd8f0) {
    return;
  }
  iVar3 = 0;
  if (-1 < (int)this->mbr_0x14) {
    if (this->mbr_0x14 == 0) {
      iVar4 = 0;
      this->mbr_0x50 = 1;
      this->mbr_0x58 = 0;
      if (0 < (int)this->mbr_0x5c) {
        pdVar6 = &this->mbr_0x100;
        do {
          if (*pdVar6 != 0) {
            cls_0x4367d0::meth_0x4367d0((cls_0x4367d0 *)this->mbr_0x0);
            *pdVar6 = 0;
          }
          iVar4 = iVar4 + 1;
          pdVar6 = pdVar6 + 1;
        } while (iVar4 < (int)this->mbr_0x5c);
      }
    }
    this->mbr_0x14 = this->mbr_0x14 - 1;
  }
  dVar2 = this->mbr_0x30;
  if ((this->mbr_0x20 != dVar2) || (this->mbr_0x24 != this->mbr_0x34)) {
    dVar5 = this->mbr_0x38 + this->mbr_0x40;
    this->mbr_0x38 = dVar5;
    dVar5 = (int)dVar5 >> 0x10;
    if ((int)this->mbr_0x20 < (int)dVar2) {
      if ((int)dVar5 < (int)dVar2) {
LAB_0053497d:
        dVar2 = dVar5;
      }
    }
    else if ((int)dVar2 < (int)dVar5) goto LAB_0053497d;
    this->mbr_0x20 = dVar2;
    dVar2 = this->mbr_0x34;
    dVar5 = this->mbr_0x3c + this->mbr_0x44;
    this->mbr_0x3c = dVar5;
    dVar5 = (int)dVar5 >> 0x10;
    if ((int)this->mbr_0x24 < (int)dVar2) {
      if ((int)dVar5 < (int)dVar2) {
LAB_005349a9:
        dVar2 = dVar5;
      }
    }
    else if ((int)dVar2 < (int)dVar5) goto LAB_005349a9;
    this->mbr_0x24 = dVar2;
  }
  dVar2 = this->mbr_0x54;
  if ((int)dVar2 < (int)this->mbr_0x58) {
    dVar2 = dVar2 + 1;
  }
  else {
    if ((int)dVar2 <= (int)this->mbr_0x58) goto LAB_005349c1;
    dVar2 = dVar2 - 1;
  }
  this->mbr_0x54 = dVar2;
LAB_005349c1:
  if (0 < (int)this->mbr_0x5c) {
    pdVar6 = &this->mbr_0x80;
    pdVar7 = &this->mbr_0x120;
    do {
      if (pdVar7[-8] != 0) {
        if ((*(byte *)(pdVar7[-8] + 0x14) & 8) == 0) {
          pdVar7[8] = 0;
        }
        else {
          pdVar7[8] = 8;
        }
        dVar2 = *pdVar7;
        if ((int)dVar2 < (int)pdVar7[8]) {
          dVar2 = dVar2 + 1;
LAB_00534a00:
          *pdVar7 = dVar2;
        }
        else if ((int)pdVar7[8] < (int)dVar2) {
          dVar2 = dVar2 - 1;
          goto LAB_00534a00;
        }
        piVar1 = (int *)pdVar7[-8];
        (**(code **)(*piVar1 + 0x28))
                  (this->mbr_0x18 + *pdVar6 + this->mbr_0x20,
                   pdVar6[1] + this->mbr_0x1c + this->mbr_0x24,piVar1[0x1a],piVar1[0x1b]);
      }
      iVar3 = iVar3 + 1;
      pdVar7 = pdVar7 + 1;
      pdVar6 = pdVar6 + 4;
    } while (iVar3 < (int)this->mbr_0x5c);
  }
  return;
}



