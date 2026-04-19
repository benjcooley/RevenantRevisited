// REVSYNC: candidate=T3DImageryRes confidence=MEDIUM evidence=src-path(3DImage.cpp x10) src-file=src/3dimage.cpp date=2026-04-17 note="3dimage.cpp has multiple classes; filename guess plausible"
// Decompiled methods and structure for class: cls_0x5a486c

/*
/OOAnalyzer/cls_0x5a486c
pack(disabled)
Structure cls_0x5a486c {
   0   cls_0x5a486c::vftable_5a486c *   4   vftptr_0x0   "pointer to cls_0x5a486c::vftable_5a486c"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   28   byte   1   mbr_0x1c   "Unsigned Byte (db)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   140   dword   4   mbr_0x8c   "Unsigned Double-Word (ddw, 4-bytes)"
   144   dword   4   mbr_0x90   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
   168   dword   4   mbr_0xa8   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   176   dword   4   mbr_0xb0   "Unsigned Double-Word (ddw, 4-bytes)"
   180   dword   4   mbr_0xb4   "Unsigned Double-Word (ddw, 4-bytes)"
   184   dword   4   mbr_0xb8   "Unsigned Double-Word (ddw, 4-bytes)"
   188   dword   4   mbr_0xbc   "Unsigned Double-Word (ddw, 4-bytes)"
   192   dword   4   mbr_0xc0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 196 Alignment: 1

*/

// Function at 004089c0

dword __thiscall OOAnalyzer::cls_0x5a486c::meth_0x4089c0(cls_0x5a486c *this)

{
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  return this->mbr_0x24;
}



// Function at 004089f0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall
OOAnalyzer::cls_0x5a486c::meth_0x4089f0
          (cls_0x5a486c *this,undefined4 *param_1,int param_2,int param_3,int param_4,int param_5)

{
  undefined4 uVar1;
  float fVar2;
  float fVar3;
  undefined4 *puVar4;
  float *pfVar5;
  uint uVar6;
  int iVar7;
  undefined4 *puVar8;
  float *pfVar9;
  dword in_stack_00000018;
  
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  if (param_5 < 0) {
    param_5 = 0;
  }
  if ((int)in_stack_00000018 < 0) {
    in_stack_00000018 = this->mbr_0x24;
  }
  if ((this->mbr_0x1c & 1) == 0) {
    iVar7 = **(int **)this->mbr_0x28;
  }
  else {
    iVar7 = ((int **)this->mbr_0x28)[param_2][param_3];
  }
  if (param_4 != 0x112) {
    if (param_4 == 0x1e2) {
      if (0 < (int)in_stack_00000018) {
        puVar4 = param_1 + 2;
        puVar8 = (undefined4 *)(param_5 * 0x20 + iVar7 + 8);
        do {
          uVar1 = puVar8[-2];
          puVar4[2] = 0xfffffff8;
          puVar4[-2] = uVar1;
          puVar4[-1] = puVar8[-1];
          *puVar4 = *puVar8;
          puVar4[4] = puVar8[4];
          uVar1 = puVar8[5];
          puVar4[3] = 0;
          puVar4[5] = uVar1;
          puVar4 = puVar4 + 8;
          puVar8 = puVar8 + 8;
          in_stack_00000018 = in_stack_00000018 - 1;
        } while (in_stack_00000018 != 0);
        return;
      }
    }
    else if ((param_4 == 0x1c4) && (0 < (int)in_stack_00000018)) {
      pfVar5 = (float *)(param_1 + 2);
      pfVar9 = (float *)(param_5 * 0x20 + iVar7 + 8);
      do {
        fVar3 = pfVar9[-2] + _DAT_005a3510;
        fVar2 = pfVar9[4];
        pfVar5[1] = 1.0;
        pfVar5[4] = fVar2;
        fVar2 = pfVar9[5];
        pfVar5[2] = -NAN;
        pfVar5[3] = 0.0;
        pfVar5[-2] = fVar3;
        fVar3 = pfVar9[-1] + _DAT_005a3510;
        pfVar5[5] = fVar2;
        in_stack_00000018 = in_stack_00000018 - 1;
        pfVar5[-1] = fVar3;
        *pfVar5 = *pfVar9 + _DAT_005a3698;
        pfVar5 = pfVar5 + 8;
        pfVar9 = pfVar9 + 8;
      } while (in_stack_00000018 != 0);
    }
    return;
  }
  puVar4 = (undefined4 *)(param_5 * 0x20 + iVar7);
  for (uVar6 = (in_stack_00000018 << 5) >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    *param_1 = *puVar4;
    puVar4 = puVar4 + 1;
    param_1 = param_1 + 1;
  }
  for (iVar7 = 0; iVar7 != 0; iVar7 = iVar7 + -1) {
    *(undefined *)param_1 = *(undefined *)puVar4;
    puVar4 = (undefined4 *)((int)puVar4 + 1);
    param_1 = (undefined4 *)((int)param_1 + 1);
  }
  return;
}



// Function at 00408b40

undefined4 __thiscall OOAnalyzer::cls_0x5a486c::meth_0x408b40(cls_0x5a486c *this)

{
  dword dVar1;
  uint in_stack_00000004;
  
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  if ((((-1 < (int)in_stack_00000004) && ((int)in_stack_00000004 <= (int)this->mbr_0x64)) &&
      (this->mbr_0x74 != 0)) &&
     ((in_stack_00000004 < this->mbr_0x64 && (*(int *)(this->mbr_0x74 + in_stack_00000004 * 4) != 0)
      ))) {
    dVar1 = *(dword *)(this->mbr_0x74 + in_stack_00000004 * 4);
    if (dVar1 == 0) {
      dVar1 = this->mbr_0x78;
    }
    return *(undefined4 *)(dVar1 + 0x24);
  }
  return 0;
}



// Function at 00408bb0

void __thiscall
OOAnalyzer::cls_0x5a486c::meth_0x408bb0
          (cls_0x5a486c *this,int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  dword dVar1;
  undefined4 in_stack_00000014;
  
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  dVar1 = *(dword *)(this->mbr_0x74 + param_1 * 4);
  if (dVar1 == 0) {
    dVar1 = this->mbr_0x78;
  }
  meth_0x4089f0(this,param_2,param_3,param_4,in_stack_00000014,*(undefined4 *)(dVar1 + 0x28));
  return;
}



// Function at 00408c20

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall
OOAnalyzer::cls_0x5a486c::meth_0x408c20
          (cls_0x5a486c *this,float param_1,float *param_2,float *param_3,float param_4)

{
  float fVar1;
  int iVar2;
  int iVar3;
  float *pfVar4;
  dword dVar5;
  float in_stack_00000014;
  float local_8;
  float local_4;
  
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  dVar5 = *(dword *)(this->mbr_0x74 + (int)param_1 * 4);
  if (dVar5 == 0) {
    dVar5 = this->mbr_0x78;
  }
  iVar2 = *(int *)(dVar5 + 0x28);
  dVar5 = *(dword *)(this->mbr_0x74 + (int)param_1 * 4);
  if (dVar5 == 0) {
    dVar5 = this->mbr_0x78;
  }
  dVar5 = *(dword *)(dVar5 + 0x24);
  if (iVar2 < 0) {
    iVar2 = 0;
  }
  if ((int)dVar5 < 0) {
    dVar5 = this->mbr_0x24;
  }
  if ((this->mbr_0x1c & 1) == 0) {
    iVar3 = **(int **)this->mbr_0x28;
  }
  else {
    iVar3 = ((int **)this->mbr_0x28)[(int)param_4][(int)in_stack_00000014];
  }
  pfVar4 = (float *)(iVar3 + iVar2 * 0x20);
  in_stack_00000014 = 1000000.0;
  param_4 = 1000000.0;
  param_1 = 1000000.0;
  local_4 = -1000000.0;
  local_8 = -1000000.0;
  fVar1 = _DAT_005a369c;
  if (0 < (int)dVar5) {
    do {
      if (*pfVar4 < param_1) {
        param_1 = *pfVar4;
      }
      if (pfVar4[1] < param_4) {
        param_4 = pfVar4[1];
      }
      if (pfVar4[2] < in_stack_00000014) {
        in_stack_00000014 = pfVar4[2];
      }
      if (local_8 < *pfVar4) {
        local_8 = *pfVar4;
      }
      if (local_4 < pfVar4[1]) {
        local_4 = pfVar4[1];
      }
      if (fVar1 < pfVar4[2]) {
        fVar1 = pfVar4[2];
      }
      pfVar4 = pfVar4 + 8;
      dVar5 = dVar5 - 1;
    } while (dVar5 != 0);
  }
  *param_2 = param_1;
  param_2[1] = param_4;
  param_2[2] = in_stack_00000014;
  *param_3 = local_8;
  param_3[1] = local_4;
  param_3[2] = fVar1;
  return;
}



// Function at 00408d80

dword __thiscall OOAnalyzer::cls_0x5a486c::meth_0x408d80(cls_0x5a486c *this)

{
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  return this->mbr_0x2c;
}



// Function at 00408db0

void __thiscall OOAnalyzer::cls_0x5a486c::meth_0x408db0(cls_0x5a486c *this)

{
  uint uVar1;
  uint uVar2;
  undefined4 *puVar3;
  undefined4 *in_stack_00000004;
  
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  uVar1 = this->mbr_0x2c * 6;
  puVar3 = (undefined4 *)this->mbr_0x30;
  for (uVar2 = uVar1 >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
    *in_stack_00000004 = *puVar3;
    puVar3 = puVar3 + 1;
    in_stack_00000004 = in_stack_00000004 + 1;
  }
  for (uVar1 = uVar1 & 3; uVar1 != 0; uVar1 = uVar1 - 1) {
    *(undefined *)in_stack_00000004 = *(undefined *)puVar3;
    puVar3 = (undefined4 *)((int)puVar3 + 1);
    in_stack_00000004 = (undefined4 *)((int)in_stack_00000004 + 1);
  }
  return;
}



// Function at 00408e00

undefined4 __thiscall OOAnalyzer::cls_0x5a486c::meth_0x408e00(cls_0x5a486c *this)

{
  dword dVar1;
  uint in_stack_00000004;
  
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  if ((((-1 < (int)in_stack_00000004) && ((int)in_stack_00000004 <= (int)this->mbr_0x64)) &&
      (this->mbr_0x74 != 0)) &&
     ((in_stack_00000004 < this->mbr_0x64 && (*(int *)(this->mbr_0x74 + in_stack_00000004 * 4) != 0)
      ))) {
    dVar1 = *(dword *)(this->mbr_0x74 + in_stack_00000004 * 4);
    if (dVar1 == 0) {
      dVar1 = this->mbr_0x78;
    }
    return *(undefined4 *)(dVar1 + 0x2c);
  }
  return 0;
}



// Function at 00408e70

void __thiscall
OOAnalyzer::cls_0x5a486c::meth_0x408e70
          (cls_0x5a486c *this,uint param_1,undefined4 *param_2,undefined4 *param_3)

