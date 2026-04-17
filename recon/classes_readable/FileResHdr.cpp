#include "FileResHdr.h"

// Decompiled methods and structure for class: FileResHdr



// Function at 0049a270

undefined4 FileResHdr::meth_0x49a270()

{
  dword dVar1;
  undefined4 uVar2;
  
  if (DAT_00668114 != 0) {
    return 1;
  }
  uVar2 = 0;
  if (this->resmagic == 0) {
    dVar1 = _AIL_redbook_open_4(0);
    this->resmagic = dVar1;
    if (dVar1 == 0) goto LAB_0049a2b1;
  }
  uVar2 = 1;
  dVar1 = _AIL_redbook_volume_4(this->resmagic);
  this->version = dVar1;
  this->topbm = 0x60;
LAB_0049a2b1:
  this->hdrsize = 0;
  this->objsize = 0xffffffff;
  this->datasize = 0;
  this->comptype = 0;
  return uVar2;
}



