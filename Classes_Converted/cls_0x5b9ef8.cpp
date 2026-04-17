#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b9ef8



// Function at 00570110

cls_0x5b9ef8::cls_0x5b9ef8()

{
  undefined4 *puVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2283;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5ba0a0::cls_0x5ba0a0(&this->cls_0x5ba0a0);
  local_4 = 0;
  cls_0x588410::cls_0x588410((cls_0x588410 *)&(this->cls_0x5ba0a0).cls_0x588410.mbr_0x24);
  local_4 = CONCAT31(local_4._1_3_,1);
  (this->cls_0x5ba0a0).vftptr_0x0 =
       (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5b9ef8__vftable_5b9ef8_005b9ef8;
  *(undefined4 *)&this->field_0x44 = 0;
  *(undefined4 *)&this->field_0x48 = 0xffffffff;
  puVar1 = FUN_00482fb0(0x1450);
  *(undefined4 **)&this->field_0x4c = puVar1;
  ExceptionList = local_c;
  return this;
}



// Function at 00570180

cls_0x5b9ef8 * __thiscall cls_0x5b9ef8::~cls_0x5b9ef8(cls_0x5b9ef8 *this)

{
  cls_0x5ba0a0 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  uint local_4;
  
  puStack_8 = &LAB_005a22a0;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  (this->cls_0x5ba0a0).vftptr_0x0 =
       (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5b9ef8__vftable_5b9ef8_005b9ef8;
  local_4 = 1;
  FUN_004830f0(*(LPCVOID *)&this->field_0x4c);
  local_4 = local_4 & 0xffffff00;
  cls_0x588410::meth_0x588480((cls_0x588410 *)&(this->cls_0x5ba0a0).cls_0x588410.mbr_0x24);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5ba0a0::~cls_0x5ba0a0(&this->cls_0x5ba0a0);
  ExceptionList = local_c;
  return (cls_0x5b9ef8 *)pcVar1;
}



// Function at 00570620

undefined4 cls_0x5b9ef8::virt_meth_0x570620(undefined4 param_1)

{
  char *_Dest;
  int iVar1;
  short sVar2;
  short *psVar3;
  char *_Source;
  undefined4 uVar4;
  int iVar5;
  bool bVar6;
  cls_0x588410 *in_stack_00000008;
  
  psVar3 = (short *)in_stack_00000008->mbr_0xc;
  in_stack_00000008->mbr_0xc = (dword)(psVar3 + 1);
  if ((short *)in_stack_00000008->mbr_0x4 < psVar3 + 1) {
    psVar3 = (short *)cls_0x588410::meth_0x5884a0(in_stack_00000008);
  }
  sVar2 = *psVar3;
  _Source = (char *)cls_0x588410::meth_0x5892e0(in_stack_00000008);
  if ((_Source == (char *)0x0) || (*_Source == '\0')) {
    uVar4 = 0;
  }
  else {
    iVar1 = *(int *)&this->field_0x48;
    iVar5 = iVar1 + 1;
    *(int *)&this->field_0x48 = iVar5;
    if (0x31 < (iVar5 - *(int *)&this->field_0x44) + 1) {
      *(int *)&this->field_0x44 = iVar1 + -0x30;
    }
    iVar1 = *(int *)&this->field_0x4c;
    *(int *)(iVar1 + (iVar5 % 0x32) * 0x68) = (int)sVar2;
    _Dest = (char *)(iVar1 + 4 + iVar5 * 0x68);
    _strncpy(_Dest,_Source,99);
    bVar6 = DAT_006682bc != 0;
    _Dest[99] = '\0';
    uVar4 = 1;
    if (bVar6) {
      DAT_005cfe70 = 1;
      return uVar4;
    }
  }
  return uVar4;
}



// Function at 00570790

cls_0x5b9ef8 * cls_0x5b9ef8::virt_meth_0x570790()

{
  byte in_stack_00000004;
  
  ~cls_0x5b9ef8(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