{
  dword dVar1;
  uint uVar2;
  uint uVar3;
  dword dVar4;
  undefined4 *puVar5;
  undefined4 *in_stack_00000010;
  
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  if ((((-1 < (int)param_1) && ((int)param_1 <= (int)this->mbr_0x64)) && (this->mbr_0x74 != 0)) &&
     ((param_1 < this->mbr_0x64 && (*(int *)(this->mbr_0x74 + param_1 * 4) != 0)))) {
    dVar4 = *(dword *)(this->mbr_0x74 + param_1 * 4);
    if (dVar4 == 0) {
      dVar4 = this->mbr_0x78;
    }
    dVar1 = *(dword *)(this->mbr_0x74 + param_1 * 4);
    if (dVar1 == 0) {
      dVar1 = this->mbr_0x78;
    }
    uVar2 = *(int *)(dVar4 + 0x2c) * 6;
    puVar5 = (undefined4 *)(this->mbr_0x30 + *(int *)(dVar1 + 0x30) * 6);
    for (uVar3 = uVar2 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *param_2 = *puVar5;
      puVar5 = puVar5 + 1;
      param_2 = param_2 + 1;
    }
    for (uVar2 = uVar2 & 3; uVar2 != 0; uVar2 = uVar2 - 1) {
      *(undefined *)param_2 = *(undefined *)puVar5;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
      param_2 = (undefined4 *)((int)param_2 + 1);
    }
    if (param_3 != (undefined4 *)0x0) {
      dVar4 = 0x20;
      if ((int)this->mbr_0x4c < 0x21) {
        dVar4 = this->mbr_0x4c;
      }
      dVar1 = *(dword *)(this->mbr_0x74 + param_1 * 4);
      if (dVar1 == 0) {
        dVar1 = this->mbr_0x78;
      }
      puVar5 = *(undefined4 **)(dVar1 + 0x34);
      for (; dVar4 != 0; dVar4 = dVar4 - 1) {
        *param_3 = *puVar5;
        puVar5 = puVar5 + 1;
        param_3 = param_3 + 1;
      }
    }
    if (in_stack_00000010 != (undefined4 *)0x0) {
      dVar4 = 0x20;
      if ((int)this->mbr_0x4c < 0x21) {
        dVar4 = this->mbr_0x4c;
      }
      dVar1 = *(dword *)(this->mbr_0x74 + param_1 * 4);
      if (dVar1 == 0) {
        dVar1 = this->mbr_0x78;
      }
      puVar5 = *(undefined4 **)(dVar1 + 0x38);
      for (; dVar4 != 0; dVar4 = dVar4 - 1) {
        *in_stack_00000010 = *puVar5;
        puVar5 = puVar5 + 1;
        in_stack_00000010 = in_stack_00000010 + 1;
      }
    }
  }
  return;
}



// Function at 00408f70

dword __thiscall OOAnalyzer::cls_0x5a486c::meth_0x408f70(cls_0x5a486c *this)

{
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  return this->mbr_0x64;
}



// Function at 00409200

dword __thiscall OOAnalyzer::cls_0x5a486c::meth_0x409200(cls_0x5a486c *this)

{
  dword dVar1;
  uint in_stack_00000004;
  
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  if (((((int)in_stack_00000004 < 0) || ((int)this->mbr_0x64 < (int)in_stack_00000004)) ||
      (this->mbr_0x74 == 0)) ||
     ((this->mbr_0x64 <= in_stack_00000004 ||
      (*(int *)(this->mbr_0x74 + in_stack_00000004 * 4) == 0)))) {
    dVar1 = 0;
  }
  else {
    dVar1 = *(dword *)(this->mbr_0x74 + in_stack_00000004 * 4);
    if (dVar1 == 0) {
      return this->mbr_0x78;
    }
  }
  return dVar1;
}



// Function at 004093b0

bool __thiscall OOAnalyzer::cls_0x5a486c::meth_0x4093b0(cls_0x5a486c *this,uint param_1)

{
  dword dVar1;
  int in_stack_00000008;
  
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  if ((((-1 < (int)param_1) && ((int)param_1 <= (int)this->mbr_0x64)) && (this->mbr_0x74 != 0)) &&
     ((param_1 < this->mbr_0x64 && (*(int *)(this->mbr_0x74 + param_1 * 4) != 0)))) {
    dVar1 = *(dword *)(this->mbr_0x74 + param_1 * 4);
    if (dVar1 == 0) {
      dVar1 = this->mbr_0x78;
    }
    return *(int *)(*(int *)(dVar1 + 0x44) + in_stack_00000008 * 4) == 0;
  }
  return true;
}



// Function at 00409430

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall OOAnalyzer::cls_0x5a486c::meth_0x409430(cls_0x5a486c *this,uint param_1)

{
  int iVar1;
  int iVar2;
  undefined uVar3;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  dword dVar7;
  uint *puVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  float *in_stack_00000008;
  float *in_stack_0000000c;
  undefined4 *in_stack_00000010;
  float *pfVar13;
  float *pfVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  int iStack_10;
  int iStack_4;
  
  dVar7 = *(dword *)(this->mbr_0x74 + param_1 * 4);
  if (dVar7 == 0) {
    dVar7 = this->mbr_0x78;
  }
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  pfVar14 = in_stack_00000008;
  (*this->vftptr_0x0->virt_meth_0x4104a0_144)(this);
  uVar3 = (*this->vftptr_0x0->virt_meth_0x4101f0_60)(this);
  if (((float *)CONCAT31(extraout_var,uVar3) < in_stack_00000008) ||
     (pfVar13 = in_stack_00000008, uVar3 = (*this->vftptr_0x0->virt_meth_0x4104a0_144)(this),
     CONCAT31(extraout_var_00,uVar3) <= param_1)) {
    return 0;
  }
  iVar1 = *(int *)(*(int *)(dVar7 + 0x44) + (int)in_stack_00000008 * 4);
  if (iVar1 == 0) {
    in_stack_00000008[2] = 0.0;
    in_stack_00000008[1] = 0.0;
    *in_stack_00000008 = 0.0;
    in_stack_0000000c[2] = 0.0;
    in_stack_0000000c[1] = 0.0;
    *in_stack_0000000c = 0.0;
    return 1;
  }
  if ((this->mbr_0x1c & 0x80) == 0) {
    uVar4 = *(uint *)(iVar1 + 4 + param_1 * 8);
    uVar6 = *(uint *)(iVar1 + param_1 * 8);
    *in_stack_0000000c = (float)((uVar4 >> 4 & 0x1ff) - 0x100) * _DAT_005a36a0;
    in_stack_0000000c[1] = (float)((uVar4 >> 0xd & 0x1ff) - 0x100) * _DAT_005a36a0;
    in_stack_0000000c[2] = (float)((uVar4 >> 0x16 & 0x1ff) - 0x100) * _DAT_005a36a0;
    *in_stack_00000008 = (float)((uVar6 & 0xfff) - 0x800) * _DAT_005a3568;
    uVar4 = __aullshr();
    in_stack_00000008[1] = (float)((uVar4 & 0xfff) - 0x800) * _DAT_005a3568;
    uVar4 = __aullshr();
    in_stack_00000010[2] = 0x3f800000;
    in_stack_00000010[1] = 0x3f800000;
    *in_stack_00000010 = 0x3f800000;
    in_stack_00000008[2] = (float)((uVar4 & 0xfff) - 0x800) * _DAT_005a3568;
    return 1;
  }
  iStack_10 = 0;
  iStack_4 = 0;
  iVar2 = *(int *)(*(int *)(dVar7 + 0x40) + (int)in_stack_00000008 * 4);
  iVar11 = -1;
  iVar15 = -1;
  iVar17 = -1;
  for (; iStack_4 < iVar2; iStack_4 = iStack_4 + iVar10) {
    iVar10 = 0;
    puVar8 = (uint *)(iVar1 + iStack_4 * 4);
    uVar4 = 1;
    uVar6 = 0;
    iVar9 = iStack_4;
    do {
      iVar12 = iVar11;
      iVar16 = iVar15;
      iVar18 = iVar17;
      switch(*puVar8 & 3) {
      case 0:
        uVar5 = *puVar8 >> 2 & 0x3f;
        switch(uVar5) {
        case 0:
          uVar4 = (uint)(uVar6 != 0);
          iVar10 = iVar10 + 1;
          goto LAB_004096ae;
        case 1:
          uVar4 = ((uVar6 != 0) - 1) + ((int)*puVar8 >> 8);
          iVar10 = iVar10 + 1;
          goto LAB_004096ae;
        case 2:
        case 3:
        case 4:
          if (uVar6 != 0) goto LAB_004096ae;
          iVar18 = iVar9;
          if ((uVar5 != 2) && (iVar18 = iVar17, uVar5 == 4)) {
            uVar6 = 1;
          }
          break;
        case 5:
        case 6:
        case 7:
          if ((uVar6 & 6) != 0) goto LAB_004096ae;
          iVar16 = iVar9;
          if ((uVar5 != 5) && (iVar16 = iVar15, uVar5 == 7)) {
            uVar6 = uVar6 | 2;
          }
          break;
        case 8:
        case 9:
        case 10:
          if ((uVar6 & 4) != 0) goto LAB_004096ae;
          iVar12 = iVar9;
          if ((uVar5 != 8) && (iVar12 = iVar11, uVar5 == 10)) goto LAB_00409698;
        }
        break;
      case 1:
        if (uVar6 != 0) goto LAB_004096ae;
        uVar6 = 1;
        iVar18 = iVar9;
        break;
      case 2:
        if ((uVar6 & 6) != 0) goto LAB_004096ae;
        uVar6 = uVar6 | 2;
        iVar16 = iVar9;
        break;
      case 3:
        iVar12 = iVar9;
        if ((uVar6 & 4) != 0) goto LAB_004096ae;
LAB_00409698:
        uVar6 = uVar6 | 4;
      }
      puVar8 = puVar8 + 1;
      iVar10 = iVar10 + 1;
      iVar9 = iVar9 + 1;
      iVar11 = iVar12;
      iVar15 = iVar16;
      iVar17 = iVar18;
    } while (iVar9 < iVar2);
LAB_004096ae:
    if ((iVar10 < 1) || (iStack_10 = iStack_10 + uVar4, (int)param_1 <= iStack_10)) break;
  }
  FUN_004105d0(iStack_4,iVar1,iVar2,iVar17,iVar15,iVar11,in_stack_00000008,in_stack_0000000c,
               in_stack_00000010,pfVar13,pfVar14);
  return 1;
}



// Function at 00409950

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall
OOAnalyzer::cls_0x5a486c::meth_0x409950
          (cls_0x5a486c *this,undefined4 param_1,byte *param_2,float *param_3,float *param_4,
          float *param_5)

