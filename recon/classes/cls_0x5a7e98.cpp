// REVSYNC: candidate=TCharAnimator confidence=MEDIUM evidence=src-path(CharAnimator.cpp x1) src-file=src/charanimator.cpp date=2026-04-17 note="only 1 CharAnimator.cpp assertion ref — low-strength fingerprint; downgraded from prior HIGH, not personally verified against src/charanimator this session."
// Decompiled methods and structure for class: cls_0x5a7e98

/*
/OOAnalyzer/cls_0x5a7e98
pack(disabled)
Structure cls_0x5a7e98 {
   0   cls_0x5a7e98::vftable_5a7e98 *   4   vftptr_0x0   "pointer to cls_0x5a7e98::vftable_5a7e98"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   160   dword   4   mbr_0xa0   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
   168   dword   4   mbr_0xa8   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   176   dword   4   mbr_0xb0   "Unsigned Double-Word (ddw, 4-bytes)"
   180   dword   4   mbr_0xb4   "Unsigned Double-Word (ddw, 4-bytes)"
   312   dword   4   mbr_0x138   "Unsigned Double-Word (ddw, 4-bytes)"
   848   dword   4   mbr_0x350   "Unsigned Double-Word (ddw, 4-bytes)"
   852   dword   4   mbr_0x354   "Unsigned Double-Word (ddw, 4-bytes)"
   856   dword   4   mbr_0x358   "Unsigned Double-Word (ddw, 4-bytes)"
   860   dword   4   mbr_0x35c   "Unsigned Double-Word (ddw, 4-bytes)"
   864   dword   4   mbr_0x360   "Unsigned Double-Word (ddw, 4-bytes)"
   868   dword   4   mbr_0x364   "Unsigned Double-Word (ddw, 4-bytes)"
   872   dword   4   mbr_0x368   "Unsigned Double-Word (ddw, 4-bytes)"
   876   dword   4   mbr_0x36c   "Unsigned Double-Word (ddw, 4-bytes)"
   880   dword   4   mbr_0x370   "Unsigned Double-Word (ddw, 4-bytes)"
   884   dword   4   mbr_0x374   "Unsigned Double-Word (ddw, 4-bytes)"
   888   dword   4   mbr_0x378   "Unsigned Double-Word (ddw, 4-bytes)"
   892   dword   4   mbr_0x37c   "Unsigned Double-Word (ddw, 4-bytes)"
   896   dword   4   mbr_0x380   "Unsigned Double-Word (ddw, 4-bytes)"
   900   dword   4   mbr_0x384   "Unsigned Double-Word (ddw, 4-bytes)"
   904   dword   4   mbr_0x388   "Unsigned Double-Word (ddw, 4-bytes)"
   908   dword   4   mbr_0x38c   "Unsigned Double-Word (ddw, 4-bytes)"
   912   dword   4   mbr_0x390   "Unsigned Double-Word (ddw, 4-bytes)"
   916   dword   4   mbr_0x394   "Unsigned Double-Word (ddw, 4-bytes)"
   920   dword   4   mbr_0x398   "Unsigned Double-Word (ddw, 4-bytes)"
   924   dword   4   mbr_0x39c   "Unsigned Double-Word (ddw, 4-bytes)"
   928   dword   4   mbr_0x3a0   "Unsigned Double-Word (ddw, 4-bytes)"
   932   dword   4   mbr_0x3a4   "Unsigned Double-Word (ddw, 4-bytes)"
   936   dword   4   mbr_0x3a8   "Unsigned Double-Word (ddw, 4-bytes)"
   940   dword   4   mbr_0x3ac   "Unsigned Double-Word (ddw, 4-bytes)"
   944   dword   4   mbr_0x3b0   "Unsigned Double-Word (ddw, 4-bytes)"
   948   dword   4   mbr_0x3b4   "Unsigned Double-Word (ddw, 4-bytes)"
   952   dword   4   mbr_0x3b8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 956 Alignment: 1

*/

// Function at 004d90b0

void __thiscall OOAnalyzer::cls_0x5a7e98::meth_0x4d90b0(cls_0x5a7e98 *this)

