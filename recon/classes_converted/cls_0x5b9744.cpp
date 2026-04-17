#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b9744



// Function at 0053a8b0

undefined4 cls_0x5b9744::virt_meth_0x53a8b0()

{
  undefined4 uVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  undefined4 *puVar5;
  int iVar6;
  int iVar7;
  undefined4 *local_58;
  int local_54;
  int local_50;
  undefined4 local_40 [16];
  
  iVar7 = 0;
  this->virt_meth_0x435150(s_options_005e4494,s_default_005e448c,
             -(uint)(this->mbr_0x17c != 0) & 0x11,0,0,0x280,0x1e0,0x1c2,0xa0,s_widgets_005e4484);
  dVar3 = DAT_005d7a68;
  dVar2 = DAT_005d7a60;
  this->mbr_0x180 = DAT_005d7a18;
  dVar4 = DAT_005e91c0;
  this->mbr_0x188 = dVar2;
  iVar6 = DAT_006682a8;
  this->mbr_0x190 = dVar4;
  dVar2 = DAT_005d79e8;
  this->mbr_0x184 = dVar3;
  dVar3 = DAT_00668194;
  this->mbr_0x19c = dVar2;
  dVar2 = DAT_005d7a48;
  this->mbr_0x198 = dVar3;
  dVar3 = DAT_005d7aa0;
  this->mbr_0x1a8 = dVar2;
  this->mbr_0x194 = (uint)(iVar6 == 0);
  this->mbr_0x1a0 = DAT_005d7a9c;
  dVar2 = DAT_005d7a64;
  this->mbr_0x1b4 = 0xffffffff;
  this->mbr_0x1b8 = 0xffffffff;
  this->mbr_0x1bc = 0xffffffff;
  this->mbr_0x1c0 = 0xffffffff;
  this->mbr_0x1a4 = dVar3;
  this->mbr_0x18c = dVar2;
  this->mbr_0x1b0 = 0;
  if ((LPCVOID)this->mbr_0x1ac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x1ac);
  }
  puVar5 = FUN_00482fb0(DAT_005d697c * 0x24);
  this->mbr_0x1ac = (dword)puVar5;
  local_54 = 0;
  if (0 < DAT_005d697c) {
    do {
      local_58 = local_40;
      local_50 = 3;
      do {
        iVar6 = 3;
        puVar5 = local_58;
        do {
          cls_0x439000::meth_0x439060((cls_0x439000 *)&UNK_0065a904.field_0xa4,local_54);
          uVar1 = *puVar5;
          puVar5 = puVar5 + 1;
          *(undefined4 *)(iVar7 + this->mbr_0x1ac) = uVar1;
          iVar7 = iVar7 + 4;
          iVar6 = iVar6 + -1;
        } while (iVar6 != 0);
        local_58 = local_58 + 4;
        local_50 = local_50 + -1;
      } while (local_50 != 0);
      local_54 = local_54 + 1;
    } while (local_54 < DAT_005d697c);
  }
  cls_0x5b98b8::meth_0x437620((cls_0x5b98b8 *)this);
  return 1;
}



// Function at 0053aa60

void cls_0x5b9744::virt_meth_0x53aa60()

{
  if ((LPCVOID)this->mbr_0x1ac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x1ac);
  }
  this->mbr_0x1ac = 0;
  this->virt_meth_0x434f30();
  return;
}



// Function at 0053b9e0

void cls_0x5b9744::virt_meth_0x53b9e0(undefined4 param_1)

{
  int iVar1;
  
  iVar1 = cls_0x5b98b8::meth_0x436980((cls_0x5b98b8 *)this);
  if (-1 < *(int *)(iVar1 + 0xa0)) {
    cls_0x5b98b8::meth_0x53ba80((cls_0x5b98b8 *)this,param_1);
    return;
  }
  this->virt_meth_0x4361f0(param_1);
  return;
}



// Function at 0053bdc0

cls_0x5b9744::cls_0x5b9744()

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->vftptr_0x0 = (cls_0x5b9744__vftable_5b9744 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->mbr_0x17c = 0;
  this->vftptr_0x0 = &cls_0x5b9744__vftable_5b9744_005b9744;
  return this;
}



// Function at 0053be20

cls_0x5b9744 * __thiscall cls_0x5b9744::~cls_0x5b9744(cls_0x5b9744 *this)

{
  cls_0x5b9744 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1c06;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b9744__vftable_5b9744_005b9744;
  local_4 = 0;
  if ((LPCVOID)this->mbr_0x1ac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x1ac);
  }
  this->mbr_0x1ac = 0;
  this->virt_meth_0x434f30();
  this->vftptr_0x0 = (cls_0x5b9744__vftable_5b9744 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5b9744 *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 0053beb0

cls_0x5b9744 * cls_0x5b9744::virt_meth_0x53beb0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b9744(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