{
  byte bVar1;
  uint uVar2;
  float fVar3;
  float *pfVar4;
  undefined uVar5;
  int iVar6;
  byte *pbVar7;
  undefined3 extraout_var;
  float unaff_EBP;
  byte *pbVar8;
  bool bVar9;
  undefined4 unaff_retaddr;
  byte *pbVar10;
  float fStack_24;
  float fStack_20;
  float fStack_1c;
  float fStack_18;
  float fStack_14;
  float fStack_10;
  float fStack_c;
  float fStack_8;
  
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  pbVar10 = param_2;
  iVar6 = meth_0x409430(this,param_1);
  if (iVar6 == 0) {
    return 0;
  }
  if ((((DAT_005d7a14 != 0) &&
       (iVar6 = *(int *)(this->mbr_0x4 + 0x54),
       (*(byte *)(iVar6 + 0x30 + (int)pbVar10 * 0x4c) & 0x40) == 0)) &&
      (uVar2 = this->mbr_0xa4, uVar2 <= *(uint *)(iVar6 + 4))) &&
     ((*(byte *)(iVar6 + 0x30 + uVar2 * 0x4c) & 0x40) == 0)) {
    if (((byte *)this->mbr_0xac == pbVar10) && (this->mbr_0xb0 == uVar2)) {
      pbVar8 = (byte *)this->mbr_0xb4;
      pbVar7 = (byte *)this->mbr_0xb8;
    }
    else {
      this->mbr_0xac = (dword)pbVar10;
      this->mbr_0xb0 = uVar2;
      param_2 = (byte *)0x0;
      meth_0x40c7d0(this,&PTR_null_005c5d40,pbVar10,0xffffffff,&param_2,1,0);
      pbVar8 = param_2;
      this->mbr_0xb4 = (dword)param_2;
      param_2 = (byte *)0x0;
      meth_0x40c7d0(this,&PTR_null_005c5d44,this->mbr_0xa4,0xffffffff,&param_2,1,0);
      this->mbr_0xb8 = (dword)param_2;
      pbVar7 = param_2;
    }
    if (((pbVar8 != (byte *)0x0) && (pbVar7 != (byte *)0x0)) &&
       (*(short *)(*(int *)(this->mbr_0x4 + 0x54) + 0x32 + this->mbr_0xa4 * 0x4c) + -1 <=
        (int)this->mbr_0xa8)) {
      do {
        bVar1 = *pbVar7;
        bVar9 = bVar1 < *pbVar8;
        if (bVar1 != *pbVar8) {
LAB_00409aca:
          iVar6 = (1 - (uint)bVar9) - (uint)(bVar9 != 0);
          goto LAB_00409acf;
        }
        if (bVar1 == 0) break;
        bVar1 = pbVar7[1];
        bVar9 = bVar1 < pbVar8[1];
        if (bVar1 != pbVar8[1]) goto LAB_00409aca;
        pbVar7 = pbVar7 + 2;
        pbVar8 = pbVar8 + 2;
      } while (bVar1 != 0);
      iVar6 = 0;
LAB_00409acf:
      if (iVar6 == 0) {
        return 1;
      }
    }
    uVar5 = (*this->vftptr_0x0->virt_meth_0x4104a0_144)(this);
    if (((-1 < (int)this->mbr_0xa4) && ((int)param_2 < 5)) && (4 < CONCAT31(extraout_var,uVar5))) {
      param_2 = (byte *)((float)(int)(param_2 + 1) * _DAT_005a36d0);
      meth_0x409430(this,unaff_retaddr);
      pfVar4 = param_4;
      FUN_00409820(param_4,&stack0xffffffd8,pbVar10);
      if (((*param_3 != fStack_1c) || (param_3[1] != fStack_18)) || (param_3[2] != fStack_14)) {
        fVar3 = _DAT_005a34e4 - (float)param_2;
        *param_3 = fVar3 * fStack_1c + (float)param_2 * *param_3;
        param_3[1] = (float)param_2 * param_3[1] + fVar3 * fStack_18;
        param_3[2] = (float)param_2 * param_3[2] + fVar3 * fStack_14;
      }
      if (((*pfVar4 != unaff_EBP) || (pfVar4[1] != fStack_24)) || (pfVar4[2] != fStack_20)) {
        fVar3 = _DAT_005a34e4 - (float)param_2;
        *pfVar4 = (float)param_2 * *pfVar4 + fVar3 * unaff_EBP;
        pfVar4[1] = (float)param_2 * pfVar4[1] + fVar3 * fStack_24;
        pfVar4[2] = (float)param_2 * pfVar4[2] + fVar3 * fStack_20;
      }
      if (((*param_5 != fStack_10) || (param_5[1] != fStack_c)) || (param_5[2] != fStack_8)) {
        fVar3 = _DAT_005a34e4 - (float)param_2;
        *param_5 = (float)param_2 * *param_5 + fVar3 * fStack_10;
        param_5[1] = (float)param_2 * param_5[1] + fVar3 * fStack_c;
        param_5[2] = (float)param_2 * param_5[2] + fVar3 * fStack_8;
      }
    }
  }
  return 1;
}



// Function at 00409ca0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint * __thiscall OOAnalyzer::cls_0x5a486c::meth_0x409ca0(cls_0x5a486c *this,char *param_1)

{
  int iVar1;
  char cVar2;
  uint *puVar3;
  uint uVar4;
  int *piVar5;
  uint uVar6;
  int iVar7;
  dword dVar8;
  uint *puVar9;
  dword dVar10;
  uint in_stack_00000008;
  int local_c;
  
  uVar6 = (uint)param_1;
  puVar3 = (uint *)FUN_00482fb0(0x34c);
  puVar9 = puVar3;
  for (iVar7 = 0xd3; iVar7 != 0; iVar7 = iVar7 + -1) {
    *puVar9 = 0;
    puVar9 = puVar9 + 1;
  }
  dVar10 = this->mbr_0x74;
  *puVar3 = in_stack_00000008;
  puVar3[1] = (uint)param_1;
  param_1 = *(char **)(dVar10 + (int)param_1 * 4);
  puVar3[2] = 0;
  if (param_1 == (char *)0x0) {
    param_1 = (char *)this->mbr_0x78;
  }
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  if ((int)(this->mbr_0x64 - 1) <= (int)uVar6) {
    if (this->mbr_0xc == 0) {
      if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
        meth_0x447ac0(this);
      }
      cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
    }
    uVar6 = this->mbr_0x64 - 1;
  }
  puVar3[3] = uVar6;
  uVar6 = *(uint *)(param_1 + 0x3c);
  puVar3[0x26] = 4;
  puVar3[0x27] = 0x112;
  if (((((int)uVar6 < 0) || ((int)this->mbr_0x34 <= (int)uVar6)) || (this->mbr_0x44 == 0)) ||
     ((this->mbr_0x34 <= uVar6 || (*(int *)(this->mbr_0x44 + uVar6 * 4) == 0)))) {
    uVar4 = 0;
  }
  else {
    dVar10 = *(dword *)(this->mbr_0x44 + uVar6 * 4);
    if (dVar10 == 0) {
      dVar10 = this->mbr_0x48;
    }
    uVar4 = *(uint *)(dVar10 + 0x54);
  }
  puVar3[0xd1] = uVar4;
  iVar7 = this->mbr_0x4c + 1;
  if (1 < iVar7) {
    local_c = 0;
    piVar5 = *(int **)(param_1 + 0x38);
    uVar4 = 0;
    do {
      piVar5 = piVar5 + 1;
      if (0 < *piVar5) {
        if ((((int)uVar4 < 0) || ((int)this->mbr_0x4c <= (int)uVar4)) ||
           ((this->mbr_0x5c == 0 ||
            ((this->mbr_0x4c <= uVar4 || (*(int *)(local_c + this->mbr_0x5c) == 0)))))) {
          dVar10 = 0;
        }
        else {
          dVar10 = *(dword *)(this->mbr_0x5c + local_c);
          if (dVar10 == 0) {
            dVar10 = this->mbr_0x60;
          }
        }
        if ((((-1 < (int)uVar6) && ((int)uVar6 < (int)this->mbr_0x34)) && (this->mbr_0x44 != 0)) &&
           ((uVar6 < this->mbr_0x34 && (*(int *)(this->mbr_0x44 + uVar6 * 4) != 0)))) {
          dVar8 = *(dword *)(this->mbr_0x44 + uVar6 * 4);
          if (dVar8 == 0) {
            dVar8 = this->mbr_0x48;
          }
          if (((dVar8 != 0) && (dVar10 != 0)) && (*(int *)(dVar10 + 100) != 0)) {
            if ((((_DAT_005a3530 < *(float *)(dVar8 + 0x34)) &&
                 (_DAT_005a3530 < *(float *)(dVar8 + 0x38))) &&
                (_DAT_005a3530 < *(float *)(dVar8 + 0x3c))) &&
               (_DAT_005a34e4 < *(float *)(dVar8 + 0x40))) {
              puVar3[0xd2] = 4;
              *puVar3 = *puVar3 | 0x1000000;
            }
            puVar3[0xd2] = 4;
            *puVar3 = *puVar3 | 0x1000000;
          }
        }
      }
      local_c = local_c + 4;
      iVar1 = uVar4 + 2;
      uVar4 = uVar4 + 1;
    } while (iVar1 < iVar7);
  }
  if ((in_stack_00000008 & 0x4000) != 0) {
    meth_0x40a0c0(this,puVar3);
  }
  if ((in_stack_00000008 & 0x20000) != 0) {
    if (puVar3[0x2b] != 0) {
      FUN_0040a3d0(puVar3);
    }
    *puVar3 = *puVar3 | 0x70000;
    uVar6 = meth_0x408e00(this);
    puVar3[0x2a] = uVar6;
    uVar6 = FUN_00482fb0(uVar6 * 6);
    puVar3[0x2b] = uVar6;
    meth_0x408e70(this,puVar3[1],uVar6,0);
  }
  cVar2 = *param_1;
  if (((cVar2 < '0') || ('9' < cVar2)) &&
     (((cVar2 < 'a' || ('z' < cVar2)) && ((cVar2 < 'A' || ('Z' < cVar2)))))) {
    iVar7 = FUN_0058ade0(param_1,0x23);
    if (iVar7 != 0) {
      *puVar3 = *puVar3 | 0x20000000;
    }
    iVar7 = FUN_0058ade0(param_1,0x2a);
    if (iVar7 != 0) {
      *puVar3 = *puVar3 | 1;
    }
    iVar7 = FUN_0058ade0(param_1,0x24);
    if (iVar7 != 0) {
      if ((*puVar3 & 0x1000000) == 0) {
        puVar3[0xd2] = 1;
        *puVar3 = *puVar3 | 0x1000000;
      }
      puVar3[0xd2] = puVar3[0xd2] | 0x40;
    }
  }
  return puVar3;
}



// Function at 00409ff0

undefined4 __thiscall OOAnalyzer::cls_0x5a486c::meth_0x409ff0(cls_0x5a486c *this,byte *param_1)