{
  dword dVar1;
  short sVar2;
  dword dVar3;
  undefined4 uVar4;
  short *psVar5;
  int iVar6;
  int iVar7;
  undefined4 *puVar8;
  dword *in_stack_00000004;
  
  if (this->mbr_0x380 == 0) {
    this->mbr_0x380 = 1;
    dVar3 = in_stack_00000004[7];
    this->mbr_0x3a4 = in_stack_00000004[8];
    dVar1 = in_stack_00000004[6];
    this->mbr_0x3a0 = dVar3;
    dVar3 = in_stack_00000004[9];
    this->mbr_0x39c = dVar1;
    dVar1 = in_stack_00000004[0xb];
    this->mbr_0x3a8 = dVar3;
    dVar3 = in_stack_00000004[10];
    this->mbr_0x3b0 = dVar1;
    dVar1 = in_stack_00000004[0xd];
    this->mbr_0x3ac = dVar3;
    dVar3 = in_stack_00000004[0xc];
    this->mbr_0x3b8 = dVar1;
    this->mbr_0x3b4 = dVar3;
    this->mbr_0x384 = *in_stack_00000004;
    this->mbr_0x388 = in_stack_00000004[1];
    this->mbr_0x38c = in_stack_00000004[2];
    this->mbr_0x390 = in_stack_00000004[3];
    dVar3 = in_stack_00000004[5];
    this->mbr_0x394 = in_stack_00000004[4];
    this->mbr_0x398 = dVar3;
    this->mbr_0x350 = (dword)&this->mbr_0x4;
    if ((&this->mbr_0x4 != (dword *)0x0) && (this->mbr_0xa8 != 0)) {
      FUN_004830f0(this->mbr_0xa8);
    }
    if ((this->mbr_0x350 != 0) && (iVar6 = *(int *)(this->mbr_0x350 + 0xac), iVar6 != 0)) {
      FUN_004830f0(iVar6);
    }
    puVar8 = (undefined4 *)this->mbr_0x350;
    for (iVar6 = 0xd3; iVar6 != 0; iVar6 = iVar6 + -1) {
      *puVar8 = 0;
      puVar8 = puVar8 + 1;
    }
    dVar3 = (int)this->mbr_0x3ac / (int)this->mbr_0x3b0 + 3;
    this->mbr_0x354 = dVar3;
    dVar3 = FUN_00482fb0(dVar3 * 0xc);
    this->mbr_0x35c = dVar3;
    dVar3 = FUN_00482fb0(this->mbr_0x354 * 0xc);
    puVar8 = (undefined4 *)this->mbr_0x35c;
    this->mbr_0x360 = dVar3;
    *puVar8 = 0;
    puVar8[1] = 0;
    puVar8 = (undefined4 *)this->mbr_0x360;
    *puVar8 = 0;
    puVar8[1] = 0;
    iVar6 = (this->mbr_0x354 - 1) * this->mbr_0x3b0;
    dVar3 = iVar6 * 2;
    this->mbr_0x358 = dVar3;
    *(dword *)(this->mbr_0x350 + 0xa0) = dVar3;
    uVar4 = FUN_00482fb0(iVar6 * 0x40);
    dVar3 = this->mbr_0x350;
    *(undefined4 *)(dVar3 + 0xa4) = uVar4;
    dVar1 = this->mbr_0x3ac;
    *(undefined4 *)(dVar3 + 0x9c) = 0x1e2;
    *(dword *)(dVar3 + 0xa8) = dVar1 * 2;
    psVar5 = (short *)FUN_00482fb0(dVar1 * 0xc);
    dVar3 = this->mbr_0x350;
    iVar7 = 0;
    *(short **)(dVar3 + 0xac) = psVar5;
    iVar6 = *(int *)(dVar3 + 0xa8);
    if (0 < iVar6) {
      do {
        sVar2 = (short)iVar7;
        *psVar5 = sVar2;
        psVar5[1] = sVar2 + 1;
        psVar5[2] = sVar2 + 2;
        iVar7 = iVar7 + 1;
        psVar5 = psVar5 + 3;
      } while (iVar7 < iVar6);
    }
    dVar1 = this->mbr_0x3ac;
    *(undefined4 *)(dVar3 + 0xb0) = 0;
    iVar6 = 0;
    *(undefined4 *)(dVar3 + 0x134) = *(undefined4 *)(dVar3 + 0xa8);
    *(undefined4 *)(dVar3 + 0x98) = 4;
    if (dVar1 != 0xffffffff && -1 < (int)(dVar1 + 1)) {
      do {
        meth_0x4d96c0(this);
        iVar6 = iVar6 + 1;
      } while (iVar6 < (int)(this->mbr_0x3ac + 1));
    }
    meth_0x4d9820(this);
  }
  return;
}



