#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b99c0



// Function at 0053f090

cls_0x5b99c0::cls_0x5b99c0 *this,dword param_1,dword *param_2,dword param_3,dword *param_4,
          dword param_5,dword param_6)

{
  TPlayScreen *this_00;
  dword dVar1;
  int iVar2;
  dword dVar3;
  dword *pdVar4;
  dword *pdVar5;
  dword in_stack_0000001c;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1dd6;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0xe);
  local_4 = 0;
  this->vftptr_0x0 = &cls_0x5b99c0__vftable_5b99c0_005b99c0;
  this->mbr_0x4 = param_1;
  if ((int)param_3 < 1) {
    param_3 = 1;
  }
  else if (0x3f < (int)param_3) {
    param_3 = 0x40;
  }
  this->mbr_0xc = param_3;
  if (param_2 == (dword *)0x0) {
    this->mbr_0x10 = param_1;
  }
  else {
    pdVar4 = &this->mbr_0x10;
    pdVar5 = pdVar4;
    for (dVar3 = param_3; dVar3 != 0; dVar3 = dVar3 - 1) {
      *pdVar5 = *param_2;
      param_2 = param_2 + 1;
      pdVar5 = pdVar5 + 1;
    }
    if ((*(int *)(param_5 + 200) != 0) && (0 < (int)param_3)) {
      do {
        iVar2 = FUN_00483300_RandomRange(0,100);
        if (iVar2 < *(int *)(param_5 + 200)) {
          (**(code **)(*(int *)*pdVar4 + 0x1b4))(1);
        }
        pdVar4 = pdVar4 + 1;
        param_3 = param_3 - 1;
      } while (param_3 != 0);
    }
  }
  dVar3 = *(dword *)(param_5 + 0xa4);
  this->mbr_0x120 = param_6;
  iVar2 = *(int *)(param_6 + 0x74);
  this->mbr_0x11c = param_5;
  this->mbr_0x110 = 0xffffffff;
  this->mbr_0x118 = in_stack_0000001c;
  this->mbr_0x124 = dVar3;
  if (((iVar2 != 0) && (this_00 = (TPlayScreen *)this->mbr_0x4, this_00 != (TPlayScreen *)0x0)) &&
     (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) {
    (this_00->TScreen).mbr_0x110 = (this_00->TScreen).mbr_0x110 | 0x20;
    TPlayScreen::meth_0x51c2c0(this_00);
  }
  if (param_4 == (dword *)0x0) {
    this->mbr_0x130 = 0xffffffff;
    this->mbr_0x12c = 0xffffffff;
    this->mbr_0x128 = 0xffffffff;
  }
  else {
    dVar3 = param_4[1];
    dVar1 = param_4[2];
    this->mbr_0x128 = *param_4;
    this->mbr_0x12c = dVar3;
    this->mbr_0x130 = dVar1;
  }
  ExceptionList = local_c;
  return this;
}



// Function at 0053f560

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5b99c0::meth_0x53f560()

{
  int *piVar1;
  int iVar2;
  int iVar3;
  longlong lVar4;
  int *in_stack_00000004;
  
  iVar3 = *(int *)(this->mbr_0x120 + 0x54);
  iVar2 = *(int *)(this->mbr_0x120 + 0x58);
  if (in_stack_00000004 != (int *)0x0) {
    if (_DAT_005a3530 < (float)in_stack_00000004[100]) {
      lVar4 = __ftol();
      iVar3 = iVar3 - (int)lVar4;
      lVar4 = __ftol();
      iVar2 = iVar2 - (int)lVar4;
    }
    piVar1 = (int *)this->mbr_0x4;
    iVar3 = FUN_00483300_RandomRange(iVar3,iVar2);
    if ((piVar1 != (int *)0x0) && (*(short *)(piVar1 + 1) == 0xb)) {
      iVar2 = (**(code **)(*piVar1 + 0x3cc))();
      iVar3 = ((iVar2 + 100) * iVar3) / 100;
    }
    if (*(short *)(in_stack_00000004 + 1) == 0xb) {
      iVar2 = (**(code **)(*in_stack_00000004 + 0x2c8))(6);
      iVar3 = ((100 - iVar2) * iVar3) / 100;
    }
    (**(code **)(*in_stack_00000004 + 0x228))
              (iVar3,*(undefined4 *)(this->mbr_0x11c + 0x80),0,0,piVar1);
    if ((piVar1 != (int *)0x0) && (*(short *)(piVar1 + 1) == 0xb)) {
      (**(code **)(*piVar1 + 0x414))();
    }
  }
  return;
}



