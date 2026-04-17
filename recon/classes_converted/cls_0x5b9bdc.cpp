#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b9bdc



// Function at 0053f1f0

undefined4 cls_0x5b9bdc::meth_0x53f1f0()

{
  dword dVar1;
  dword *in_stack_00000004;
  
  if (((this->mbr_0x128 == 0xffffffff) && (this->mbr_0x12c == 0xffffffff)) &&
     (this->mbr_0x130 == 0xffffffff)) {
    in_stack_00000004[2] = 0;
    in_stack_00000004[1] = 0;
    *in_stack_00000004 = 0;
    return 1;
  }
  *in_stack_00000004 = this->mbr_0x128;
  dVar1 = this->mbr_0x130;
  in_stack_00000004[1] = this->mbr_0x12c;
  in_stack_00000004[2] = dVar1;
  return 1;
}



// Function at 00540bc0

void cls_0x5b9bdc::meth_0x540bc0(int *param_1)

{
  dword dVar1;
  int iVar2;
  undefined2 uVar3;
  cls_0x5a7e38 *this_00;
  undefined4 uVar4;
  int iVar5;
  undefined4 *puVar6;
  longlong lVar7;
  undefined4 *in_stack_00000008;
  int local_4c;
  int iStack_48;
  int iStack_44;
  
  puVar6 = in_stack_00000008;
  for (iVar5 = 0xd; iVar5 != 0; iVar5 = iVar5 + -1) {
    *puVar6 = 0;
    puVar6 = puVar6 + 1;
  }
  *(undefined2 *)((int)in_stack_00000008 + 10) = DAT_00666970;
  *(undefined2 *)in_stack_00000008 = 0x19;
  iVar5 = param_1[4];
  in_stack_00000008[4] = param_1[5];
  dVar1 = this->mbr_0x120;
  in_stack_00000008[3] = iVar5;
  in_stack_00000008[5] = param_1[6];
  if (dVar1 == 0) {
    iVar5 = 0;
  }
  else {
    iVar5 = *(int *)(dVar1 + 0x70);
  }
  if (*(int *)(iVar5 + 0xac) != 0) {
    if (dVar1 == 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = *(int *)(dVar1 + 0x70);
    }
    in_stack_00000008[5] = in_stack_00000008[5] + *(int *)(iVar5 + 0xc0);
    iVar2 = *(int *)(iVar5 + 0xb8);
    in_stack_00000008[4] = in_stack_00000008[4] + *(int *)(iVar5 + 0xbc);
    in_stack_00000008[3] = in_stack_00000008[3] + iVar2;
    goto LAB_00540d26;
  }
  if (dVar1 == 0) {
    iVar5 = 0;
  }
  else {
    iVar5 = *(int *)(dVar1 + 0x70);
  }
  if (*(int *)(iVar5 + 0xa4) == 0) {
    iVar5 = 0x32;
    in_stack_00000008[3] = in_stack_00000008[3] + 0x32;
    in_stack_00000008[4] = in_stack_00000008[4] + 0x32;
LAB_00540cdc:
    in_stack_00000008[5] = in_stack_00000008[5] + iVar5;
  }
  else {
    this_00 = (cls_0x5a7e38 *)(**(code **)(*param_1 + 0x24))();
    if (this_00 != (cls_0x5a7e38 *)0x0) {
      uVar4 = cls_0x5a7e38::meth_0x40edc0(this_00);
      cls_0x5a7e38::meth_0x40ef80(this_00,uVar4);
      lVar7 = __ftol();
      in_stack_00000008[3] = in_stack_00000008[3] + (int)lVar7;
      lVar7 = __ftol();
      in_stack_00000008[4] = in_stack_00000008[4] + (int)lVar7;
      lVar7 = __ftol();
      iVar5 = (int)lVar7;
      goto LAB_00540cdc;
    }
  }
  iVar5 = meth_0x53f1f0(this);
  if (iVar5 == 0) {
    in_stack_00000008[5] = in_stack_00000008[5] + *(int *)(this->mbr_0x120 + 100);
  }
  else {
    in_stack_00000008[4] = in_stack_00000008[4] + iStack_48;
    in_stack_00000008[5] = in_stack_00000008[5] + iStack_44;
    in_stack_00000008[3] = in_stack_00000008[3] + local_4c;
  }
LAB_00540d26:
  dVar1 = this->mbr_0x120;
  if ((*(int *)(dVar1 + 0x68) == 0) || (this->mbr_0x4 == 0)) {
    *(undefined *)((int)in_stack_00000008 + 0x32) = 0;
  }
  else {
    *(undefined *)((int)in_stack_00000008 + 0x32) = *(undefined *)(this->mbr_0x4 + 0x36);
  }
  uVar3 = cls_0x4746d0::meth_0x475210((cls_0x4746d0 *)&UNK_0066cc1c.field_0x4c,dVar1 + 0x2a);
  *(undefined2 *)((int)in_stack_00000008 + 2) = uVar3;
  return;
}



// Function at 00542340

void cls_0x5b9bdc::virt_meth_0x542340()

{
  dword *pdVar1;
  TPlayScreen *this_00;
  int iVar2;
  undefined extraout_DL;
  uint uVar3;
  
  uVar3 = 0;
  pdVar1 = &this->mbr_0x13c;
  this->mbr_0x110 = 0;
  if (0 < (int)this->mbr_0x13c) {
    do {
      if ((((this->mbr_0x14c != 0) && (uVar3 < *pdVar1)) &&
          (*(int *)(this->mbr_0x14c + uVar3 * 4) != 0)) &&
         (this_00 = *(TPlayScreen **)(this->mbr_0x14c + uVar3 * 4), this_00 != (TPlayScreen *)0x0)
         ) {
        if (((this->mbr_0x11c != 0) && (*(int *)(this->mbr_0x11c + 200) != 0)) &&
           ((0 < (int)this->mbr_0xc && ((int *)this->mbr_0x10 != (int *)0x0)))) {
          (**(code **)(*(int *)this->mbr_0x10 + 0x1b4))(0);
        }
        (*((this_00->TScreen).vftptr_0x0)->FUN_00472e90_64)
                  ((this_00->TScreen).mbr_0x8 | 0x1000);
        TPlayScreen::meth_0x4de7d0(this_00);
        FUN_00540af0((cls_0x45f7c0 *)pdVar1,extraout_DL,(char)this_00,
                     (char)(this_00->TScreen).mbr_0x10,0);
      }
      uVar3 = uVar3 + 1;
    } while ((int)uVar3 < (int)*pdVar1);
  }
  if (((this->mbr_0x120 != 0) && (iVar2 = *(int *)(this->mbr_0x120 + 0x70), iVar2 != 0)) &&
     (iVar2 != -0x74)) {
    cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
    cls_0x41c7d0::meth_0x49bd90((cls_0x41c7d0 *)&DAT_00667548);
  }
  return;
}



// Function at 00542430

cls_0x5b9bdc * cls_0x5b9bdc::virt_meth_0x542430()

{
  byte in_stack_00000004;
  
  ~cls_0x5b9bdc(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00542450

cls_0x5b9bdc * __thiscall cls_0x5b9bdc::~cls_0x5b9bdc(cls_0x5b9bdc *this)

{
  cls_0x5b9bdc *pcVar1;
  
  this->vftptr_0x0 = (cls_0x5b9bdc__vftable_5b9bdc *)&cls_0x5b99c0__vftable_5b99c0_005b99c0;
  pcVar1 = (cls_0x5b9bdc *)FUN_004830f0((LPCVOID)this->mbr_0x14c);
  return pcVar1;
}