// Function at 004d92d0

void __thiscall OOAnalyzer::cls_0x5a7e98::meth_0x4d92d0(cls_0x5a7e98 *this)

{
  dword dVar1;
  
  if (this->mbr_0x380 != 0) {
    dVar1 = this->mbr_0x350;
    this->mbr_0x380 = 0;
    if (dVar1 != 0) {
      if (*(int *)(dVar1 + 0xac) != 0) {
        FUN_004830f0(*(int *)(dVar1 + 0xac));
        dVar1 = this->mbr_0x350;
        *(undefined4 *)(dVar1 + 0xac) = 0;
      }
      if (*(int *)(dVar1 + 0xa4) != 0) {
        FUN_004830f0(*(int *)(dVar1 + 0xa4));
        *(undefined4 *)(this->mbr_0x350 + 0xa4) = 0;
      }
    }
    if (this->mbr_0x35c != 0) {
      FUN_004830f0(this->mbr_0x35c);
    }
    if (this->mbr_0x360 != 0) {
      FUN_004830f0(this->mbr_0x360);
    }
    this->mbr_0x360 = 0;
    this->mbr_0x35c = 0;
    if (this->mbr_0x3b4 != 0) {
      FUN_00482f80(this->mbr_0x3b4);
      this->mbr_0x3b4 = 0;
    }
  }
  return;
}



// Function at 004d9380

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x5a7e98::meth_0x4d9380(cls_0x5a7e98 *this)

{
  float fVar1;
  float fVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  dword dVar7;
  int iVar8;
  int iVar9;
  undefined4 *puVar10;
  float local_34;
  uint local_30;
  int local_2c;
  dword *local_28;
  float local_24;
  int local_1c;
  int local_18;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  if (((this->mbr_0x380 != 0) &&
      (puVar10 = (undefined4 *)this->mbr_0x350, puVar10 != (undefined4 *)0x0)) &&
     (puVar10[0x28] != 0)) {
    *puVar10 = 0x5a110;
    FUN_0043a9f0(puVar10 + 0x16);
    dVar7 = this->mbr_0x3b0;
    local_34 = 0.7;
    local_2c = 0;
    fVar1 = _DAT_005a34e4 / (float)dVar7;
    if (1 < (int)(this->mbr_0x354 - 2)) {
      local_30 = 0;
      iVar8 = 0x18;
      do {
        local_24 = 0.0;
        local_18 = 0;
        if (0 < (int)dVar7) {
          do {
            iVar3 = __ftol();
            local_28 = &this->mbr_0x35c;
            iVar9 = local_2c << 5;
            local_2c = local_2c + 2;
            local_1c = 2;
            do {
              dVar7 = *local_28;
              FUN_0043b280(&local_c,local_24,dVar7 + (((int)local_30 < 0) - 1 & local_30) * 0xc,
                           (dVar7 - 0xc) + iVar8,iVar8 + dVar7,dVar7 + 0xc + iVar8);
              puVar10 = (undefined4 *)(iVar9 + *(int *)(this->mbr_0x350 + 0xa4));
              *puVar10 = local_c;
              puVar10[1] = local_8;
              puVar10[2] = local_4;
              uVar4 = __ftol();
              uVar5 = __ftol();
              uVar6 = __ftol();
              puVar10[4] = ((uVar4 | iVar3 << 8) << 8 | uVar5) << 8 | uVar6;
              local_28 = local_28 + 1;
              iVar9 = iVar9 + 0x20;
              local_1c = local_1c + -1;
            } while (local_1c != 0);
            dVar7 = this->mbr_0x3b0;
            local_24 = fVar1 + local_24;
            fVar2 = _DAT_005a7ec0 / (float)((this->mbr_0x354 - 2) * dVar7);
            if (local_34 < fVar2) {
              local_34 = 0.0;
            }
            else {
              local_34 = local_34 - fVar2;
            }
            local_18 = local_18 + 1;
          } while (local_18 < (int)dVar7);
        }
        iVar8 = iVar8 + 0xc;
        iVar3 = local_30 + 2;
        local_30 = local_30 + 1;
      } while (iVar3 < (int)(this->mbr_0x354 - 2));
    }
  }
  return;
}



// Function at 004d95b0

