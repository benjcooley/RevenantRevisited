#include "SControlEntry.h"

// Decompiled methods and structure for class: SControlEntry



// Function at 004de770

SControlEntry::SControlEntry *this,int param_1)

{
  uint uVar1;
  
  TScreen::TScreen(&this->mode);
  uVar1 = *(uint *)(param_1 + 4);
  (this->mode).vftptr_0x0 =
       (TScreen__vftable_5a50e8 *)&cls_0x5a85ac__vftable_5a85ac_005a85ac;
  (this->mode).mbr_0x8 = (this->mode).mbr_0x8 | uVar1 | 0x48001;
  *(undefined4 *)&(this->mode).field_0xe4 = 0;
  this->mbr_0x14c = 0;
  this->cmdflag = 0;
  this->codes = 0;
  this->down = 0;
  return this;
}



// Function at 004f4ae0

SControlEntry * SControlEntry::virt_meth_0x4f4ae0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen(&this->mode);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



