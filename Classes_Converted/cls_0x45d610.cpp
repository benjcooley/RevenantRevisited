#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x45d610



// Function at 0045d610

void cls_0x45d610::meth_0x45d610(undefined4 param_1, undefined4 param_2)

{
  undefined4 local_18 [3];
  undefined4 local_c [3];
  
  FUN_0046d810(param_1,local_c);
  FUN_0046d810(param_2,local_18);
  meth_0x45d660(this,local_c,local_18);
  return;
}



// Function at 0045d660

void cls_0x45d610::meth_0x45d660(cls_0x5a68d8 *param_1, uint *param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  cls_0x5a68d8 *this_00;
  dword dVar7;
  uint uVar8;
  uint uVar9;
  dword in_stack_0000000c;
  dword local_18;
  dword local_14;
  dword local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d068;
  local_c = ExceptionList;
  uVar1 = *(uint *)((int)param_1 + 4);
  uVar2 = *(uint *)param_1;
  uVar9 = uVar2 & 0xfffffe00;
  uVar3 = *param_2;
  uVar8 = uVar1 & 0xfffffe00;
  ExceptionList = &local_c;
  this->mbr_0xcc = uVar3 & 0xfffffe00;
  uVar4 = param_2[1];
  this->mbr_0x908 = (int)uVar3 >> 9;
  iVar5 = DAT_0065c5c4;
  dVar7 = (int)uVar2 >> 9;
  this->mbr_0x900 = dVar7;
  this->mbr_0x910 = dVar7;
  iVar6 = DAT_00667c30;
  this->mbr_0xd4 = uVar4 & 0xfffffe00;
  this->mbr_0xc8 = uVar9;
  dVar7 = (int)uVar1 >> 9;
  this->mbr_0xd0 = uVar8;
  this->mbr_0x904 = dVar7;
  this->mbr_0x914 = dVar7;
  this->mbr_0x90c = (int)uVar4 >> 9;
  FUN_0046dad0(iVar6 / 2 + uVar9,iVar5 / 2 + uVar8,(int *)&local_18,0);
  FUN_0046d7a0(&local_18,&param_2,&param_1);
  iVar5 = DAT_00667c30;
  this->mbr_0x38 = (int)param_1 - DAT_0065c5c4 / 2;
  this->mbr_0x34 = (int)param_2 - iVar5 / 2;
  if (((local_18 != this->mbr_0xb0) || (local_14 != this->mbr_0xb4)) || (local_10 != this->mbr_0xb8)
     ) {
    cls_0x49beb0::meth_0x49beb0((cls_0x49beb0 *)&DAT_00667548,local_18,local_14);
  }
  this->mbr_0xac = local_10;
  this->mbr_0xa4 = local_18;
  this->mbr_0xa8 = local_14;
  TCharacter::meth_0x4546a0((TCharacter *)&DAT_006668d8);
  DAT_0065847c = DAT_006680b0;
  this->mbr_0x8f8 = 1;
  DAT_006680b0 = 1;
  this->mbr_0x8fc = in_stack_0000000c;
  if (in_stack_0000000c != 0) {
    DAT_006582f8 = FUN_004a1ec0(((this->mbr_0x908 - this->mbr_0x900) + 1) * 0x40,
                                ((this->mbr_0x90c - this->mbr_0x904) + 1) * 0x40,0x40004,0);
    this_00 = (cls_0x5a68d8 *)FUN_00482fb0(0x6c);
    local_4 = 0;
    if (this_00 == (cls_0x5a68d8 *)0x0) {
      DAT_00658450 = (cls_0x5a68d8 *)0x0;
    }
    else {
      param_1 = this_00;
      cls_0x5a68d8::cls_0x5a68d8(this_00);
      local_4 = CONCAT31(local_4._1_3_,1);
      this_00->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
      cls_0x5a68d8::meth_0x4a3a40(this_00);
      DAT_00658450 = this_00;
    }
  }
  ExceptionList = local_c;
  return;
}



