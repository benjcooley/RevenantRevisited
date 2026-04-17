// Decompiled methods and structure for class: cls_0x50ba90

/*
/OOAnalyzer/cls_0x50ba90
pack(disabled)
Structure cls_0x50ba90 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 84 Alignment: 1

*/

// Function at 0050ba90

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

cls_0x50ba90 * __thiscall OOAnalyzer::cls_0x50ba90::cls_0x50ba90(cls_0x50ba90 *this,dword param_1)

{
  float fVar1;
  dword dVar2;
  short sVar3;
  dword *pdVar4;
  undefined4 *puVar5;
  uint uVar6;
  undefined2 *puVar7;
  float *pfVar8;
  dword dVar9;
  int iVar10;
  short *psVar11;
  uint uVar12;
  int iVar13;
  float10 fVar14;
  float10 fVar15;
  dword in_stack_00000008;
  float local_20 [4];
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  iVar13 = 0;
  local_20[0] = -60.0;
  this->mbr_0x18 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x4 = 0;
  local_20[1] = -60.0;
  local_20[2] = 0.0;
  local_20[3] = 60.0;
  local_10 = 0x42700000;
  local_c = 0x42700000;
  local_8 = 0;
  local_4 = 0xc2700000;
  pfVar8 = local_20;
  iVar10 = 8;
  pdVar4 = &this->mbr_0x50;
  do {
    fVar1 = *pfVar8;
    fVar15 = (float10)_DAT_005a8a98;
    pfVar8 = pfVar8 + 1;
    iVar10 = iVar10 + -1;
    fVar14 = (float10)fcos((float10)fVar1 * fVar15);
    pdVar4[-8] = (dword)(float)fVar14;
    fVar15 = (float10)fsin((float10)fVar1 * fVar15);
    *pdVar4 = (dword)(float)fVar15;
    pdVar4 = pdVar4 + 1;
  } while (iVar10 != 0);
  this->mbr_0xc = 10;
  this->mbr_0x0 = param_1;
  fVar1 = _DAT_005a34e4 / (float)in_stack_00000008;
  this->mbr_0x14 = in_stack_00000008;
  this->mbr_0x10 = 5;
  this->mbr_0x24 = 0;
  this->mbr_0x28 = 0;
  this->mbr_0x2c = 0x3f800000;
  *(float *)&this->field_0x20 = fVar1;
  puVar5 = FUN_00482fb0(in_stack_00000008 * 0xc);
  this->mbr_0x1c = (dword)puVar5;
  dVar9 = this->mbr_0x14;
  *(dword *)(this->mbr_0x0 + 0xa0) = dVar9 << 1;
  puVar5 = FUN_00482fb0(dVar9 << 6);
  dVar9 = this->mbr_0x0;
  *(undefined4 **)(dVar9 + 0xa4) = puVar5;
  dVar2 = this->mbr_0x14;
  *(undefined4 *)(dVar9 + 0x9c) = 0x1e2;
  iVar10 = dVar2 * 2 + -2;
  *(int *)(dVar9 + 0xa8) = iVar10;
  if (*(int *)(dVar9 + 0xac) == 0) {
    puVar5 = FUN_00482fb0(iVar10 * 6);
    dVar9 = this->mbr_0x0;
    *(undefined4 **)(dVar9 + 0xac) = puVar5;
  }
  *(undefined4 *)(dVar9 + 0xb0) = 0;
  *(undefined4 *)(dVar9 + 0xb4) = 0;
  *(undefined4 *)(dVar9 + 0x134) = 0;
  *(undefined4 *)(dVar9 + 0x138) = *(undefined4 *)(dVar9 + 0xa8);
  uVar12 = 0;
  if (*(int *)(dVar9 + 0xa8) < 1) {
    return this;
  }
  do {
    uVar6 = uVar12 & 0x80000003;
    if ((int)uVar6 < 0) {
      uVar6 = (uVar6 - 1 | 0xfffffffc) + 1;
    }
    switch(uVar6) {
    case 0:
      puVar7 = (undefined2 *)(*(int *)(dVar9 + 0xac) + iVar13);
      *puVar7 = 2;
      puVar7[1] = 0;
      puVar7[2] = 3;
      break;
    case 1:
      puVar7 = (undefined2 *)(iVar13 + *(int *)(dVar9 + 0xac));
      *puVar7 = 1;
      puVar7[1] = 3;
      puVar7[2] = 0;
      break;
    case 2:
      puVar7 = (undefined2 *)(*(int *)(dVar9 + 0xac) + iVar13);
      *puVar7 = 2;
      puVar7[1] = 4;
      puVar7[2] = 3;
      break;
    case 3:
      puVar7 = (undefined2 *)(iVar13 + *(int *)(dVar9 + 0xac));
      *puVar7 = 5;
      puVar7[1] = 3;
      puVar7[2] = 4;
    }
    iVar10 = ((int)(uVar12 + ((int)uVar12 >> 0x1f & 3U)) >> 2) << 2;
    if (iVar10 != 0) {
      psVar11 = (short *)(*(int *)(dVar9 + 0xac) + iVar13);
      sVar3 = (short)iVar10;
      *psVar11 = *psVar11 + sVar3;
      psVar11[1] = psVar11[1] + sVar3;
      psVar11[2] = psVar11[2] + sVar3;
    }
    uVar12 = uVar12 + 1;
    iVar13 = iVar13 + 6;
  } while ((int)uVar12 < *(int *)(dVar9 + 0xa8));
  return this;
}



// Function at 0050bd10

void __thiscall OOAnalyzer::cls_0x50ba90::meth_0x50bd10(cls_0x50ba90 *this,float param_1)

{
  float in_stack_00000008;
  
  this->mbr_0x28 = (dword)param_1;
  this->mbr_0x2c = (dword)in_stack_00000008;
  *(float *)&this->field_0x20 = (in_stack_00000008 - param_1) / (float)this->mbr_0x14;
  if ((float)this->mbr_0x24 < param_1) {
    this->mbr_0x24 = (dword)param_1;
  }
  if (in_stack_00000008 < (float)this->mbr_0x24) {
    this->mbr_0x24 = (dword)in_stack_00000008;
  }
  return;
}



// Function at 0050c140

void __thiscall OOAnalyzer::cls_0x50ba90::meth_0x50c140(cls_0x50ba90 *this,dword param_1)

{
  dword in_stack_00000008;
  
  this->mbr_0xc = param_1;
  this->mbr_0x10 = in_stack_00000008;
  return;
}



