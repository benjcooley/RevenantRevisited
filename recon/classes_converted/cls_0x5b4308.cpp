#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b4308



// Function at 0050ad50

void cls_0x5b4308::virt_meth_0x50ad50()

{
  int iVar1;
  dword *pdVar2;
  dword *in_stack_00000004;
  
  pdVar2 = &this->mbr_0x4;
  for (iVar1 = 0x1f; iVar1 != 0; iVar1 = iVar1 + -1) {
    *pdVar2 = *in_stack_00000004;
    in_stack_00000004 = in_stack_00000004 + 1;
    pdVar2 = pdVar2 + 1;
  }
  return;
}



// Function at 0050af40

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5b4308::meth_0x50af40()

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  int iVar7;
  int iVar8;
  int *piVar9;
  int iVar10;
  longlong lVar11;
  
  iVar7 = FUN_00483300(1,100);
  if (iVar7 <= (int)this->mbr_0x8) {
    iVar7 = 0;
    if (0 < (int)this->mbr_0x84) {
      piVar9 = (int *)this->mbr_0x80;
      while (*piVar9 != 0) {
        iVar7 = iVar7 + 1;
        piVar9 = piVar9 + 0x12;
        if ((int)this->mbr_0x84 <= iVar7) {
          return;
        }
      }
      iVar10 = iVar7 * 0x48;
      ((int *)this->mbr_0x80)[iVar7 * 0x12] = 1;
      lVar11 = __ftol();
      iVar7 = (int)lVar11;
      lVar11 = __ftol();
      iVar7 = FUN_00483300((int)lVar11,iVar7);
      *(float *)(iVar10 + 4 + this->mbr_0x80) = (float)iVar7 + (float)this->mbr_0xc;
      lVar11 = __ftol();
      iVar7 = (int)lVar11;
      lVar11 = __ftol();
      iVar8 = FUN_00483300((int)lVar11,iVar7);
      iVar7 = iVar10 + this->mbr_0x80;
      *(float *)(iVar10 + 8 + this->mbr_0x80) = (float)iVar8 + (float)this->mbr_0x10;
      lVar11 = __ftol();
      iVar8 = (int)lVar11;
      lVar11 = __ftol();
      iVar8 = FUN_00483300((int)lVar11,iVar8);
      dVar1 = this->mbr_0x30;
      dVar2 = this->mbr_0x34;
      dVar3 = this->mbr_0x38;
      *(float *)(iVar7 + 0xc) = (float)iVar8 + (float)this->mbr_0x14;
      *(dword *)(iVar7 + 0x28) = dVar1;
      *(dword *)(iVar7 + 0x2c) = dVar2;
      *(dword *)(iVar7 + 0x30) = dVar3;
      lVar11 = __ftol();
      iVar7 = (int)lVar11;
      lVar11 = __ftol();
      iVar7 = FUN_00483300((int)lVar11,iVar7);
      *(float *)(iVar10 + 0x1c + this->mbr_0x80) = (float)iVar7 + (float)this->mbr_0x24;
      lVar11 = __ftol();
      iVar7 = (int)lVar11;
      lVar11 = __ftol();
      iVar7 = FUN_00483300((int)lVar11,iVar7);
      *(float *)(iVar10 + 0x20 + this->mbr_0x80) = (float)iVar7 + (float)this->mbr_0x28;
      lVar11 = __ftol();
      iVar7 = (int)lVar11;
      lVar11 = __ftol();
      iVar7 = FUN_00483300((int)lVar11,iVar7);
      *(float *)(iVar10 + 0x24 + this->mbr_0x80) = (float)iVar7 + (float)this->mbr_0x2c;
      if (_DAT_005a3530 <= (float)this->mbr_0x54) {
        if ((float)this->mbr_0x54 == _DAT_005a3530) {
          lVar11 = __ftol();
          iVar7 = (int)lVar11;
          lVar11 = __ftol();
          iVar7 = FUN_00483300((int)lVar11,iVar7);
          fVar4 = (float)iVar7;
        }
        else {
          lVar11 = __ftol();
          iVar7 = FUN_00483300(0,(int)lVar11);
          fVar4 = (float)iVar7;
        }
      }
      else {
        iVar7 = 0;
        lVar11 = __ftol();
        iVar7 = FUN_00483300((int)lVar11,iVar7);
        fVar4 = (float)iVar7;
      }
      if (_DAT_005a3530 <= (float)this->mbr_0x58) {
        if ((float)this->mbr_0x58 == _DAT_005a3530) {
          lVar11 = __ftol();
          iVar7 = (int)lVar11;
          lVar11 = __ftol();
          iVar7 = FUN_00483300((int)lVar11,iVar7);
          fVar5 = (float)iVar7;
        }
        else {
          lVar11 = __ftol();
          iVar7 = FUN_00483300(0,(int)lVar11);
          fVar5 = (float)iVar7;
        }
      }
      else {
        iVar7 = 0;
        lVar11 = __ftol();
        iVar7 = FUN_00483300((int)lVar11,iVar7);
        fVar5 = (float)iVar7;
      }
      if (_DAT_005a3530 <= (float)this->mbr_0x5c) {
        if ((float)this->mbr_0x54 == _DAT_005a3530) {
          lVar11 = __ftol();
          iVar7 = (int)lVar11;
          lVar11 = __ftol();
          iVar7 = FUN_00483300((int)lVar11,iVar7);
          fVar6 = (float)iVar7;
        }
        else {
          lVar11 = __ftol();
          iVar7 = FUN_00483300(0,(int)lVar11);
          fVar6 = (float)iVar7;
        }
      }
      else {
        iVar7 = 0;
        lVar11 = __ftol();
        iVar7 = FUN_00483300((int)lVar11,iVar7);
        fVar6 = (float)iVar7;
      }
      dVar1 = this->mbr_0x80;
      dVar2 = this->mbr_0x6c;
      iVar7 = iVar10 + dVar1;
      *(float *)(iVar7 + 0x10) = fVar4 + (float)this->mbr_0x48;
      *(float *)(iVar7 + 0x14) = fVar5 + (float)this->mbr_0x4c;
      *(float *)(iVar7 + 0x18) = fVar6 + (float)this->mbr_0x50;
      *(dword *)(iVar7 + 0x34) = dVar2;
      iVar7 = iVar10 + dVar1;
      iVar8 = FUN_00483300(this->mbr_0x70,this->mbr_0x74);
      dVar1 = this->mbr_0x80;
      *(int *)(iVar7 + 0x38) = iVar8;
      *(dword *)(iVar7 + 0x3c) = this->mbr_0x78;
      iVar10 = iVar10 + dVar1;
      iVar7 = FUN_00483300(0,1);
      dVar1 = this->mbr_0x7c;
      *(int *)(iVar10 + 0x40) = iVar7;
      *(dword *)(iVar10 + 0x44) = dVar1;
    }
  }
  return;
}



// Function at 00515240

cls_0x5b4308 * __thiscall cls_0x5b4308::~cls_0x5b4308(cls_0x5b4308 *this)

{
  cls_0x5b4308 *pcVar1;
  
  this->vftptr_0x0 = &cls_0x5b4308__vftable_5b4308_005b4308;
  pcVar1 = (cls_0x5b4308 *)FUN_004830f0((LPCVOID)this->mbr_0x80);
  return pcVar1;
}



// Function at 00515260

cls_0x5b4308 * __thiscall cls_0x5b4308::~cls_0x5b4308(cls_0x5b4308 *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5b4308__vftable_5b4308_005b4308;
  FUN_004830f0((LPCVOID)this->mbr_0x80);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