void __thiscall OOAnalyzer::cls_0x5a7e98::meth_0x4d95b0(cls_0x5a7e98 *this)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  if (((this->mbr_0x380 != 0) && (this->mbr_0x350 != 0)) && (*(int *)(this->mbr_0x350 + 0xa0) != 0))
  {
    piVar1 = *(int **)(this->mbr_0x39c + 4);
    if (((piVar1 != (int *)0x0) && ((int *)piVar1[0x36] != (int *)0x0)) &&
       ((*(int *)piVar1[0x36] == 7 && ((*(short *)(piVar1 + 1) != 0xb || (piVar1[0xac] != 0)))))) {
      iVar3 = (int)*(short *)(piVar1 + 0x17);
      iVar2 = (**(code **)(*piVar1 + 0x1fc))();
      iVar2 = *(int *)(iVar2 + 0x48);
      if ((iVar2 != 0) && ((*(int *)(iVar2 + 0xec) != 0 || (*(int *)(iVar2 + 0xf0) != 0)))) {
        if (iVar3 == *(int *)(iVar2 + 0xec)) {
          meth_0x4d9c60(this);
        }
        if ((*(int *)(iVar2 + 0xec) <= iVar3) && (iVar3 <= *(int *)(iVar2 + 0xf0))) {
          meth_0x4d96c0(this);
          return;
        }
      }
      meth_0x4d9ae0(this);
      meth_0x4d9b60(this);
      return;
    }
    iVar2 = 0;
    if (0 < (int)(this->mbr_0x3ac + ((int)this->mbr_0x3ac >> 0x1f & 3U)) >> 2) {
      do {
        meth_0x4d9b60(this);
        iVar2 = iVar2 + 1;
      } while (iVar2 < (int)(this->mbr_0x3ac + ((int)this->mbr_0x3ac >> 0x1f & 3U)) >> 2);
    }
  }
  return;
}



// Function at 004d96c0

void __thiscall OOAnalyzer::cls_0x5a7e98::meth_0x4d96c0(cls_0x5a7e98 *this)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  dword dVar4;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  if (this->mbr_0x380 == 0) {
    return;
  }
  if (this->mbr_0x350 == 0) {
    return;
  }
  if (*(int *)(this->mbr_0x350 + 0xa0) == 0) {
    return;
  }
  iVar3 = *(int *)(this->mbr_0x39c + 4);
  if ((*(short *)(iVar3 + 4) == 0xb) && (iVar3 = *(int *)(iVar3 + 0x2b0), iVar3 != 0)) {
    if (this->mbr_0x3b4 == 0) {
      meth_0x4d92d0(this);
      return;
    }
    iVar3 = *(int *)(iVar3 + 0x38);
    if ((iVar3 != 0) && (iVar3 = FUN_0059a530(this->mbr_0x3b4,iVar3), iVar3 != 0)) {
      meth_0x4d92d0(this);
      return;
    }
  }
  meth_0x4d9ae0(this);
  meth_0x4d9b60(this);
  if ((this->mbr_0x380 == 0) ||
     (((iVar3 = cls_0x5a7e38::meth_0x40edc0((cls_0x5a7e38 *)this->mbr_0x39c), iVar3 < 0 &&
       (iVar3 = cls_0x5a7e38::meth_0x40edc0((cls_0x5a7e38 *)this->mbr_0x39c), iVar3 < 0)) &&
      (iVar3 = cls_0x5a7e38::meth_0x40edc0((cls_0x5a7e38 *)this->mbr_0x39c), iVar3 < 0)))) {
    dVar4 = 0;
  }
  else {
    iVar3 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this->mbr_0x39c);
    dVar4 = iVar3 + 0x58;
  }
  this->mbr_0x37c = dVar4;
  FUN_0043ad80(dVar4,&this->mbr_0x364,&local_18);
  FUN_0043ad80(this->mbr_0x37c,&this->mbr_0x370,&local_c);
  puVar1 = (undefined4 *)this->mbr_0x35c;
  puVar2 = (undefined4 *)this->mbr_0x360;
  *puVar1 = local_18;
  puVar1[1] = local_14;
  puVar1[2] = local_10;
  *puVar2 = local_c;
  puVar2[1] = local_8;
  puVar2[2] = local_4;
  return;
}



// Function at 004d9820

void __thiscall OOAnalyzer::cls_0x5a7e98::meth_0x4d9820(cls_0x5a7e98 *this)

