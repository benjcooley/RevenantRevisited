#include "S3DAnimObj.h"

// Decompiled methods and structure for class: S3DAnimObj



// Function at 00469370

undefined4 S3DAnimObj::virt_meth_0x469370()

{
  int iVar1;
  
  iVar1 = this->virt_meth_0x435150(s_selstart_005d3bc4,0x659cb8,0x11,0x7e,0x41,0x18a,0x13c,
                     0x15c,0x54,s_widgets_005d3bbc);
  if (iVar1 == 0) {
    return 0;
  }
  this->htextures = 0;
  this->numfaces = 0;
  this->surfaces = 0;
  this->hmaterial = 0;
  return 1;
}



// Function at 004693e0

void S3DAnimObj::virt_meth_0x4693e0(int param_1)

{
  cls_0x5a3d44 *this_00;
  int in_stack_00000008;
  
  cls_0x5b98b8::meth_0x436930((cls_0x5b98b8 *)this);
  this_00 = (cls_0x5a3d44 *)cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  if (this_00 != (cls_0x5a3d44 *)0x0) {
    if (param_1 == 0x26) {
      if (in_stack_00000008 != 0) {
        cls_0x5a3d44::meth_0x430b80(this_00);
        return;
      }
    }
    else if ((param_1 == 0x28) && (in_stack_00000008 != 0)) {
      cls_0x5a3d44::meth_0x430b80(this_00);
      return;
    }
  }
  this->virt_meth_0x4361f0(param_1);
  return;
}



// Function at 004697a0

undefined4 S3DAnimObj::virt_meth_0x4697a0(byte *param_1, char *param_2, undefined4 param_3, undefined4 param_4)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  int in_stack_00000014;
  
  *param_2 = '\0';
  uVar1 = FUN_0059a530((uint)this,param_1,(byte *)s_startpos_005d3c74);
  if (uVar1 != 0) {
    uVar3 = this->virt_meth_0x436ea0();
    return uVar3;
  }
  iVar2 = TSound::meth_0x460ca0((TSound *)&UNK_0065a630.field_0x138,DAT_00658da0);
  if (in_stack_00000014 < *(int *)(iVar2 + 0x88)) {
    cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    FUN_0058b100(param_2,(byte *)s__s___s__005d3c88);
    return 1;
  }
  cls_0x45f7c0::meth_0x51fdd0((cls_0x45f7c0 *)&DAT_0065a890,(char)DAT_00667fcc,0);
  cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  FUN_0058b100(param_2,(byte *)s__s___s__005d3c9c);
  return 1;
}



// Function at 00469890

void S3DAnimObj::virt_meth_0x469890()

{
  cls_0x5a4e24__vftable_5a4e24 *pcVar1;
  
  this->virt_meth_0x435d70();
  if (((DAT_00668128 != 0) && (this->hmaterial != 0)) && (this->surfaces == 0)) {
    this->hmaterial = 0;
    this->surfaces = 1;
    FUN_00483300(0,*(int *)(this->numfaces + 0x98) + -1);
    cls_0x5a3d44::meth_0x430b80((cls_0x5a3d44 *)this->numfaces);
    pcVar1 = this->vftptr_0x0;
    cls_0x5b98b8::meth_0x436980((cls_0x5b98b8 *)this);
    (*pcVar1->virt_meth_0x469460_148)(this);
  }
  return;
}



// Function at 0046d250

S3DAnimObj::S3DAnimObj()

{
  this->parent = 0x280;
  this->scl = 0x280;
  this->animtrack = 0x1e0;
  this->matrix = 0x1e0;
  this->vftptr_0x0 = (cls_0x5a4e24__vftable_5a4e24 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->flags = 0;
  this->pos = 0;
  this->objnum = 0;
  this->rot = 0;
  this->primtype = 0;
  this->verttype = 0;
  this->numverts = 0;
  this->lverts = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a4e24__vftable_5a4e24_005a4e24;
  return this;
}



// Function at 0046d2b0

S3DAnimObj * S3DAnimObj::virt_meth_0x46d2b0()

{
  byte in_stack_00000004;
  
  ~S3DAnimObj(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0046d2d0

S3DAnimObj * __thiscall S3DAnimObj::~S3DAnimObj(S3DAnimObj *this)

{
  S3DAnimObj *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d1be;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a4e24__vftable_5a4e24 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (S3DAnimObj *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



