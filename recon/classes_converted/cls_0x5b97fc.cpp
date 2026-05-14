#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b97fc



// Function at 0053bf00

void cls_0x5b97fc::virt_meth_0x53bf00(dword param_1)

{
  char *pcVar1;
  uint in_stack_00000008;
  
  if ((in_stack_00000008 & 0x10) == 0) {
    param_1 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  }
  this->mbr_0x180 = param_1;
  this->mbr_0x17c = in_stack_00000008;
  if ((in_stack_00000008 & 2) == 0) {
    pcVar1 = s_okcancel_005e4658;
    if ((in_stack_00000008 & 4) == 0) {
      pcVar1 = &DAT_005e4664;
    }
  }
  else {
    pcVar1 = s_yesno_005e4650;
  }
  this->virt_meth_0x435150(s_popup_005e4678,pcVar1,
             -(uint)((in_stack_00000008 & 1) != 0) & 0x11,0x81,0x7a,0x18e,0xd4,0x134,100,
             s_widgets_005e4670);
  return;
}



// Function at 0053bfa0

undefined4 cls_0x5b97fc::virt_meth_0x53bfa0(int param_1)

{
  byte *pbVar1;
  int *piVar2;
  uint uVar3;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  int in_stack_00000008;
  
  this->virt_meth_0x437a80(param_1);
  if (in_stack_00000008 == 1) {
    piVar2 = (int *)cls_0x5b98b8::meth_0x436980((cls_0x5b98b8 *)this);
    (**(code **)(*piVar2 + 0x18))(this->mbr_0x180);
    return 1;
  }
  if (in_stack_00000008 == 3000) {
    pbVar1 = (byte *)(param_1 + 0x18);
    uVar3 = FUN_0059a530_stricmp(extraout_ECX,&DAT_005e4688,pbVar1);
    if ((uVar3 != 0) && (uVar3 = FUN_0059a530_stricmp(extraout_ECX_00,&DAT_005e468c,pbVar1), uVar3 != 0)) {
      uVar3 = FUN_0059a530_stricmp(extraout_ECX_01,(byte *)s_cancel_005e4690,pbVar1);
      if ((uVar3 != 0) && (uVar3 = FUN_0059a530_stricmp(extraout_ECX_02,&DAT_005e4698,pbVar1), uVar3 != 0))
      {
        return 1;
      }
      this->mbr_0x5c = 0;
      (*this->vftptr_0x0->virt_meth_0x435010_8)((cls_0x5b93c4 *)this);
      return 1;
    }
    this->mbr_0x5c = 1;
    (*this->vftptr_0x0->virt_meth_0x435010_8)((cls_0x5b93c4 *)this);
  }
  return 1;
}



// Function at 0053c4c0

cls_0x5b97fc::cls_0x5b97fc()

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->vftptr_0x0 = (cls_0x5b97fc__vftable_5b97fc *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5b97fc__vftable_5b97fc_005b97fc;
  return this;
}



// Function at 0053c520

cls_0x5b97fc * cls_0x5b97fc::virt_meth_0x53c520()

{
  byte in_stack_00000004;
  
  ~cls_0x5b97fc(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0053c540

cls_0x5b97fc * __thiscall cls_0x5b97fc::~cls_0x5b97fc(cls_0x5b97fc *this)

{
  cls_0x5b97fc *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1c1e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5b97fc__vftable_5b97fc *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5b97fc *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



