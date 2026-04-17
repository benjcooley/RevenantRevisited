// Decompiled methods and structure for class: cls_0x5b4308

/*
/OOAnalyzer/cls_0x5b4308
pack(disabled)
Structure cls_0x5b4308 {
   0   cls_0x5b4308::vftable_5b4308 *   4   vftptr_0x0   "pointer to cls_0x5b4308::vftable_5b4308"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   104   dword   4   mbr_0x68   "Unsigned Double-Word (ddw, 4-bytes)"
   108   dword   4   mbr_0x6c   "Unsigned Double-Word (ddw, 4-bytes)"
   112   dword   4   mbr_0x70   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   132   dword   4   mbr_0x84   "Unsigned Double-Word (ddw, 4-bytes)"
   136   dword   4   mbr_0x88   "Unsigned Double-Word (ddw, 4-bytes)"
   140   dword   4   mbr_0x8c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 144 Alignment: 1

*/

// Function at 0050ad50

void __thiscall OOAnalyzer::cls_0x5b4308::virt_meth_0x50ad50(cls_0x5b4308 *this)

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

void __thiscall OOAnalyzer::cls_0x5b4308::meth_0x50af40(cls_0x5b4308 *this)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  int iVar7;
  undefined4 uVar8;
  int iVar9;
  int *piVar10;
  int iVar11;
  
  iVar7 = FUN_00483300(1,100);
  if (iVar7 <= (int)this->mbr_0x8) {
    iVar7 = 0;
    if (0 < (int)this->mbr_0x84) {
      piVar10 = (int *)this->mbr_0x80;
      while (*piVar10 != 0) {
        iVar7 = iVar7 + 1;
        piVar10 = piVar10 + 0x12;
        if ((int)this->mbr_0x84 <= iVar7) {
          return;
        }
      }
      iVar11 = iVar7 * 0x48;
      ((int *)this->mbr_0x80)[iVar7 * 0x12] = 1;
      uVar8 = __ftol();
      uVar8 = __ftol(uVar8);
      iVar7 = FUN_00483300(uVar8);
      *(float *)(iVar11 + 4 + this->mbr_0x80) = (float)iVar7 + (float)this->mbr_0xc;
      uVar8 = __ftol();
      uVar8 = __ftol(uVar8);
      iVar9 = FUN_00483300(uVar8);
      iVar7 = iVar11 + this->mbr_0x80;
      *(float *)(iVar11 + 8 + this->mbr_0x80) = (float)iVar9 + (float)this->mbr_0x10;
      uVar8 = __ftol();
      uVar8 = __ftol(uVar8);
      iVar9 = FUN_00483300(uVar8);
      dVar1 = this->mbr_0x30;
      dVar2 = this->mbr_0x34;
      dVar3 = this->mbr_0x38;
      *(float *)(iVar7 + 0xc) = (float)iVar9 + (float)this->mbr_0x14;
      *(dword *)(iVar7 + 0x28) = dVar1;
      *(dword *)(iVar7 + 0x2c) = dVar2;
      *(dword *)(iVar7 + 0x30) = dVar3;
      uVar8 = __ftol();
      uVar8 = __ftol(uVar8);
      iVar7 = FUN_00483300(uVar8);
      *(float *)(iVar11 + 0x1c + this->mbr_0x80) = (float)iVar7 + (float)this->mbr_0x24;
      uVar8 = __ftol();
      uVar8 = __ftol(uVar8);
      iVar7 = FUN_00483300(uVar8);
      *(float *)(iVar11 + 0x20 + this->mbr_0x80) = (float)iVar7 + (float)this->mbr_0x28;
      uVar8 = __ftol();
      uVar8 = __ftol(uVar8);
      iVar7 = FUN_00483300(uVar8);
      *(float *)(iVar11 + 0x24 + this->mbr_0x80) = (float)iVar7 + (float)this->mbr_0x2c;
      if (_DAT_005a3530 <= (float)this->mbr_0x54) {
        if ((float)this->mbr_0x54 == _DAT_005a3530) {
          uVar8 = __ftol();
          uVar8 = __ftol(uVar8);
          iVar7 = FUN_00483300(uVar8);
          fVar4 = (float)iVar7;
        }
        else {
          uVar8 = __ftol();
          iVar7 = FUN_00483300(0,uVar8);
          fVar4 = (float)iVar7;
        }
      }
      else {
        uVar8 = __ftol(0);
        iVar7 = FUN_00483300(uVar8);
        fVar4 = (float)iVar7;
      }
      if (_DAT_005a3530 <= (float)this->mbr_0x58) {
        if ((float)this->mbr_0x58 == _DAT_005a3530) {
          uVar8 = __ftol();
          uVar8 = __ftol(uVar8);
          iVar7 = FUN_00483300(uVar8);
          fVar5 = (float)iVar7;
        }
        else {
          uVar8 = __ftol();
          iVar7 = FUN_00483300(0,uVar8);
          fVar5 = (float)iVar7;
        }
      }
      else {
        uVar8 = __ftol(0);
        iVar7 = FUN_00483300(uVar8);
        fVar5 = (float)iVar7;
      }
      if (_DAT_005a3530 <= (float)this->mbr_0x5c) {
        if ((float)this->mbr_0x54 == _DAT_005a3530) {
          uVar8 = __ftol();
          uVar8 = __ftol(uVar8);
          iVar7 = FUN_00483300(uVar8);
          fVar6 = (float)iVar7;
        }
        else {
          uVar8 = __ftol();
          iVar7 = FUN_00483300(0,uVar8);
          fVar6 = (float)iVar7;
        }
      }
      else {
        uVar8 = __ftol(0);
        iVar7 = FUN_00483300(uVar8);
        fVar6 = (float)iVar7;
      }
      dVar1 = this->mbr_0x80;
      dVar2 = this->mbr_0x6c;
      iVar7 = iVar11 + dVar1;
      *(float *)(iVar7 + 0x10) = fVar4 + (float)this->mbr_0x48;
      *(float *)(iVar7 + 0x14) = fVar5 + (float)this->mbr_0x4c;
      *(float *)(iVar7 + 0x18) = fVar6 + (float)this->mbr_0x50;
      *(dword *)(iVar7 + 0x34) = dVar2;
      iVar7 = iVar11 + dVar1;
      uVar8 = FUN_00483300(this->mbr_0x70,this->mbr_0x74);
      dVar1 = this->mbr_0x80;
      *(undefined4 *)(iVar7 + 0x38) = uVar8;
      *(dword *)(iVar7 + 0x3c) = this->mbr_0x78;
      iVar11 = iVar11 + dVar1;
      uVar8 = FUN_00483300(0,1);
      dVar1 = this->mbr_0x7c;
      *(undefined4 *)(iVar11 + 0x40) = uVar8;
      *(dword *)(iVar11 + 0x44) = dVar1;
    }
  }
  return;
}



// Function at 00515240

cls_0x5b4308 * __thiscall OOAnalyzer::cls_0x5b4308::~cls_0x5b4308(cls_0x5b4308 *this)

{
  cls_0x5b4308 *pcVar1;
  
  this->vftptr_0x0 = &cls_0x5b4308__vftable_5b4308_005b4308;
  pcVar1 = (cls_0x5b4308 *)FUN_004830f0(this->mbr_0x80);
  return pcVar1;
}



// Function at 00515260

cls_0x5b4308 * __thiscall OOAnalyzer::cls_0x5b4308::~cls_0x5b4308(cls_0x5b4308 *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5b4308__vftable_5b4308_005b4308;
  FUN_004830f0(this->mbr_0x80);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