{
  byte bVar1;
  byte *pbVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  byte *pbVar6;
  bool bVar7;
  
  FUN_0059bd3e(param_1);
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  uVar5 = 0;
  if ((int)this->mbr_0x64 < 1) {
    return 0;
  }
  do {
    if (((this->mbr_0x74 != 0) && (uVar5 < this->mbr_0x64)) &&
       (*(int *)(this->mbr_0x74 + uVar5 * 4) != 0)) {
      pbVar2 = *(byte **)(this->mbr_0x74 + uVar5 * 4);
      pbVar6 = param_1;
      if (pbVar2 == (byte *)0x0) {
        pbVar2 = (byte *)this->mbr_0x78;
      }
      do {
        bVar1 = *pbVar2;
        bVar7 = bVar1 < *pbVar6;
        if (bVar1 != *pbVar6) {
LAB_0040a07f:
          iVar3 = (1 - (uint)bVar7) - (uint)(bVar7 != 0);
          goto LAB_0040a084;
        }
        if (bVar1 == 0) break;
        bVar1 = pbVar2[1];
        bVar7 = bVar1 < pbVar6[1];
        if (bVar1 != pbVar6[1]) goto LAB_0040a07f;
        pbVar2 = pbVar2 + 2;
        pbVar6 = pbVar6 + 2;
      } while (bVar1 != 0);
      iVar3 = 0;
LAB_0040a084:
      if (iVar3 == 0) {
        if ((int)uVar5 < 0) {
          return 0;
        }
        uVar4 = meth_0x409ca0(this,uVar5);
        return uVar4;
      }
    }
    uVar5 = uVar5 + 1;
    if ((int)this->mbr_0x64 <= (int)uVar5) {
      return 0;
    }
  } while( true );
}



// Function at 0040a0c0

void __thiscall OOAnalyzer::cls_0x5a486c::meth_0x40a0c0(cls_0x5a486c *this,uint *param_1)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  dword dVar4;
  uint in_stack_00000008;
  
  uVar3 = param_1[0x29];
  if ((uVar3 != 0) && ((*param_1 & 0x8000) != 0)) {
    if ((uVar3 != 0) && (param_1[0x27] == 0x112)) {
      FUN_004830f0(uVar3);
    }
    if ((param_1[0x29] != 0) && (param_1[0x27] == 0x1e2)) {
      FUN_004830f0(param_1[0x29]);
    }
    if ((param_1[0x29] != 0) && (param_1[0x27] == 0x1c4)) {
      FUN_004830f0(param_1[0x29]);
    }
    param_1[0x28] = 0;
    param_1[0x29] = 0;
    *param_1 = *param_1 & 0xffff1fff;
  }
  uVar3 = param_1[1];
  param_1[0x27] = in_stack_00000008;
  dVar4 = this->mbr_0xc;
  *param_1 = *param_1 | 0xe000;
  if (dVar4 == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  if (((((int)uVar3 < 0) || ((int)this->mbr_0x64 < (int)uVar3)) || (this->mbr_0x74 == 0)) ||
     ((this->mbr_0x64 <= uVar3 || (*(int *)(this->mbr_0x74 + uVar3 * 4) == 0)))) {
    uVar3 = 0;
  }
  else {
    dVar4 = *(dword *)(this->mbr_0x74 + uVar3 * 4);
    if (dVar4 == 0) {
      dVar4 = this->mbr_0x78;
    }
    uVar3 = *(uint *)(dVar4 + 0x24);
  }
  uVar1 = param_1[0x27];
  param_1[0x28] = uVar3;
  if (((uVar1 == 0x112) || (uVar1 == 0x1e2)) || (uVar1 == 0x1c4)) {
    uVar3 = FUN_00482fb0(uVar3 << 5);
    param_1[0x29] = uVar3;
  }
  uVar3 = param_1[0x27];
  uVar1 = param_1[0x29];
  uVar2 = param_1[1];
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
  }
  dVar4 = *(dword *)(this->mbr_0x74 + uVar2 * 4);
  if (dVar4 == 0) {
    dVar4 = this->mbr_0x78;
  }
  meth_0x4089f0(this,uVar1,0,0,uVar3,*(undefined4 *)(dVar4 + 0x28));
  return;
}



// Function at 0040a420

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall
OOAnalyzer::cls_0x5a486c::meth_0x40a420
          (cls_0x5a486c *this,uint *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4
          )

{
  float *pfVar1;
  float fVar2;
  byte bVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  undefined4 *puVar7;
  uint *puVar8;
  uint *puVar9;
  undefined4 *puVar10;
  uint *puVar11;
  int in_stack_00000014;
  uint local_80 [12];
  uint local_50;
  uint local_4c;
  uint local_48;
  undefined4 local_40 [4];
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_20;
  undefined4 local_18;
  
  puVar11 = param_1 + 0x16;
  FUN_0043a9f0(puVar11);
  uVar6 = *param_1;
  if (((this->mbr_0x1c & 1) == 0) && (((uVar6 & 0xfc) == 0 || ((uVar6 & 0x200) != 0)))) {
    if ((uVar6 & 2) == 0) {
      uVar4 = param_1[1];
    }
    else {
      uVar4 = param_1[3];
    }
    pfVar1 = (float *)(param_1 + 0x10);
    iVar5 = meth_0x409950(this,uVar4,param_2,param_3,param_1 + 4,param_1 + 10);
    if (iVar5 != 0) {
      if (*pfVar1 <= _DAT_005a36d4) {
        return 0;
      }
      if ((float)param_1[0x11] <= _DAT_005a36d4) {
        return 0;
      }
      if ((float)param_1[0x12] <= _DAT_005a36d4) {
        return 0;
      }
      fVar2 = (float)param_1[10];
      if (fVar2 != _DAT_005a3530) {
        FUN_0043b180(puVar11,fVar2);
      }
      if ((float)param_1[0xb] != _DAT_005a3530) {
        FUN_0043b1d0(puVar11,param_1[0xb]);
      }
      if ((float)param_1[0xc] != _DAT_005a3530) {
        FUN_0043b230(puVar11,param_1[0xc]);
      }
      if (((*pfVar1 != 1.0) || (param_1[0x11] != 0x3f800000)) || (param_1[0x12] != 0x3f800000)) {
        FUN_0043b130(puVar11,pfVar1);
      }
      param_1[0x22] = param_1[4];
      param_1[0x23] = param_1[5];
      param_1[0x24] = param_1[6];
    }
  }
  if ((uVar6 & 0xfc) != 0) {
    uVar6 = *param_1 & 0xc0;
    if ((uVar6 != 0) &&
       ((((float)param_1[0x10] <= _DAT_005a36d4 || ((float)param_1[0x11] <= _DAT_005a36d4)) ||
        ((float)param_1[0x12] <= _DAT_005a36d4)))) {
      return 0;
    }
    bVar3 = (byte)*param_1;
    if ((bVar3 & 0xc) == 4) {
      FUN_0043b090(puVar11,param_1 + 4);
    }
    else if ((bVar3 & 0x30) == 0x10) {
      FUN_0043b180(puVar11,param_1[10]);
      FUN_0043b1d0(puVar11,param_1[0xb]);
      FUN_0043b230(puVar11,param_1[0xc]);
    }
    else if (uVar6 == 0x40) {
      FUN_0043b130(puVar11,param_1 + 0x10);
    }
    bVar3 = (byte)*param_1;
    if ((bVar3 & 0xc) == 8) {
      FUN_0043b090(puVar11,param_1 + 4);
    }
    else if ((bVar3 & 0x30) == 0x20) {
      FUN_0043b180(puVar11,param_1[10]);
      FUN_0043b1d0(puVar11,param_1[0xb]);
      FUN_0043b230(puVar11,param_1[0xc]);
    }
    else if ((bVar3 & 0xc0) == 0x80) {
      FUN_0043b130(puVar11,param_1 + 0x10);
    }
    bVar3 = (byte)*param_1;
    if ((bVar3 & 0xc) == 0xc) {
      FUN_0043b090(puVar11,param_1 + 4);
    }
    else if ((bVar3 & 0x30) == 0x30) {
      FUN_0043b180(puVar11,param_1[10]);
      FUN_0043b1d0(puVar11,param_1[0xb]);
      FUN_0043b230(puVar11,param_1[0xc]);
    }
    else if ((bVar3 & 0xc0) == 0xc0) {
      FUN_0043b130(puVar11,param_1 + 0x10);
    }
  }
  if (param_1[2] != 0) {
    if (in_stack_00000014 != 0) {
      meth_0x40a420(this,(uint *)param_1[2],param_2,param_3,param_4);
    }
    FUN_0043aa90(puVar11,puVar11,param_1[2] + 0x58);
  }
  if (((*param_1 & 0x20000000) != 0) && (DAT_006671f0 == 0)) {
    puVar7 = &DAT_005c5ca0;
    puVar10 = local_40;
    for (iVar5 = 0x10; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar10 = *puVar7;
      puVar7 = puVar7 + 1;
      puVar10 = puVar10 + 1;
    }
    puVar8 = &DAT_005c5ca0;
    puVar9 = local_80;
    for (iVar5 = 0x10; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar9 = *puVar8;
      puVar8 = puVar8 + 1;
      puVar9 = puVar9 + 1;
    }
    FUN_0043b130(local_80,param_1 + 0x10);
    FUN_0043b180(local_80,param_1[10]);
    FUN_0043b1d0(local_80,param_1[0xb]);
    FUN_0043b230(local_80,param_1[0xc]);
    local_40[0] = 0x3f51b3f2;
    local_40[2] = 0xbf12d5e0;
    local_20 = 0x3f12d5e0;
    local_18 = 0x3f51b3f2;
    FUN_0043aa90(local_80,local_80,local_40);
    puVar7 = &DAT_005c5ca0;
    puVar10 = local_40;
    for (iVar5 = 0x10; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar10 = *puVar7;
      puVar7 = puVar7 + 1;
      puVar10 = puVar10 + 1;
    }
    local_40[0] = 0xbf3504e6;
    local_40[1] = 0x3f3504e6;
    local_30 = 0xbf3504e6;
    local_2c = 0xbf3504e6;
    FUN_0043aa90(local_80,local_80,local_40);
    local_48 = param_1[0x24];
    local_50 = param_1[0x22];
    local_4c = param_1[0x23];
    puVar8 = local_80;
    for (iVar5 = 0x10; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar11 = *puVar8;
      puVar8 = puVar8 + 1;
      puVar11 = puVar11 + 1;
    }
  }
  return 1;
}



// Function at 0040a8f0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall OOAnalyzer::cls_0x5a486c::meth_0x40a8f0(cls_0x5a486c *this)