{
  float fVar1;
  float fVar2;
  float fVar3;
  
  if ((this->mbr_0x380 != 0) && (this->mbr_0x350 != 0)) {
    fVar1 = (float)this->mbr_0x390 - (float)this->mbr_0x384;
    fVar2 = (float)this->mbr_0x394 - (float)this->mbr_0x388;
    fVar3 = (float)this->mbr_0x398 - (float)this->mbr_0x38c;
    if ((fVar2 < fVar1) && (fVar3 < fVar1)) {
      if (this->mbr_0x3b8 == 0) {
        this->mbr_0x364 = 0;
      }
      else {
        this->mbr_0x364 = this->mbr_0x384;
      }
      this->mbr_0x368 = 0;
      this->mbr_0x36c = 0;
      this->mbr_0x370 = this->mbr_0x390;
      this->mbr_0x374 = 0;
      this->mbr_0x378 = 0;
      return;
    }
    if ((fVar1 < fVar2) && (fVar3 < fVar2)) {
      this->mbr_0x364 = 0;
      if (this->mbr_0x3b8 == 0) {
        this->mbr_0x368 = 0;
      }
      else {
        this->mbr_0x368 = this->mbr_0x388;
      }
      this->mbr_0x36c = 0;
      this->mbr_0x370 = 0;
      this->mbr_0x374 = this->mbr_0x388;
      this->mbr_0x378 = 0;
      return;
    }
    this->mbr_0x364 = 0;
    this->mbr_0x368 = 0;
    if (this->mbr_0x3b8 == 0) {
      this->mbr_0x36c = 0;
    }
    else {
      this->mbr_0x36c = this->mbr_0x38c;
    }
    this->mbr_0x370 = 0;
    this->mbr_0x374 = 0;
    this->mbr_0x378 = this->mbr_0x398;
  }
  return;
}



// Function at 004d9ae0

void __thiscall OOAnalyzer::cls_0x5a7e98::meth_0x4d9ae0(cls_0x5a7e98 *this)

{
  dword dVar1;
  dword dVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  int iVar5;
  
  if ((((this->mbr_0x380 != 0) && (this->mbr_0x350 != 0)) && (*(int *)(this->mbr_0x350 + 0xa0) != 0)
      ) && (iVar5 = this->mbr_0x354 - 1, 0 < iVar5)) {
    dVar1 = this->mbr_0x35c;
    dVar2 = this->mbr_0x360;
    puVar3 = (undefined4 *)(dVar2 + iVar5 * 0xc);
    puVar4 = (undefined4 *)((dVar1 - 8) + iVar5 * 0xc);
    do {
      *(undefined4 *)((dVar1 - dVar2) + (int)puVar3) = puVar4[-1];
      puVar4[3] = *puVar4;
      puVar4[4] = puVar4[1];
      *puVar3 = puVar3[-3];
      puVar3[1] = puVar3[-2];
      puVar3[2] = puVar3[-1];
      puVar3 = puVar3 + -3;
      iVar5 = iVar5 + -1;
      puVar4 = puVar4 + -3;
    } while (iVar5 != 0);
  }
  return;
}



// Function at 004d9b60

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x5a7e98::meth_0x4d9b60(cls_0x5a7e98 *this)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float *pfVar5;
  float *pfVar6;
  float *pfVar7;
  int iVar8;
  dword dVar9;
  
  if (((this->mbr_0x380 != 0) && (this->mbr_0x350 != 0)) && (*(int *)(this->mbr_0x350 + 0xa0) != 0))
  {
    dVar9 = this->mbr_0x354;
    if (0 < (int)dVar9) {
      fVar1 = _DAT_005a34e4 - _DAT_005a34e4 / (float)dVar9;
      iVar8 = this->mbr_0x35c - (int)(float *)this->mbr_0x360;
      pfVar5 = (float *)this->mbr_0x360;
      pfVar7 = (float *)(this->mbr_0x35c + 8);
      do {
        pfVar6 = pfVar5 + 3;
        dVar9 = dVar9 - 1;
        fVar2 = (*pfVar5 + *(float *)(iVar8 + (int)pfVar5)) * _DAT_005a356c;
        fVar3 = (pfVar5[1] + pfVar7[-1]) * _DAT_005a356c;
        fVar4 = (*pfVar7 + pfVar5[2]) * _DAT_005a356c;
        *(float *)(iVar8 + -0xc + (int)pfVar6) =
             (*(float *)(iVar8 + -0xc + (int)pfVar6) - fVar2) * fVar1 + fVar2;
        pfVar7[-1] = (pfVar7[-1] - fVar3) * fVar1 + fVar3;
        *pfVar7 = (*pfVar7 - fVar4) * fVar1 + fVar4;
        *pfVar5 = (*pfVar5 - fVar2) * fVar1 + fVar2;
        pfVar5[1] = (pfVar5[1] - fVar3) * fVar1 + fVar3;
        pfVar5[2] = (pfVar5[2] - fVar4) * fVar1 + fVar4;
        pfVar5 = pfVar6;
        pfVar7 = pfVar7 + 3;
      } while (dVar9 != 0);
    }
  }
  return;
}



