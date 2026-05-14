#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x49a300



// Function at 0049a300

void cls_0x49a300::meth_0x49a300()

{
  int iVar1;
  dword dVar2;
  dword dVar3;
  int in_stack_00000004;
  
  this->mbr_0x8 = 0;
  if (this->mbr_0x0 != 0) {
    iVar1 = _AIL_redbook_status_4(this->mbr_0x0);
    if (iVar1 == 0) {
      this->mbr_0x3c = 0xffffffff;
    }
    else {
      dVar3 = in_stack_00000004 + 1;
      if ((int)this->mbr_0x3c < 0) {
        dVar2 = _AIL_redbook_tracks_4(this->mbr_0x0);
        this->mbr_0x3c = dVar2;
      }
      if ((0 < (int)dVar3) && ((int)dVar3 <= (int)this->mbr_0x3c)) {
        this->mbr_0x18 = dVar3;
        _AIL_redbook_track_info_16(this->mbr_0x0,dVar3,&this->mbr_0x1c,&this->mbr_0x20);
        _AIL_redbook_play_12(this->mbr_0x0,this->mbr_0x1c,this->mbr_0x20);
        this->mbr_0x8 = 1;
        return;
      }
    }
  }
  return;
}



// Function at 0049a480

int cls_0x49a300::meth_0x49a480()

{
  int iVar1;
  int in_stack_00000004;
  int iStack_8;
  int iStack_4;
  
  if (this->mbr_0x0 != 0) {
    iVar1 = _AIL_redbook_status_4(this->mbr_0x0);
    if (iVar1 != 0) {
      _AIL_redbook_track_info_16(this->mbr_0x0,in_stack_00000004 + 1,&iStack_4,&iStack_8);
    }
    return iStack_8 - iStack_4;
  }
  return 0;
}



// Function at 0049a4d0

void cls_0x49a300::meth_0x49a4d0()

{
  int iVar1;
  dword dVar2;
  dword *pdVar3;
  
  if (this->mbr_0x0 != 0) {
    iVar1 = _AIL_redbook_status_4(this->mbr_0x0);
    if (iVar1 != 0) {
      if ((int)this->mbr_0x3c < 0) {
        dVar2 = _AIL_redbook_tracks_4(this->mbr_0x0);
        this->mbr_0x3c = dVar2;
      }
      do {
        dVar2 = FUN_00483300_RandomRange(0,this->mbr_0x3c - 1);
        iVar1 = 0;
        pdVar3 = &this->mbr_0x30;
        do {
          if (dVar2 == *pdVar3) break;
          iVar1 = iVar1 + 1;
          pdVar3 = pdVar3 + 1;
        } while (iVar1 < 3);
        if (iVar1 == 3) {
          pdVar3 = &this->mbr_0x38;
          iVar1 = 2;
          do {
            *pdVar3 = pdVar3[-1];
            pdVar3 = pdVar3 + -1;
            iVar1 = iVar1 + -1;
          } while (iVar1 != 0);
          this->mbr_0x30 = dVar2;
          meth_0x49a300(this);
          iVar1 = _AIL_redbook_status_4(this->mbr_0x0);
          if (iVar1 == 1) {
            this->mbr_0x8 = 1;
          }
          else {
            this->mbr_0x8 = 0;
          }
        }
      } while( true );
    }
  }
  return;
}



// Function at 0049a590

dword cls_0x49a300::meth_0x49a590()

{
  dword dVar1;
  int iVar2;
  
  dVar1 = this->mbr_0x0;
  if (dVar1 == 0) {
    this->mbr_0x8 = 0;
    return dVar1;
  }
  if (this->mbr_0x8 != 0) {
    iVar2 = _AIL_redbook_status_4(dVar1);
    this->mbr_0x8 = (uint)(iVar2 == 1);
  }
  return this->mbr_0x8;
}