{
  undefined uVar1;
  undefined3 extraout_var;
  int iVar2;
  dword dVar3;
  uint uVar4;
  uint uVar5;
  dword dVar6;
  int iVar7;
  uint uVar8;
  uint *puVar9;
  uint uVar10;
  uint *puVar11;
  uint uVar12;
  uint uVar13;
  uint *in_stack_00000004;
  int in_stack_00000008;
  int in_stack_0000000c;
  undefined4 in_stack_00000010;
  int in_stack_00000014;
  int in_stack_00000018;
  char *pcVar14;
  undefined4 uVar15;
  uint uStack_b4;
  dword dStack_b0;
  uint uStack_a8;
  uint uStack_98;
  uint uStack_94;
  uint uStack_90;
  undefined4 uStack_8c;
  undefined4 uStack_88;
  undefined4 uStack_84;
  undefined auStack_80 [64];
  uint auStack_40 [16];
  
  uVar4 = in_stack_00000004[1];
  if ((int)uVar4 < 0) {
    return 0;
  }
  if ((int)this->mbr_0x64 < (int)uVar4) {
    return 0;
  }
  if (this->mbr_0x74 == 0) {
    return 0;
  }
  if (this->mbr_0x64 <= uVar4) {
    return 0;
  }
  if (*(int *)(this->mbr_0x74 + uVar4 * 4) == 0) {
    return 0;
  }
  uVar1 = (*this->vftptr_0x0->virt_meth_0x4101f0_60)(this);
  if (CONCAT31(extraout_var,uVar1) <= in_stack_00000008) {
    return 0;
  }
  if (this->mbr_0x10 != 0) {
    if (DAT_005c619c == 0) {
      if (0 < (int)this->mbr_0x4c) {
LAB_0040a970:
        cls_0x5a35ac::meth_0x4085f0((cls_0x5a35ac *)this);
      }
    }
    else if ((int)this->mbr_0x4c < 1) goto LAB_0040a970;
  }
  if (this->mbr_0xc == 0) {
    if (*(int *)(this->mbr_0x4 + 0x60) == 0) {
      meth_0x447ac0(this);
    }
    iVar2 = cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this);
    if (iVar2 == 0) {
      return 0;
    }
  }
  dStack_b0 = *(dword *)(this->mbr_0x74 + uVar4 * 4);
  if (dStack_b0 == 0) {
    dStack_b0 = this->mbr_0x78;
  }
  if ((((*in_stack_00000004 & 0x100) == 0) && (in_stack_00000018 == 0)) &&
     (iVar2 = meth_0x40a420(this,in_stack_00000004,in_stack_00000008,in_stack_0000000c,
                            in_stack_00000010), iVar2 == 0)) {
    return 1;
  }
  uVar4 = *in_stack_00000004;
  if (((uVar4 & 0x100) == 0) &&
     ((((float)in_stack_00000004[0x10] < _DAT_005a36d4 ||
       ((float)in_stack_00000004[0x11] < _DAT_005a36d4)) ||
      ((float)in_stack_00000004[0x12] < _DAT_005a36d4)))) {
    return 1;
  }
  if ((uVar4 & 1) != 0) {
    return 1;
  }
  if (((this->mbr_0x1c & 1) == 0) || ((uVar4 & 0x1fc) != 0)) {
    if ((uVar4 & 0x400000) == 0) {
      if ((DAT_005e8984 == 0) && (DAT_005e8794 == 0)) {
        FUN_0043aa90(auStack_80,in_stack_00000004 + 0x16,in_stack_00000010);
        iVar2 = FUN_00417430(1,auStack_80);
        if (iVar2 == 0) goto LAB_0040ab92;
        uVar15 = 0x853;
        pcVar14 = s_d__revenant_3DImage_cpp_005c5d78;
      }
      else {
        puVar9 = in_stack_00000004 + 0x16;
        puVar11 = auStack_40;
        for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
          *puVar11 = *puVar9;
          puVar9 = puVar9 + 1;
          puVar11 = puVar11 + 1;
        }
        uStack_8c = _DAT_005a36d8;
        if (DAT_005e8794 != 0) {
          uStack_8c = _DAT_005a36dc;
        }
        uStack_88 = uStack_8c;
        uStack_84 = uStack_8c;
        FUN_0043b130(auStack_40,&uStack_8c);
        FUN_0043aa90(auStack_80,auStack_40,in_stack_00000010);
        iVar2 = FUN_00417430(1,auStack_80);
        if (iVar2 == 0) goto LAB_0040ab92;
        uVar15 = 0x84e;
        pcVar14 = s_d__revenant_3DImage_cpp_005c5d60;
      }
    }
    else {
      iVar2 = FUN_00417430(1,in_stack_00000004 + 0x16);
      if (iVar2 == 0) goto LAB_0040ab92;
      uVar15 = 0x840;
      pcVar14 = s_d__revenant_3DImage_cpp_005c5d48;
    }
  }
  else {
    iVar2 = FUN_00417430(1,in_stack_00000010);
    if (iVar2 == 0) goto LAB_0040ab92;
    uVar15 = 0x862;
    pcVar14 = s_d__revenant_3DImage_cpp_005c5d90;
  }
  FUN_004a90d0(iVar2,pcVar14,uVar15);
LAB_0040ab92:
  dVar3 = *(dword *)(this->mbr_0x44 + *(int *)(dStack_b0 + 0x3c) * 4);
  if (dVar3 == 0) {
    dVar3 = this->mbr_0x48;
  }
  if ((*in_stack_00000004 & 0x80000) == 0) {
    uVar4 = *(uint *)(dVar3 + 0x54);
  }
  else {
    uVar4 = in_stack_00000004[0xd1];
  }
  iVar2 = FUN_00417410(1,&uStack_94);
  if (iVar2 != 0) {
    FUN_004a90d0(iVar2,s_d__revenant_3DImage_cpp_005c5da8,0x870);
  }
  if ((uStack_94 != uVar4) && (iVar2 = FUN_004173f0(1,uVar4), iVar2 != 0)) {
    FUN_004a90d0(iVar2,s_d__revenant_3DImage_cpp_005c5dc0,0x872);
  }
  iVar2 = 0;
  if (this->mbr_0x4c != 0xffffffff && -1 < (int)(this->mbr_0x4c + 1)) {
    do {
      if ((in_stack_00000014 < 0) || (in_stack_00000014 == iVar2)) {
        if ((*in_stack_00000004 & 0x2000) == 0) {
          uStack_b4 = *(uint *)(dStack_b0 + 0x24);
          uStack_a8 = 0x112;
          if ((this->mbr_0x1c & 1) == 0) {
            uVar4 = **(int **)this->mbr_0x28 + *(int *)(dStack_b0 + 0x28) * 0x20;
          }
          else {
            uVar4 = *(int *)(this->mbr_0x28 + in_stack_00000008 * 4) +
                    (*(int *)(dStack_b0 + 0x28) + in_stack_0000000c) * 4;
          }
        }
        else {
          uStack_b4 = in_stack_00000004[0x28];
          uStack_a8 = in_stack_00000004[0x27];
          uVar4 = in_stack_00000004[0x29];
        }
        if ((*in_stack_00000004 & 0x10000) == 0) {
          uVar8 = *(uint *)(*(int *)(dStack_b0 + 0x38) + iVar2 * 4);
          uVar5 = *(int *)(*(int *)(dStack_b0 + 0x34) + iVar2 * 4) + *(int *)(dStack_b0 + 0x30);
          uVar12 = this->mbr_0x30;
        }
        else {
          uVar5 = in_stack_00000004[iVar2 + 0x2c];
          uVar8 = in_stack_00000004[iVar2 + 0x4d];
          uVar12 = in_stack_00000004[0x2b];
        }
        if ((0 < (int)uStack_b4) && (0 < (int)uVar8)) {
          if (this->mbr_0x10 == 0) {
LAB_0040adb1:
            uVar10 = 0;
            uVar13 = 0;
          }
          else {
            if (((DAT_005c619c != 0) && (0 < iVar2)) && ((*in_stack_00000004 & 0x200000) == 0)) {
              meth_0x40c520(this,iVar2 + -1);
            }
            if ((this->mbr_0x10 == 0) || (DAT_005c619c == 0)) goto LAB_0040adb1;
            if ((*in_stack_00000004 & 0x200000) == 0) {
              if (iVar2 == 0) goto LAB_0040adb1;
              dVar6 = *(dword *)((this->mbr_0x5c - 4) + iVar2 * 4);
              if (dVar6 == 0) {
                dVar6 = this->mbr_0x60;
              }
              uVar10 = *(uint *)(dVar6 + 0x94);
              dVar6 = *(dword *)((this->mbr_0x5c - 4) + iVar2 * 4);
              if (dVar6 == 0) {
                dVar6 = this->mbr_0x60;
              }
              uVar13 = *(uint *)(dVar6 + 0x90);
            }
            else {
              uVar10 = in_stack_00000004[iVar2 + 0x8f];
              uVar13 = in_stack_00000004[iVar2 + 0xb0];
            }
          }
          FUN_00417320(0,&uStack_98,&uStack_90);
          if (((uStack_98 != uVar10) || (uStack_90 != uVar13)) &&
             (iVar7 = FUN_004172b0(0,uVar10,uVar13), iVar7 != 0)) {
            FUN_004a90d0(iVar7,s_d__revenant_3DImage_cpp_005c5dd8,0x8cc);
          }
          if (((*in_stack_00000004 & 0x1000000) != 0) &&
             (uVar10 = in_stack_00000004[0xd2], uVar10 != 0)) {
            if ((DAT_005e91c0 == 0) ||
               (((*(float *)(dVar3 + 0x34) == _DAT_005a3530 &&
                 (*(float *)(dVar3 + 0x38) == _DAT_005a3530)) &&
                (*(float *)(dVar3 + 0x3c) == _DAT_005a3530)))) {
              iVar7 = cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),uVar10);
              if (iVar7 != 0) {
                FUN_004a90d0(iVar7,s_d__revenant_3DImage_cpp_005c5e08,0x8de);
              }
            }
            else {
              DAT_005e91c0 = 0;
              iVar7 = cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),uVar10);
              if (iVar7 != 0) {
                FUN_004a90d0(iVar7,s_d__revenant_3DImage_cpp_005c5df0,0x8d9);
              }
              DAT_005e91c0 = 1;
            }
          }
          iVar7 = FUN_004174b0(4,uStack_a8,uVar4,uStack_b4,uVar12 + uVar5 * 6,uVar8 * 3,0);
          if (iVar7 != 0) {
            FUN_004a90d0(iVar7,s_d__revenant_3DImage_cpp_005c5e20,0x8ea);
          }
        }
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)(this->mbr_0x4c + 1));
  }
  return 1;
}



// Function at 0040af50

/* WARNING: Type propagation algorithm not settling */

void __thiscall
OOAnalyzer::cls_0x5a486c::meth_0x40af50
          (cls_0x5a486c *this,int param_1,char *param_2,undefined4 *param_3)