// Function at 0053f800

void cls_0x5b99c0::virt_meth_0x53f800()

{
  int *piVar1;
  int iVar2;
  
  if ((DAT_0065d0c4 == 0) && (iVar2 = 0, 0 < (int)(this->cls_0x41c7f0).mbr_0x0)) {
    do {
      piVar1 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + iVar2 * 4);
      if ((piVar1 != (int *)0x0) && (piVar1[0x11] == 0)) {
        (**(code **)(*piVar1 + 0x110))();
        (**(code **)(*piVar1 + 0x8c))();
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  return;
}



// Function at 0053f860

void cls_0x5b99c0::virt_meth_0x53f860()

{
  cls_0x41c7f0 *pcVar1;
  dword dVar2;
  TPlayScreen *this_00;
  undefined extraout_DL;
  uint uVar3;
  
  uVar3 = 0;
  dVar2 = (this->cls_0x41c7f0).mbr_0x0;
  pcVar1 = &this->cls_0x41c7f0;
  this->mbr_0x110 = 0;
  if (0 < (int)dVar2) {
    do {
      dVar2 = (this->cls_0x41c7f0).mbr_0x10;
      if ((((dVar2 != 0) && (uVar3 < pcVar1->mbr_0x0)) && (*(int *)(dVar2 + uVar3 * 4) != 0)) &&
         (this_00 = *(TPlayScreen **)((this->cls_0x41c7f0).mbr_0x10 + uVar3 * 4),
         this_00 != (TPlayScreen *)0x0)) {
        if (((this->mbr_0x11c != 0) && (*(int *)(this->mbr_0x11c + 200) != 0)) &&
           ((0 < (int)this->mbr_0xc && ((int *)this->mbr_0x10 != (int *)0x0)))) {
          (**(code **)(*(int *)this->mbr_0x10 + 0x1b4))(0);
        }
        (*((this_00->TScreen).vftptr_0x0)->FUN_00472e90_64)
                  ((this_00->TScreen).mbr_0x8 | 0x1000);
        TPlayScreen::meth_0x4de7d0(this_00);
        FUN_00540af0((cls_0x45f7c0 *)pcVar1,extraout_DL,(char)this_00,
                     (char)(this_00->TScreen).mbr_0x10,0);
      }
      uVar3 = uVar3 + 1;
    } while ((int)uVar3 < (int)pcVar1->mbr_0x0);
  }
  return;
}



// Function at 00540b00

cls_0x5b99c0 * cls_0x5b99c0::virt_meth_0x540b00()

{
  byte in_stack_00000004;
  
  ~cls_0x5b99c0(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00540b20

cls_0x5b99c0 * __thiscall cls_0x5b99c0::~cls_0x5b99c0(cls_0x5b99c0 *this)

{
  LPCVOID pvVar1;
  cls_0x5b99c0 *pcVar2;
  
  pvVar1 = (LPCVOID)(this->cls_0x41c7f0).mbr_0x10;
  this->vftptr_0x0 = &cls_0x5b99c0__vftable_5b99c0_005b99c0;
  pcVar2 = (cls_0x5b99c0 *)FUN_004830f0(pvVar1);
  return pcVar2;
}



