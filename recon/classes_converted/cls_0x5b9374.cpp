#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b9374



// Function at 005338a0

undefined4 cls_0x5b9374::virt_meth_0x5338a0()

{
  int iVar1;
  uint *unaff_ESI;
  
  (this->TObjectInstance).mbr_0x54 = 0;
  DAT_0066f6f4 = FUN_0047f670((cls_0x49ead0 *)s_death_dat_005e3ebc,(void *)0xffffffff,0,unaff_ESI);
  iVar1 = cls_0x5b93c4::virt_meth_0x5339b0((cls_0x5b93c4 *)&DAT_0066f500);
  if (iVar1 == 0) {
    FUN_00481c10((byte *)s_Trouble_initializing_Death_pane_005e3ec8);
  }
  iVar1 = cls_0x46d6b0::meth_0x46d710(DAT_0065abc0);
  FUN_0043a020(iVar1);
  cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
  return 1;
}



// Function at 00533c60

cls_0x5b9374::cls_0x5b9374()

{
  TObjectInstance::TObjectInstance(&this->TObjectInstance);
  (this->TObjectInstance).vftptr_0x0 =
       (TObjectInstance__vftable_5a5ed4 *)&cls_0x5b9374__vftable_5b9374_005b9374;
  return this;
}



// Function at 00533c90

cls_0x5b9374 * cls_0x5b9374::virt_meth_0x533c90()

{
  byte in_stack_00000004;
  
  TObjectInstance::~TObjectInstance(&this->TObjectInstance);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