{
  char cVar1;
  int *piVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined uVar5;
  char cVar6;
  int iVar7;
  undefined4 uVar8;
  uint uVar9;
  uint uVar10;
  undefined4 *puVar11;
  char *pcVar12;
  undefined4 *puVar13;
  undefined4 *in_stack_00000010;
  char *pcVar14;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  char local_3c [19];
  undefined local_29;
  char local_28;
  char local_27 [39];
  
  if (param_1 == 0) {
    return;
  }
  uVar9 = 0xffffffff;
  pcVar14 = param_2;
  do {
    pcVar12 = pcVar14;
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    pcVar12 = pcVar14 + 1;
    cVar6 = *pcVar14;
    pcVar14 = pcVar12;
  } while (cVar6 != '\0');
  uVar9 = ~uVar9;
  puVar11 = (undefined4 *)(pcVar12 + -uVar9);
  puVar13 = (undefined4 *)&local_28;
  for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
    *puVar13 = *puVar11;
    puVar11 = puVar11 + 1;
    puVar13 = puVar13 + 1;
  }
  for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
    *(undefined *)puVar13 = *(undefined *)puVar11;
    puVar11 = (undefined4 *)((int)puVar11 + 1);
    puVar13 = (undefined4 *)((int)puVar13 + 1);
  }
  puVar11 = (undefined4 *)&local_28;
  local_28 = FUN_0058b266((int)local_28);
  cVar6 = local_28;
  while (cVar6 != '\0') {
    pcVar14 = (char *)((int)puVar11 + 1);
    puVar11 = (undefined4 *)((int)puVar11 + 1);
    cVar6 = FUN_0058b266((int)*pcVar14);
    *(char *)puVar11 = cVar6;
  }
  iVar7 = FUN_0058ad30(&local_28,&DAT_005c5e38);
  if (iVar7 == 0) {
    puVar11 = (undefined4 *)FUN_0058ad30(&local_28,&PTR_LAB_005c5e70);
    if (puVar11 == (undefined4 *)&local_28) {
      uVar9 = 0xffffffff;
      pcVar14 = param_2;
      do {
        pcVar12 = pcVar14;
        if (uVar9 == 0) break;
        uVar9 = uVar9 - 1;
        pcVar12 = pcVar14 + 1;
        cVar6 = *pcVar14;
        pcVar14 = pcVar12;
      } while (cVar6 != '\0');
      uVar9 = ~uVar9;
      puVar11 = (undefined4 *)(pcVar12 + -uVar9);
      puVar13 = param_3;
      for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
        *puVar13 = *puVar11;
        puVar11 = puVar11 + 1;
        puVar13 = puVar13 + 1;
      }
      for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
        *(undefined *)puVar13 = *(undefined *)puVar11;
        puVar11 = (undefined4 *)((int)puVar11 + 1);
        puVar13 = (undefined4 *)((int)puVar13 + 1);
      }
      uVar9 = 0xffffffff;
      puVar11 = param_3;
      do {
        if (uVar9 == 0) break;
        uVar9 = uVar9 - 1;
        cVar6 = *(char *)puVar11;
        puVar11 = (undefined4 *)((int)puVar11 + 1);
      } while (cVar6 != '\0');
      iVar7 = FUN_0058ae9c((int)*(char *)((~uVar9 - 2) + (int)param_3));
      if (iVar7 != 0) {
        return;
      }
      piVar2 = *(int **)(param_1 + 0xe0);
      if ((piVar2 != (int *)0x0) &&
         (((*piVar2 == 3 || ((piVar2 != (int *)0x0 && (*piVar2 == 0x19)))) &&
          ((int *)piVar2[0x11] != (int *)0x0)))) {
        uVar8 = (**(code **)(*(int *)piVar2[0x11] + 0x2cc))();
        uVar5 = DAT_005c5e9c;
        uVar4 = s_sword_005c5e7c._4_2_;
        uVar3 = s_staff_005c5e74._4_2_;
        switch(uVar8) {
        case 0:
          iVar7 = -1;
          break;
        case 1:
        case 4:
          iVar7 = -1;
          do {
            puVar11 = param_3;
            if (iVar7 == 0) break;
            iVar7 = iVar7 + -1;
            puVar11 = (undefined4 *)((int)param_3 + 1);
            cVar6 = *(char *)param_3;
            param_3 = puVar11;
          } while (cVar6 != '\0');
          *(undefined4 *)((int)puVar11 + -1) = s_sword_005c5e7c._0_4_;
          *(undefined2 *)((int)puVar11 + 3) = uVar4;
          return;
        case 2:
          iVar7 = FUN_00483300(0,5);
          cVar6 = s_bigsword_005c5e8c[8];
          uVar8 = s_bigsword_005c5e8c._4_4_;
          uVar3 = s_sword_005c5e84._4_2_;
          if (iVar7 != 0) {
            iVar7 = -1;
            do {
              puVar11 = param_3;
              if (iVar7 == 0) break;
              iVar7 = iVar7 + -1;
              puVar11 = (undefined4 *)((int)param_3 + 1);
              cVar1 = *(char *)param_3;
              param_3 = puVar11;
            } while (cVar1 != '\0');
            *(undefined4 *)((int)puVar11 + -1) = s_bigsword_005c5e8c._0_4_;
            *(undefined4 *)((int)puVar11 + 3) = uVar8;
            *(char *)((int)puVar11 + 7) = cVar6;
            return;
          }
          iVar7 = -1;
          do {
            puVar11 = param_3;
            if (iVar7 == 0) break;
            iVar7 = iVar7 + -1;
            puVar11 = (undefined4 *)((int)param_3 + 1);
            cVar6 = *(char *)param_3;
            param_3 = puVar11;
          } while (cVar6 != '\0');
          *(undefined4 *)((int)puVar11 + -1) = s_sword_005c5e84._0_4_;
          *(undefined2 *)((int)puVar11 + 3) = uVar3;
          return;
        case 3:
        case 5:
          iVar7 = -1;
          do {
            puVar11 = param_3;
            if (iVar7 == 0) break;
            iVar7 = iVar7 + -1;
            puVar11 = (undefined4 *)((int)param_3 + 1);
            cVar6 = *(char *)param_3;
            param_3 = puVar11;
          } while (cVar6 != '\0');
          *(undefined4 *)((int)puVar11 + -1) = s_staff_005c5e74._0_4_;
          *(undefined2 *)((int)puVar11 + 3) = uVar3;
          return;
        case 6:
        case 7:
          iVar7 = -1;
          do {
            puVar11 = param_3;
            if (iVar7 == 0) break;
            iVar7 = iVar7 + -1;
            puVar11 = (undefined4 *)((int)param_3 + 1);
            cVar6 = *(char *)param_3;
            param_3 = puVar11;
          } while (cVar6 != '\0');
          *(undefined4 *)((int)puVar11 + -1) = DAT_005c5ea0;
          return;
        default:
          return;
        }
        do {
          puVar11 = param_3;
          if (iVar7 == 0) break;
          iVar7 = iVar7 + -1;
          puVar11 = (undefined4 *)((int)param_3 + 1);
          cVar6 = *(char *)param_3;
          param_3 = puVar11;
        } while (cVar6 != '\0');
        *(undefined4 *)((int)puVar11 + -1) = DAT_005c5e98;
        *(undefined *)((int)puVar11 + 3) = uVar5;
        return;
      }
    }
    else {
      puVar11 = (undefined4 *)FUN_0058ad30(&local_28,s_BLOCK_005c5ea4);
      if (puVar11 == (undefined4 *)&local_28) {
        *(undefined *)param_3 = 0;
        return;
      }
    }
  }
  else if ((*(short *)(param_1 + 4) == 0xc) || (*(short *)(param_1 + 4) == 0xb)) {
    local_48 = *(undefined4 *)(param_1 + 0x10);
    local_44 = *(undefined4 *)(param_1 + 0x14);
    local_40 = *(undefined4 *)(param_1 + 0x18);
    uVar9 = FUN_00452ea0(&local_48,*(undefined2 *)(param_1 + 0xe));
    uVar9 = uVar9 & 0xf;
    if (uVar9 == 5) {
      pcVar14 = &DAT_005c5e40;
    }
    else if (uVar9 == 2) {
      pcVar14 = s_stone_005c5e48;
    }
    else if (uVar9 == 8) {
      pcVar14 = s_carpet_005c5e50;
    }
    else if (uVar9 == 4) {
      pcVar14 = s_grass_005c5e58;
    }
    else {
      pcVar14 = &DAT_005c5e60;
    }
    _strncpy(local_3c,pcVar14,0x13);
    local_29 = 0;
    FUN_0058b100(param_3,&DAT_005c5e68,param_2,local_3c);
    if (in_stack_00000010 == (undefined4 *)0x0) {
      return;
    }
    if ((*(int *)(param_1 + 0xe0) != 0) && (iVar7 = FUN_004dab80(s_sneak_005c618c), iVar7 != 0)) {
      *in_stack_00000010 = 0x40;
      return;
    }
    *in_stack_00000010 = 0x5c;
    return;
  }
  FUN_0058b100(param_3,&DAT_005c5eac,param_2);
  return;
}



// Function at 0040b2e0

void __thiscall
OOAnalyzer::cls_0x5a486c::meth_0x40b2e0(cls_0x5a486c *this,cls_0x5a7b98 *param_1,dword param_2)

{
  dword dVar1;
  dword *pdVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  char *pcVar6;
  char *pcVar7;
  bool bVar8;
  int in_stack_0000000c;
  undefined *puVar9;
  int local_90;
  int local_8c;
  dword local_88;
  undefined4 local_84;
  undefined local_80 [128];
  
  local_88 = this->mbr_0xc0;
  local_84 = 0x7f;
  if (param_2 != this->mbr_0xbc) {
    dVar1 = 0;
    if (0 < (int)this->mbr_0x7c) {
      do {
        pdVar2 = *(dword **)(this->mbr_0x8c + dVar1 * 4);
        if (pdVar2 == (dword *)0x0) {
          pdVar2 = (dword *)this->mbr_0x90;
        }
        if (*pdVar2 == param_2) {
          this->mbr_0xc0 = dVar1;
          this->mbr_0xbc = param_2;
          local_88 = dVar1;
          break;
        }
        dVar1 = dVar1 + 1;
      } while ((int)dVar1 < (int)this->mbr_0x7c);
    }
    if (this->mbr_0xbc != param_2) {
      return;
    }
  }
  local_8c = 0;
  if (this->mbr_0x7c != local_88 && -1 < (int)(this->mbr_0x7c - local_88)) {
    local_90 = local_88 * 4;
    do {
      pdVar2 = *(dword **)(this->mbr_0x8c + local_90);
      if (pdVar2 == (dword *)0x0) {
        pdVar2 = (dword *)this->mbr_0x90;
      }
      if (*pdVar2 != param_2) {
        return;
      }
      if ((int)(in_stack_0000000c + 1U) < (int)pdVar2[1]) {
        return;
      }
      iVar5 = 5;
      bVar8 = true;
      pcVar6 = (char *)pdVar2[2];
      pcVar7 = &DAT_005c5eb0;
      do {
        if (iVar5 == 0) break;
        iVar5 = iVar5 + -1;
        bVar8 = *pcVar6 == *pcVar7;
        pcVar6 = pcVar6 + 1;
        pcVar7 = pcVar7 + 1;
      } while (bVar8);
      if ((bVar8) && (pdVar2[1] == in_stack_0000000c + 1U)) {
        if (param_1 == (cls_0x5a7b98 *)0x0) {
          FUN_004833c0(pdVar2[3],0,0);
          iVar5 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
          if ((-1 < iVar5) &&
             (iVar3 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548), iVar3 != 0)) {
            cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar5,0x7f,1);
          }
        }
        else {
          puVar9 = local_80;
          uVar4 = FUN_004833c0(pdVar2[3],0,0,puVar9,&local_84);
          meth_0x40af50(this,param_1,uVar4,puVar9);
          cls_0x5a7b98::meth_0x473990(param_1,(char)local_80,0);
        }
      }
      local_8c = local_8c + 1;
      local_90 = local_90 + 4;
    } while (local_8c < (int)(this->mbr_0x7c - local_88));
  }
  return;
}