// Function at 004d9c60

void __thiscall OOAnalyzer::cls_0x5a7e98::meth_0x4d9c60(cls_0x5a7e98 *this)

{
  dword dVar1;
  undefined4 *puVar2;
  int iVar3;
  dword dVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  if (this->mbr_0x380 != 0) {
    iVar3 = cls_0x5a7e38::meth_0x40edc0((cls_0x5a7e38 *)this->mbr_0x39c);
    if (((iVar3 < 0) &&
        (iVar3 = cls_0x5a7e38::meth_0x40edc0((cls_0x5a7e38 *)this->mbr_0x39c), iVar3 < 0)) &&
       (iVar3 = cls_0x5a7e38::meth_0x40edc0((cls_0x5a7e38 *)this->mbr_0x39c), iVar3 < 0)) {
      dVar4 = 0;
    }
    else {
      iVar3 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this->mbr_0x39c);
      dVar4 = iVar3 + 0x58;
    }
    this->mbr_0x37c = dVar4;
    FUN_0043ad80(dVar4,&this->mbr_0x364,&local_18);
    FUN_0043ad80(this->mbr_0x37c,&this->mbr_0x370,&local_c);
    dVar4 = this->mbr_0x354;
    if (0 < (int)dVar4) {
      dVar1 = this->mbr_0x35c;
      puVar2 = (undefined4 *)this->mbr_0x360;
      puVar5 = puVar2;
      puVar6 = (undefined4 *)(dVar1 + 8);
      do {
        *(undefined4 *)((dVar1 - (int)puVar2) + (int)puVar5) = local_18;
        puVar6[-1] = local_14;
        *puVar6 = local_10;
        *puVar5 = local_c;
        puVar5[1] = local_8;
        puVar5[2] = local_4;
        dVar4 = dVar4 - 1;
        puVar5 = puVar5 + 3;
        puVar6 = puVar6 + 3;
      } while (dVar4 != 0);
    }
    return;
  }
  return;
}



// Function at 004d9d50

void __thiscall OOAnalyzer::cls_0x5a7e98::meth_0x4d9d50(cls_0x5a7e98 *this)

{
  int iVar1;
  
  if (((this->mbr_0x380 != 0) && (this->mbr_0x350 != 0)) && (*(int *)(this->mbr_0x350 + 0xa0) != 0))
  {
    FUN_004de1f0();
    iVar1 = cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),2);
    if (iVar1 != 0) {
      FUN_004a90d0(iVar1,s_d__revenant_CharAnimator_cpp_005e0720,0x67e);
    }
    meth_0x4d9380(this);
    cls_0x5a486c::meth_0x40a8f0(*(cls_0x5a486c **)(this->mbr_0x39c + 8));
    FUN_004de200();
  }
  return;
}



// Function at 004da930

cls_0x5a7e98 * __thiscall OOAnalyzer::cls_0x5a7e98::~cls_0x5a7e98(cls_0x5a7e98 *this)

{
  cls_0x5a7e98 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059ec1b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a7e98__vftable_5a7e98_005a7e98;
  local_4 = 0;
  meth_0x4d92d0(this);
  pcVar1 = (cls_0x5a7e98 *)this->mbr_0xa8;
  local_4 = 0xffffffff;
  if (pcVar1 != (cls_0x5a7e98 *)0x0) {
    pcVar1 = (cls_0x5a7e98 *)FUN_004830f0(pcVar1);
  }
  if (this->mbr_0xb0 != 0) {
    pcVar1 = (cls_0x5a7e98 *)FUN_004830f0(this->mbr_0xb0);
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 004da9a0

cls_0x5a7e98 * __thiscall OOAnalyzer::cls_0x5a7e98::virt_meth_0x4da9a0(cls_0x5a7e98 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a7e98(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