// Function at 0040c520

undefined4 __thiscall OOAnalyzer::cls_0x5a486c::meth_0x40c520(cls_0x5a486c *this,uint param_1)

{
  dword dVar1;
  dword dVar2;
  int iVar3;
  int in_stack_00000008;
  
  if (((((int)param_1 < 0) || ((int)this->mbr_0x4c <= (int)param_1)) || (this->mbr_0x5c == 0)) ||
     ((this->mbr_0x4c <= param_1 || (*(int *)(this->mbr_0x5c + param_1 * 4) == 0)))) {
    return 0;
  }
  dVar1 = *(dword *)(this->mbr_0x5c + param_1 * 4);
  if (dVar1 == 0) {
    dVar1 = this->mbr_0x60;
  }
  if (1 < *(int *)(dVar1 + 0x7c)) {
    dVar1 = *(dword *)(this->mbr_0x5c + param_1 * 4);
    if (dVar1 == 0) {
      dVar1 = this->mbr_0x60;
    }
    if (*(int *)(dVar1 + 0x7c) < in_stack_00000008) {
      dVar1 = *(dword *)(this->mbr_0x5c + param_1 * 4);
      if (dVar1 == 0) {
        dVar1 = this->mbr_0x60;
      }
      in_stack_00000008 = in_stack_00000008 % *(int *)(dVar1 + 0x7c);
    }
    dVar1 = *(dword *)(this->mbr_0x5c + param_1 * 4);
    if (dVar1 == 0) {
      dVar1 = this->mbr_0x60;
    }
    if (*(int *)(dVar1 + 0x80) != in_stack_00000008) {
      dVar1 = *(dword *)(this->mbr_0x5c + param_1 * 4);
      if (dVar1 == 0) {
        dVar1 = this->mbr_0x60;
      }
      if (*(int *)(dVar1 + 0x8c) == 0) {
        dVar1 = *(dword *)(this->mbr_0x5c + param_1 * 4);
        if (dVar1 == 0) {
          dVar1 = this->mbr_0x60;
        }
        dVar2 = *(dword *)(this->mbr_0x5c + param_1 * 4);
        if (dVar2 == 0) {
          dVar2 = this->mbr_0x60;
        }
        *(undefined4 *)(dVar2 + 0x90) =
             *(undefined4 *)(*(int *)(dVar1 + 0x84) + in_stack_00000008 * 4);
        dVar1 = *(dword *)(this->mbr_0x5c + param_1 * 4);
        if (dVar1 == 0) {
          dVar1 = this->mbr_0x60;
        }
        dVar2 = *(dword *)(this->mbr_0x5c + param_1 * 4);
        if (dVar2 == 0) {
          dVar2 = this->mbr_0x60;
        }
        *(undefined4 *)(dVar2 + 0x94) =
             *(undefined4 *)(*(int *)(dVar1 + 0x88) + in_stack_00000008 * 4);
        iVar3 = *(int *)(this->mbr_0x5c + param_1 * 4);
        if (iVar3 != 0) {
          *(int *)(iVar3 + 0x80) = in_stack_00000008;
          return 1;
        }
      }
      else {
        dVar1 = *(dword *)(this->mbr_0x5c + param_1 * 4);
        if (dVar1 == 0) {
          dVar1 = this->mbr_0x60;
        }
        dVar2 = *(dword *)(this->mbr_0x5c + param_1 * 4);
        if (dVar2 == 0) {
          dVar2 = this->mbr_0x60;
        }
        iVar3 = (**(code **)(**(int **)(dVar2 + 0x94) + 0x14))
                          (*(int **)(dVar2 + 0x94),
                           *(undefined4 *)(*(int *)(dVar1 + 0x88) + in_stack_00000008 * 4));
        if (iVar3 != 0) {
          return 0;
        }
        iVar3 = *(int *)(this->mbr_0x5c + param_1 * 4);
        if (iVar3 != 0) {
          *(int *)(iVar3 + 0x80) = in_stack_00000008;
          return 1;
        }
      }
      *(int *)(this->mbr_0x60 + 0x80) = in_stack_00000008;
      return 1;
    }
    return 1;
  }
  return 1;
}



// Function at 0040c7d0

int __thiscall
OOAnalyzer::cls_0x5a486c::meth_0x40c7d0
          (cls_0x5a486c *this,byte *param_1,dword param_2,int param_3,int param_4,int param_5,
          int *param_6)

{
  byte bVar1;
  dword dVar2;
  int iVar3;
  dword dVar4;
  dword *pdVar5;
  byte *pbVar6;
  int iVar7;
  byte *pbVar8;
  int *piVar9;
  bool bVar10;
  int *in_stack_0000001c;
  
  if (param_5 < 1) {
    return 0;
  }
  if (((int)param_2 < 0) || (param_2 != this->mbr_0xbc)) {
    dVar4 = 0;
    if (-1 < (int)param_2) {
      dVar2 = 0;
      if (0 < (int)this->mbr_0x7c) {
        do {
          pdVar5 = *(dword **)(this->mbr_0x8c + dVar2 * 4);
          if (pdVar5 == (dword *)0x0) {
            pdVar5 = (dword *)this->mbr_0x90;
          }
          if (*pdVar5 == param_2) {
            this->mbr_0xc0 = dVar2;
            this->mbr_0xbc = param_2;
            dVar4 = dVar2;
            break;
          }
          dVar2 = dVar2 + 1;
        } while ((int)dVar2 < (int)this->mbr_0x7c);
      }
    }
  }
  else {
    dVar4 = this->mbr_0xc0;
  }
  iVar3 = 0;
  if ((int)dVar4 < (int)this->mbr_0x7c) {
    do {
      piVar9 = *(int **)(this->mbr_0x8c + dVar4 * 4);
      if (piVar9 == (int *)0x0) {
        piVar9 = (int *)this->mbr_0x90;
      }
      if ((int)param_2 < 0) {
LAB_0040c87a:
        if (-1 < param_3) {
          if (piVar9[1] < param_3) goto LAB_0040c8fb;
          if (param_3 < piVar9[1]) {
            return iVar3;
          }
        }
        pbVar6 = (byte *)piVar9[2];
        pbVar8 = param_1;
        do {
          bVar1 = *pbVar6;
          bVar10 = bVar1 < *pbVar8;
          if (bVar1 != *pbVar8) {
LAB_0040c8b6:
            iVar7 = (1 - (uint)bVar10) - (uint)(bVar10 != 0);
            goto LAB_0040c8bb;
          }
          if (bVar1 == 0) break;
          bVar1 = pbVar6[1];
          bVar10 = bVar1 < pbVar8[1];
          if (bVar1 != pbVar8[1]) goto LAB_0040c8b6;
          pbVar6 = pbVar6 + 2;
          pbVar8 = pbVar8 + 2;
        } while (bVar1 != 0);
        iVar7 = 0;
LAB_0040c8bb:
        if (iVar7 == 0) {
          if (iVar3 == 0) {
            if (param_6 != (int *)0x0) {
              *param_6 = *piVar9;
            }
            if (in_stack_0000001c != (int *)0x0) {
              *in_stack_0000001c = piVar9[1];
            }
          }
          *(int *)(param_4 + iVar3 * 4) = piVar9[3];
          iVar3 = iVar3 + 1;
          if (param_5 <= iVar3) {
            return iVar3;
          }
        }
      }
      else if ((int)param_2 <= *piVar9) {
        if ((int)param_2 < *piVar9) {
          return iVar3;
        }
        goto LAB_0040c87a;
      }
LAB_0040c8fb:
      dVar4 = dVar4 + 1;
    } while ((int)dVar4 < (int)this->mbr_0x7c);
  }
  return iVar3;
}



// Function at 0040ca90

void __thiscall
OOAnalyzer::cls_0x5a486c::meth_0x40ca90(cls_0x5a486c *this,cls_0x46ec50 *param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *in_stack_0000000c;
  
  iVar1 = in_stack_0000000c[2];
  if (((((0 < iVar1) && (iVar2 = *in_stack_0000000c, 0 < iVar2)) && (iVar2 < 0x280)) &&
      ((iVar3 = in_stack_0000000c[3], 0 < iVar3 && (iVar4 = in_stack_0000000c[1], 0 < iVar4)))) &&
     ((iVar4 < 0x1e0 &&
      ((iVar1 != iVar2 && -1 < iVar1 - iVar2 && (iVar3 != iVar4 && -1 < iVar3 - iVar4)))))) {
    cls_0x46ec50::meth_0x46ec50(param_1);
    (*this->vftptr_0x0->virt_meth_0x410280_100)(this);
    (*this->vftptr_0x0->virt_meth_0x4102b0_104)(this);
    (*this->vftptr_0x0->virt_meth_0x410530_156)(this);
    (*this->vftptr_0x0->virt_meth_0x410560_160)(this);
    (*this->vftptr_0x0->virt_meth_0x410310_112)(this);
    (*this->vftptr_0x0->virt_meth_0x410590_164)(this);
  }
  return;
}



// Function at 00446210

cls_0x5a486c * __thiscall OOAnalyzer::cls_0x5a486c::cls_0x5a486c(cls_0x5a486c *this,dword param_1)

{
  int iVar1;
  dword dVar2;
  int in_stack_00000008;
  
  iVar1 = DAT_006581e8;
  this->mbr_0x8 = param_1;
  this->vftptr_0x0 = &cls_0x5a486c__vftable_5a486c_005a486c;
  dVar2 = *(dword *)(iVar1 + param_1 * 4);
  if (dVar2 == 0) {
    dVar2 = DAT_006581ec;
  }
  iVar1 = *(int *)(dVar2 + 0x60);
  this->mbr_0x4 = dVar2;
  if ((iVar1 == 0) && (0 < in_stack_00000008)) {
    meth_0x447ac0(this);
  }
  return this;
}



// Function at 00447ac0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall OOAnalyzer::cls_0x5a486c::meth_0x447ac0(cls_0x5a486c *this)

{
  char cVar1;
  dword dVar2;
  undefined uVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  uint uVar7;
  uint uVar8;
  undefined4 *puVar9;
  char *pcVar10;
  char *pcVar11;
  char *pcVar12;
  undefined4 *puVar13;
  int in_stack_00000004;
  undefined4 auStack_104 [65];
  cls_0x4a21a0 *this_00;
  
  if (*(int *)(this->mbr_0x4 + 0x50) != 3) {
    ReleaseMutex(DAT_006581f8);
    FUN_00482130();
    dVar2 = this->mbr_0x4;
    if ((*(int *)(dVar2 + 0x50) == 0) || (*(int *)(dVar2 + 0x50) == 1)) {
      if (in_stack_00000004 == 0) {
        *(undefined4 *)(dVar2 + 0x50) = 1;
        DAT_00658240 = 0;
        PulseEvent(DAT_0065823c);
      }
      else {
        *(undefined4 *)(dVar2 + 0x50) = 2;
        FUN_00482140();
        if (*(int *)(this->mbr_0x4 + 0x78) < 0) {
LAB_00447b92:
          uVar7 = 0xffffffff;
          pcVar10 = &DAT_0065bc44;
          do {
            pcVar11 = pcVar10;
            if (uVar7 == 0) break;
            uVar7 = uVar7 - 1;
            pcVar11 = pcVar10 + 1;
            cVar1 = *pcVar10;
            pcVar10 = pcVar11;
          } while (cVar1 != '\0');
          uVar7 = ~uVar7;
          puVar9 = (undefined4 *)(pcVar11 + -uVar7);
          puVar13 = auStack_104;
          for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
            *puVar13 = *puVar9;
            puVar9 = puVar9 + 1;
            puVar13 = puVar13 + 1;
          }
          for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
            *(undefined *)puVar13 = *(undefined *)puVar9;
            puVar9 = (undefined4 *)((int)puVar9 + 1);
            puVar13 = (undefined4 *)((int)puVar13 + 1);
          }
        }
        else {
          iVar5 = *(int *)(this->mbr_0x4 + 0x78);
          iVar4 = cls_0x4609f0::meth_0x460c60((cls_0x4609f0 *)&UNK_0065a630.field_0x138);
          if (iVar4 <= iVar5) goto LAB_00447b92;
          iVar5 = cls_0x4609f0::meth_0x460ca0((cls_0x4609f0 *)&UNK_0065a630.field_0x138,iVar5);
          uVar7 = 0xffffffff;
          pcVar10 = (char *)(iVar5 + 0x58);
          do {
            pcVar11 = pcVar10;
            if (uVar7 == 0) break;
            uVar7 = uVar7 - 1;
            pcVar11 = pcVar10 + 1;
            cVar1 = *pcVar10;
            pcVar10 = pcVar11;
          } while (cVar1 != '\0');
          uVar7 = ~uVar7;
          puVar9 = (undefined4 *)(pcVar11 + -uVar7);
          puVar13 = auStack_104;
          for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
            *puVar13 = *puVar9;
            puVar9 = puVar9 + 1;
            puVar13 = puVar13 + 1;
          }
          for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
            *(undefined *)puVar13 = *(undefined *)puVar9;
            puVar9 = (undefined4 *)((int)puVar9 + 1);
            puVar13 = (undefined4 *)((int)puVar13 + 1);
          }
          iVar5 = -1;
          puVar9 = auStack_104;
          do {
            puVar13 = puVar9;
            if (iVar5 == 0) break;
            iVar5 = iVar5 + -1;
            puVar13 = (undefined4 *)((int)puVar9 + 1);
            cVar1 = *(char *)puVar9;
            puVar9 = puVar13;
          } while (cVar1 != '\0');
          *(undefined4 *)((int)puVar13 + -1) = s__Imagery__005cfe0c._0_4_;
          *(undefined4 *)((int)puVar13 + 3) = s__Imagery__005cfe0c._4_4_;
          *(undefined2 *)((int)puVar13 + 7) = s__Imagery__005cfe0c._8_2_;
        }
        uVar7 = 0xffffffff;
        pcVar10 = &DAT_00658244;
        do {
          pcVar11 = pcVar10;
          if (uVar7 == 0) break;
          uVar7 = uVar7 - 1;
          pcVar11 = pcVar10 + 1;
          cVar1 = *pcVar10;
          pcVar10 = pcVar11;
        } while (cVar1 != '\0');
        uVar7 = ~uVar7;
        iVar5 = -1;
        puVar9 = auStack_104;
        do {
          puVar13 = puVar9;
          if (iVar5 == 0) break;
          iVar5 = iVar5 + -1;
          puVar13 = (undefined4 *)((int)puVar9 + 1);
          cVar1 = *(char *)puVar9;
          puVar9 = puVar13;
        } while (cVar1 != '\0');
        puVar9 = (undefined4 *)(pcVar11 + -uVar7);
        puVar13 = (undefined4 *)((int)puVar13 + -1);
        for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
          *puVar13 = *puVar9;
          puVar9 = puVar9 + 1;
          puVar13 = puVar13 + 1;
        }
        pcVar10 = (char *)this->mbr_0x4;
        for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
          *(undefined *)puVar13 = *(undefined *)puVar9;
          puVar9 = (undefined4 *)((int)puVar9 + 1);
          puVar13 = (undefined4 *)((int)puVar13 + 1);
        }
        uVar7 = 0xffffffff;
        pcVar11 = pcVar10;
        do {
          pcVar12 = pcVar11;
          if (uVar7 == 0) break;
          uVar7 = uVar7 - 1;
          pcVar12 = pcVar11 + 1;
          cVar1 = *pcVar11;
          pcVar11 = pcVar12;
        } while (cVar1 != '\0');
        uVar7 = ~uVar7;
        iVar5 = -1;
        puVar9 = auStack_104;
        do {
          puVar13 = puVar9;
          if (iVar5 == 0) break;
          iVar5 = iVar5 + -1;
          puVar13 = (undefined4 *)((int)puVar9 + 1);
          cVar1 = *(char *)puVar9;
          puVar9 = puVar13;
        } while (cVar1 != '\0');
        puVar9 = (undefined4 *)(pcVar12 + -uVar7);
        puVar13 = (undefined4 *)((int)puVar13 + -1);
        for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
          *puVar13 = *puVar9;
          puVar9 = puVar9 + 1;
          puVar13 = puVar13 + 1;
        }
        for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
          *(undefined *)puVar13 = *(undefined *)puVar9;
          puVar9 = (undefined4 *)((int)puVar9 + 1);
          puVar13 = (undefined4 *)((int)puVar13 + 1);
        }
        uVar6 = FUN_0047f670(auStack_104,0xffffffff,pcVar10 + 0x68);
        *(undefined4 *)(this->mbr_0x4 + 0x60) = uVar6;
        FUN_00482130();
        dVar2 = this->mbr_0x4;
        if (*(int *)(dVar2 + 0x60) == 0) {
          *(undefined4 *)(dVar2 + 0x50) = 4;
        }
        else {
          DAT_006682cc = DAT_006682cc + *(int *)(dVar2 + 0x68);
          *(undefined4 *)(dVar2 + 0x50) = 3;
        }
      }
    }
    FUN_00482140();
    if ((*(int *)(this->mbr_0x4 + 0x50) == 2) && (in_stack_00000004 != 0)) {
      do {
        WaitForSingleObject(DAT_006581f0,0xffffffff);
      } while (*(int *)(this->mbr_0x4 + 0x50) == 2);
    }
    if (*(int *)(this->mbr_0x4 + 0x50) == 4) {
      FUN_00481c10(s_Imagery_body_load_failed__005cfe18,0);
    }
  }
  if ((*(int *)(this->mbr_0x4 + 0x60) != 0) && (*(int *)(this->mbr_0x4 + 0x74) == 0)) {
    uVar3 = (*this->vftptr_0x0->virt_meth_0x448540_212)(this);
    this_00 = (cls_0x4a21a0 *)CONCAT31(extraout_var,uVar3);
    if (this_00 == (cls_0x4a21a0 *)0x0) {
      uVar3 = (*this->vftptr_0x0->virt_meth_0x448550_216)(this);
      iVar5 = CONCAT31(extraout_var_00,uVar3);
      if (((iVar5 == 0) || (*(int *)(iVar5 + 0x38) == 0)) ||
         (this_00 = (cls_0x4a21a0 *)(*(int *)(iVar5 + 0x38) + 0x38 + iVar5),
         this_00 == (cls_0x4a21a0 *)0x0)) goto LAB_00447d5f;
    }
    if (((this_00->field_0x10 & 6) != 0) && (*(int *)&this_00->field_0x18 == 0)) {
      *(undefined4 *)&this_00->field_0x18 = _DAT_006668d0;
    }
    uVar6 = cls_0x4a21a0::meth_0x4a21a0(this_00);
    uVar6 = FUN_00482fb0(uVar6);
    *(undefined4 *)(this->mbr_0x4 + 0x74) = uVar6;
    uVar7 = cls_0x4a21a0::meth_0x4a21a0(this_00);
    puVar9 = *(undefined4 **)(this->mbr_0x4 + 0x74);
    for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
      *puVar9 = *(undefined4 *)this_00;
      this_00 = (cls_0x4a21a0 *)&this_00->field_0x4;
      puVar9 = puVar9 + 1;
    }
    for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined *)puVar9 = *(undefined *)this_00;
      this_00 = (cls_0x4a21a0 *)&this_00->field_0x1;
      puVar9 = (undefined4 *)((int)puVar9 + 1);
    }
  }
LAB_00447d5f:
  return *(undefined4 *)(this->mbr_0x4 + 0x60);
}



// Function at 00447d80

void __thiscall OOAnalyzer::cls_0x5a486c::meth_0x447d80(cls_0x5a486c *this)

{
  int iVar1;
  dword dVar2;
  
  FUN_00482130();
  if (*(int *)(this->mbr_0x4 + 0x50) == 2) {
    FUN_00482140();
    iVar1 = *(int *)(this->mbr_0x4 + 0x50);
    while (iVar1 == 2) {
      WaitForSingleObject(DAT_006581f0,0xffffffff);
      iVar1 = *(int *)(this->mbr_0x4 + 0x50);
    }
    FUN_00482130();
  }
  dVar2 = this->mbr_0x4;
  iVar1 = *(int *)(dVar2 + 0x50);
  if (((iVar1 == 1) || (iVar1 == 4)) || (iVar1 == 0)) {
    *(undefined4 *)(dVar2 + 0x50) = 0;
    *(undefined4 *)(dVar2 + 0x60) = 0;
    FUN_00482140();
  }
  else if (iVar1 == 3) {
    *(undefined4 *)(dVar2 + 0x50) = 0;
    FUN_00482f80(*(undefined4 *)(dVar2 + 0x60));
    *(undefined4 *)(this->mbr_0x4 + 0x60) = 0;
    FUN_00482140();
    return;
  }
  return;
}



// Function at 004483c0

cls_0x5a486c * __thiscall OOAnalyzer::cls_0x5a486c::~cls_0x5a486c(cls_0x5a486c *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5a486c__vftable_5a486c_005a486c;
  meth_0x447d80(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



