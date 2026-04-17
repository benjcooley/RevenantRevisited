// Decompiled methods and structure for class: cls_0x5b4a7c

/*
/OOAnalyzer/cls_0x5b4a7c
pack(disabled)
Structure cls_0x5b4a7c {
   0   cls_0x5b4a7c::vftable_5b4a7c *   4   vftptr_0x0   "pointer to cls_0x5b4a7c::vftable_5b4a7c"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   508   dword   4   mbr_0x1fc   "Unsigned Double-Word (ddw, 4-bytes)"
   540   dword   4   mbr_0x21c   "Unsigned Double-Word (ddw, 4-bytes)"
   544   dword   4   mbr_0x220   "Unsigned Double-Word (ddw, 4-bytes)"
   552   dword   4   mbr_0x228   "Unsigned Double-Word (ddw, 4-bytes)"
   556   dword   4   mbr_0x22c   "Unsigned Double-Word (ddw, 4-bytes)"
   576   dword   4   mbr_0x240   "Unsigned Double-Word (ddw, 4-bytes)"
   580   dword   4   mbr_0x244   "Unsigned Double-Word (ddw, 4-bytes)"
   584   dword   4   mbr_0x248   "Unsigned Double-Word (ddw, 4-bytes)"
   728   dword   4   mbr_0x2d8   "Unsigned Double-Word (ddw, 4-bytes)"
   836   dword   4   mbr_0x344   "Unsigned Double-Word (ddw, 4-bytes)"
   852   dword   4   mbr_0x354   "Unsigned Double-Word (ddw, 4-bytes)"
   864   dword   4   mbr_0x360   "Unsigned Double-Word (ddw, 4-bytes)"
   868   dword   4   mbr_0x364   "Unsigned Double-Word (ddw, 4-bytes)"
   872   dword   4   mbr_0x368   "Unsigned Double-Word (ddw, 4-bytes)"
   876   dword   4   mbr_0x36c   "Unsigned Double-Word (ddw, 4-bytes)"
   884   dword   4   mbr_0x374   "Unsigned Double-Word (ddw, 4-bytes)"
   888   dword   4   mbr_0x378   "Unsigned Double-Word (ddw, 4-bytes)"
   892   dword   4   mbr_0x37c   "Unsigned Double-Word (ddw, 4-bytes)"
   896   dword   4   mbr_0x380   "Unsigned Double-Word (ddw, 4-bytes)"
   1188   dword   4   mbr_0x4a4   "Unsigned Double-Word (ddw, 4-bytes)"
   1192   dword   4   mbr_0x4a8   "Unsigned Double-Word (ddw, 4-bytes)"
   1196   dword   4   mbr_0x4ac   "Unsigned Double-Word (ddw, 4-bytes)"
   1200   dword   4   mbr_0x4b0   "Unsigned Double-Word (ddw, 4-bytes)"
   1204   dword   4   mbr_0x4b4   "Unsigned Double-Word (ddw, 4-bytes)"
   1208   dword   4   mbr_0x4b8   "Unsigned Double-Word (ddw, 4-bytes)"
   1212   dword   4   mbr_0x4bc   "Unsigned Double-Word (ddw, 4-bytes)"
   1216   dword   4   mbr_0x4c0   "Unsigned Double-Word (ddw, 4-bytes)"
   1220   dword   4   mbr_0x4c4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 1224 Alignment: 1

*/

// Function at 00513bb0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x5b4a7c::meth_0x513bb0(cls_0x5b4a7c *this)

{
  float *pfVar1;
  float fVar2;
  float fVar3;
  cls_0x5a486c *this_00;
  float fVar4;
  undefined4 *puVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  dword *pdVar9;
  longlong lVar10;
  int local_c;
  
  puVar5 = (undefined4 *)cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  pdVar9 = &this->mbr_0x37c;
  local_c = 10;
  do {
    if ((float)pdVar9[1] != 0.0) {
      FUN_0040c960();
      uVar6 = this->mbr_0x244;
      this->mbr_0xec = 0;
      uVar8 = uVar6 & 0x80000003;
      if ((int)uVar8 < 0) {
        uVar8 = (uVar8 - 1 | 0xfffffffc) + 1;
      }
      fVar2 = (float)uVar8 * _DAT_005a3568;
      iVar7 = puVar5[0x29];
      puVar5[5] = pdVar9[-5];
      fVar4 = (float)((int)(uVar6 + ((int)uVar6 >> 0x1f & 3U)) >> 2) * _DAT_005a3568;
      *puVar5 = 0x209c;
      puVar5[0xc] = 0xbf860a92;
      puVar5[10] = 0xbf490fdb;
      puVar5[0xb] = 0;
      *(float *)(iVar7 + 0x18) = fVar2;
      *(float *)(iVar7 + 0x1c) = fVar4;
      fVar3 = fVar2 + _DAT_005a3568;
      *(float *)(iVar7 + 0x58) = fVar3;
      *(float *)(iVar7 + 0x5c) = fVar4;
      *(float *)(iVar7 + 0x38) = fVar2;
      fVar4 = fVar4 + _DAT_005a3568;
      *(float *)(iVar7 + 0x3c) = fVar4;
      *(float *)(iVar7 + 0x78) = fVar3;
      fVar2 = (float)pdVar9[-4];
      *(float *)(iVar7 + 0x7c) = fVar4;
      fVar3 = (float)pdVar9[-6];
      puVar5[6] = fVar2;
      fVar2 = (float)*pdVar9 * (float)pdVar9[-3];
      puVar5[0x12] = fVar2;
      puVar5[0x11] = fVar2;
      puVar5[4] = fVar3;
      this_00 = (cls_0x5a486c *)this->mbr_0x8;
      puVar5[0x10] = fVar2;
      cls_0x5a486c::meth_0x40a8f0(this_00);
      FUN_0040c9c0(&this->mbr_0x9c);
      if ((this->mbr_0x3c & 1) != 0) {
        FUN_0040ca20((int *)&this->mbr_0x9c);
      }
      this->mbr_0xec = 1;
      if ((this->mbr_0x3c & 2) != 0) {
        cls_0x5a486c::meth_0x40ca90((cls_0x5a486c *)this->mbr_0x8,this->mbr_0x4,this->mbr_0xc);
      }
      this->mbr_0xec = 1;
      FUN_0040c960();
      this->mbr_0xec = 0;
      lVar10 = __ftol();
      uVar6 = (uint)lVar10;
      uVar8 = uVar6 & 0x80000003;
      if ((int)uVar8 < 0) {
        uVar8 = (uVar8 - 1 | 0xfffffffc) + 1;
      }
      pfVar1 = (float *)(puVar5 + 0x16);
      fVar2 = (float)uVar8 * _DAT_005a3568;
      iVar7 = puVar5[0x29];
      *puVar5 = 0x2100;
      fVar4 = (float)((int)(uVar6 + ((int)uVar6 >> 0x1f & 3U)) >> 2) * _DAT_005a3568;
      *(float *)(iVar7 + 0x18) = fVar2;
      *(float *)(iVar7 + 0x1c) = fVar4;
      fVar3 = fVar2 + _DAT_005a3568;
      *(float *)(iVar7 + 0x58) = fVar3;
      *(float *)(iVar7 + 0x5c) = fVar4;
      *(float *)(iVar7 + 0x38) = fVar2;
      fVar4 = fVar4 + _DAT_005a3568;
      *(float *)(iVar7 + 0x3c) = fVar4;
      *(float *)(iVar7 + 0x78) = fVar3;
      *(float *)(iVar7 + 0x7c) = fVar4;
      FUN_0043a9f0(pfVar1);
      FUN_0043b230(pfVar1,(float)-pdVar9[-2] * (float)_DAT_005a8a98);
      FUN_0043b180(pfVar1,-0.5235988);
      FUN_0043b1d0(pfVar1,1.047198);
      iVar7 = (uint)*(byte *)(this->mbr_0x4 + 0x36) * -0x168;
      FUN_0043b230(pfVar1,(float)((int)(iVar7 + (iVar7 >> 0x1f & 0xffU)) >> 8) *
                          (float)_DAT_005a8a98);
      fVar2 = (float)pdVar9[-3];
      puVar5[0x12] = fVar2;
      puVar5[0x11] = fVar2;
      puVar5[0x10] = fVar2;
      FUN_0043b130(pfVar1,(float *)(puVar5 + 0x10));
      puVar5[4] = pdVar9[-6];
      fVar2 = (float)pdVar9[-4];
      puVar5[5] = pdVar9[-5];
      puVar5[6] = fVar2;
      FUN_0043b090(pfVar1,puVar5 + 4);
      cls_0x5a486c::meth_0x40a8f0((cls_0x5a486c *)this->mbr_0x8);
      FUN_0040c9c0(&this->mbr_0x9c);
      if ((this->mbr_0x3c & 1) != 0) {
        FUN_0040ca20((int *)&this->mbr_0x9c);
      }
      this->mbr_0xec = 1;
      if ((this->mbr_0x3c & 2) != 0) {
        cls_0x5a486c::meth_0x40ca90((cls_0x5a486c *)this->mbr_0x8,this->mbr_0x4,this->mbr_0xc);
      }
      this->mbr_0xec = 1;
    }
    pdVar9 = pdVar9 + 8;
    local_c = local_c + -1;
  } while (local_c != 0);
  return;
}



// Function at 00513ed0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x5b4a7c::meth_0x513ed0(cls_0x5b4a7c *this)

{
  float *pfVar1;
  float fVar2;
  dword dVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  float fVar7;
  float fVar8;
  uint *puVar9;
  int iVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  dword *pdVar14;
  longlong lVar15;
  int local_28;
  int local_18;
  
  dVar3 = this->mbr_0x4;
  iVar4 = *(int *)(dVar3 + 0x10);
  iVar5 = *(int *)(dVar3 + 0x14);
  iVar6 = *(int *)(dVar3 + 0x18);
  puVar9 = (uint *)cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  local_28 = 10;
  pdVar14 = &this->mbr_0x220;
  iVar10 = 9;
  do {
    iVar12 = iVar10;
    if ((((float)pdVar14[-1] == (float)this->mbr_0xfc) &&
        ((float)*pdVar14 == *(float *)&this->field_0x100)) &&
       ((float)pdVar14[1] == *(float *)&this->field_0x104)) break;
    pdVar14 = pdVar14 + -8;
    iVar10 = iVar12 + -1;
    local_28 = iVar12;
  } while (0 < iVar12 + -1);
  if (local_28 < 10) {
    local_18 = 10 - local_28;
    pfVar1 = (float *)(puVar9 + 0x16);
    pdVar14 = &this->mbr_0x228;
    do {
      FUN_0040c960();
      this->mbr_0xec = 0;
      *puVar9 = 0x100;
      FUN_0043a9f0(pfVar1);
      uVar11 = this->mbr_0x244;
      uVar13 = uVar11 & 0x80000003;
      if ((int)uVar13 < 0) {
        uVar13 = (uVar13 - 1 | 0xfffffffc) + 1;
      }
      fVar2 = (float)uVar13 * _DAT_005a3568;
      *puVar9 = *puVar9 | 0x2000;
      uVar13 = puVar9[0x29];
      fVar8 = (float)((int)(uVar11 + ((int)uVar11 >> 0x1f & 3U)) >> 2) * _DAT_005a3568;
      *(float *)(uVar13 + 0x18) = fVar2;
      *(float *)(uVar13 + 0x1c) = fVar8;
      fVar7 = fVar2 + _DAT_005a3568;
      *(float *)(uVar13 + 0x58) = fVar7;
      *(float *)(uVar13 + 0x5c) = fVar8;
      *(float *)(uVar13 + 0x38) = fVar2;
      fVar8 = fVar8 + _DAT_005a3568;
      *(float *)(uVar13 + 0x3c) = fVar8;
      *(float *)(uVar13 + 0x78) = fVar7;
      *(float *)(uVar13 + 0x7c) = fVar8;
      FUN_0043b180(pfVar1,-0.5235988);
      FUN_0043b1d0(pfVar1,1.047198);
      iVar10 = (uint)*(byte *)(this->mbr_0x4 + 0x36) * -0x168;
      FUN_0043b230(pfVar1,(float)((int)(iVar10 + (iVar10 >> 0x1f & 0xffU)) >> 8) *
                          (float)_DAT_005a8a98);
      fVar2 = (float)pdVar14[3] * (float)*pdVar14;
      puVar9[0x12] = (uint)fVar2;
      puVar9[0x11] = (uint)fVar2;
      puVar9[0x10] = (uint)fVar2;
      FUN_0043b130(pfVar1,(float *)(puVar9 + 0x10));
      puVar9[4] = (uint)((float)pdVar14[-3] - (float)iVar4);
      puVar9[5] = (uint)((float)pdVar14[-2] - (float)iVar5);
      puVar9[6] = (uint)((float)pdVar14[-1] - (float)iVar6);
      FUN_0043b090(pfVar1,puVar9 + 4);
      cls_0x5a486c::meth_0x40a8f0((cls_0x5a486c *)this->mbr_0x8);
      FUN_0040c9c0(&this->mbr_0x9c);
      if ((this->mbr_0x3c & 1) != 0) {
        FUN_0040ca20((int *)&this->mbr_0x9c);
      }
      this->mbr_0xec = 1;
      if ((this->mbr_0x3c & 2) != 0) {
        cls_0x5a486c::meth_0x40ca90((cls_0x5a486c *)this->mbr_0x8,this->mbr_0x4,this->mbr_0xc);
      }
      pdVar14 = pdVar14 + -8;
      local_18 = local_18 + -1;
      this->mbr_0xec = 1;
    } while (local_18 != 0);
    if (local_28 < 10) {
      local_18 = 10 - local_28;
      pfVar1 = (float *)(puVar9 + 0x16);
      pdVar14 = &this->mbr_0x22c;
      do {
        FUN_0040c960();
        this->mbr_0xec = 0;
        lVar15 = __ftol();
        uVar11 = (uint)lVar15;
        uVar13 = uVar11 & 0x80000003;
        if ((int)uVar13 < 0) {
          uVar13 = (uVar13 - 1 | 0xfffffffc) + 1;
        }
        fVar2 = (float)uVar13 * _DAT_005a3568;
        *puVar9 = 0x2100;
        uVar13 = puVar9[0x29];
        fVar8 = (float)((int)(uVar11 + ((int)uVar11 >> 0x1f & 3U)) >> 2) * _DAT_005a3568;
        *(float *)(uVar13 + 0x18) = fVar2;
        *(float *)(uVar13 + 0x1c) = fVar8;
        fVar7 = fVar2 + _DAT_005a3568;
        *(float *)(uVar13 + 0x58) = fVar7;
        *(float *)(uVar13 + 0x5c) = fVar8;
        *(float *)(uVar13 + 0x38) = fVar2;
        fVar8 = fVar8 + _DAT_005a3568;
        *(float *)(uVar13 + 0x3c) = fVar8;
        *(float *)(uVar13 + 0x78) = fVar7;
        *(float *)(uVar13 + 0x7c) = fVar8;
        FUN_0043a9f0(pfVar1);
        FUN_0043b230(pfVar1,(float)-*pdVar14 * (float)_DAT_005a8a98);
        FUN_0043b180(pfVar1,-0.5235988);
        FUN_0043b1d0(pfVar1,1.047198);
        iVar10 = (uint)*(byte *)(this->mbr_0x4 + 0x36) * -0x168;
        FUN_0043b230(pfVar1,(float)((int)(iVar10 + (iVar10 >> 0x1f & 0xffU)) >> 8) *
                            (float)_DAT_005a8a98);
        fVar2 = (float)pdVar14[-1];
        puVar9[0x12] = (uint)fVar2;
        puVar9[0x11] = (uint)fVar2;
        puVar9[0x10] = (uint)fVar2;
        FUN_0043b130(pfVar1,(float *)(puVar9 + 0x10));
        puVar9[4] = (uint)((float)pdVar14[-4] - (float)iVar4);
        puVar9[5] = (uint)((float)pdVar14[-3] - (float)iVar5);
        puVar9[6] = (uint)((float)pdVar14[-2] - (float)iVar6);
        FUN_0043b090(pfVar1,puVar9 + 4);
        cls_0x5a486c::meth_0x40a8f0((cls_0x5a486c *)this->mbr_0x8);
        FUN_0040c9c0(&this->mbr_0x9c);
        if ((this->mbr_0x3c & 1) != 0) {
          FUN_0040ca20((int *)&this->mbr_0x9c);
        }
        this->mbr_0xec = 1;
        if ((this->mbr_0x3c & 2) != 0) {
          cls_0x5a486c::meth_0x40ca90((cls_0x5a486c *)this->mbr_0x8,this->mbr_0x4,this->mbr_0xc);
        }
        pdVar14 = pdVar14 + -8;
        local_18 = local_18 + -1;
        this->mbr_0xec = 1;
      } while (local_18 != 0);
    }
  }
  return;
}



// Function at 00514350

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x5b4a7c::meth_0x514350(cls_0x5b4a7c *this)

{
  float *pfVar1;
  float fVar2;
  dword dVar3;
  float fVar4;
  float fVar5;
  undefined4 *puVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  longlong lVar10;
  
  puVar6 = (undefined4 *)cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  FUN_0040c960();
  this->mbr_0xec = 0;
  lVar10 = __ftol();
  uVar7 = (uint)lVar10;
  uVar9 = uVar7 & 0x80000003;
  if ((int)uVar9 < 0) {
    uVar9 = (uVar9 - 1 | 0xfffffffc) + 1;
  }
  pfVar1 = (float *)(puVar6 + 0x16);
  fVar2 = (float)uVar9 * _DAT_005a3568;
  iVar8 = puVar6[0x29];
  *puVar6 = 0x2100;
  fVar5 = (float)((int)(uVar7 + ((int)uVar7 >> 0x1f & 3U)) >> 2) * _DAT_005a3568;
  *(float *)(iVar8 + 0x18) = fVar2;
  *(float *)(iVar8 + 0x1c) = fVar5;
  fVar4 = fVar2 + _DAT_005a3568;
  *(float *)(iVar8 + 0x58) = fVar4;
  *(float *)(iVar8 + 0x5c) = fVar5;
  *(float *)(iVar8 + 0x38) = fVar2;
  fVar5 = fVar5 + _DAT_005a3568;
  *(float *)(iVar8 + 0x3c) = fVar5;
  *(float *)(iVar8 + 0x78) = fVar4;
  *(float *)(iVar8 + 0x7c) = fVar5;
  FUN_0043a9f0(pfVar1);
  FUN_0043b230(pfVar1,(float)-this->mbr_0x4bc * (float)_DAT_005a8a98);
  FUN_0043b180(pfVar1,-0.5235988);
  FUN_0043b1d0(pfVar1,1.047198);
  iVar8 = (uint)*(byte *)(this->mbr_0x4 + 0x36) * -0x168;
  FUN_0043b230(pfVar1,(float)((int)(iVar8 + (iVar8 >> 0x1f & 0xffU)) >> 8) * (float)_DAT_005a8a98);
  fVar2 = (float)this->mbr_0x4b8;
  puVar6[0x12] = fVar2;
  puVar6[0x11] = fVar2;
  puVar6[0x10] = fVar2;
  FUN_0043b130(pfVar1,(float *)(puVar6 + 0x10));
  puVar6[4] = this->mbr_0x4ac;
  dVar3 = this->mbr_0x4b4;
  puVar6[5] = this->mbr_0x4b0;
  puVar6[6] = dVar3;
  FUN_0043b090(pfVar1,puVar6 + 4);
  cls_0x5a486c::meth_0x40a8f0((cls_0x5a486c *)this->mbr_0x8);
  FUN_0040c9c0(&this->mbr_0x9c);
  if ((this->mbr_0x3c & 1) != 0) {
    FUN_0040ca20((int *)&this->mbr_0x9c);
  }
  this->mbr_0xec = 1;
  if ((this->mbr_0x3c & 2) != 0) {
    cls_0x5a486c::meth_0x40ca90((cls_0x5a486c *)this->mbr_0x8,this->mbr_0x4,this->mbr_0xc);
  }
  this->mbr_0xec = 1;
  return;
}



// Function at 00514520

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x5b4a7c::meth_0x514520(cls_0x5b4a7c *this)

{
  float *pfVar1;
  uint uVar2;
  dword dVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  undefined4 *puVar7;
  int iVar8;
  uint uVar9;
  
  puVar7 = (undefined4 *)cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  FUN_0040c960();
  uVar2 = this->mbr_0x244;
  this->mbr_0xec = 0;
  uVar9 = uVar2 & 0x80000003;
  if ((int)uVar9 < 0) {
    uVar9 = (uVar9 - 1 | 0xfffffffc) + 1;
  }
  pfVar1 = (float *)(puVar7 + 0x16);
  fVar4 = (float)uVar9 * _DAT_005a3568;
  iVar8 = puVar7[0x29];
  *puVar7 = 0x2100;
  fVar6 = (float)((int)(uVar2 + ((int)uVar2 >> 0x1f & 3U)) >> 2) * _DAT_005a3568;
  *(float *)(iVar8 + 0x18) = fVar4;
  *(float *)(iVar8 + 0x1c) = fVar6;
  fVar5 = fVar4 + _DAT_005a3568;
  *(float *)(iVar8 + 0x58) = fVar5;
  *(float *)(iVar8 + 0x5c) = fVar6;
  *(float *)(iVar8 + 0x38) = fVar4;
  fVar6 = fVar6 + _DAT_005a3568;
  *(float *)(iVar8 + 0x3c) = fVar6;
  *(float *)(iVar8 + 0x78) = fVar5;
  *(float *)(iVar8 + 0x7c) = fVar6;
  FUN_0043a9f0(pfVar1);
  FUN_0043b180(pfVar1,-0.5235988);
  FUN_0043b1d0(pfVar1,1.047198);
  iVar8 = (uint)*(byte *)(this->mbr_0x4 + 0x36) * -0x168;
  FUN_0043b230(pfVar1,(float)((int)(iVar8 + (iVar8 >> 0x1f & 0xffU)) >> 8) * (float)_DAT_005a8a98);
  fVar4 = (float)this->mbr_0x4c4 * (float)this->mbr_0x4b8;
  puVar7[0x12] = fVar4;
  puVar7[0x11] = fVar4;
  puVar7[0x10] = fVar4;
  FUN_0043b130(pfVar1,(float *)(puVar7 + 0x10));
  dVar3 = this->mbr_0x4ac;
  puVar7[5] = this->mbr_0x4b0;
  puVar7[4] = dVar3;
  puVar7[6] = this->mbr_0x4b4;
  FUN_0043b090(pfVar1,puVar7 + 4);
  cls_0x5a486c::meth_0x40a8f0((cls_0x5a486c *)this->mbr_0x8);
  FUN_0040c9c0(&this->mbr_0x9c);
  if ((this->mbr_0x3c & 1) != 0) {
    FUN_0040ca20((int *)&this->mbr_0x9c);
  }
  this->mbr_0xec = 1;
  if ((this->mbr_0x3c & 2) != 0) {
    cls_0x5a486c::meth_0x40ca90((cls_0x5a486c *)this->mbr_0x8,this->mbr_0x4,this->mbr_0xc);
  }
  this->mbr_0xec = 1;
  return;
}



// Function at 005146d0

undefined4 __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x5146d0(cls_0x5b4a7c *this)

{
  dword dVar1;
  
  FUN_004de1f0();
  FUN_004de210();
  (**(code **)(this->mbr_0x248 + 0x10))();
  dVar1 = this->mbr_0xc;
  if (-1 < (int)dVar1) {
    if ((int)dVar1 < 2) {
      meth_0x514520(this);
      meth_0x513ed0(this);
      meth_0x514350(this);
    }
    else if (dVar1 == 2) {
      meth_0x513ed0(this);
      meth_0x513bb0(this);
      (**(code **)(this->mbr_0x2d8 + 0x14))();
      FUN_004de200();
      return 1;
    }
  }
  FUN_004de200();
  return 1;
}



// Function at 005156c0

cls_0x5b4a7c * __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x5156c0(cls_0x5b4a7c *this)

{
  byte in_stack_00000004;
  
  cls_0x5156e0::~cls_0x5156e0((cls_0x5156e0 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00515970

cls_0x5b4a7c * __thiscall OOAnalyzer::cls_0x5b4a7c::~cls_0x5b4a7c(cls_0x5b4a7c *this)

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int iVar2;
  int iVar3;
  cls_0x5b4a7c *pcVar4;
  uint uVar5;
  uint uVar6;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0fd8;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &cls_0x5b4a7c__vftable_5b4a7c_005b4a7c;
  local_4 = 0;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30::meth_0x46e630((cls_0x5b4f30 *)this);
  }
  uVar1 = (*this->vftptr_0x0->FUN_00477aa0_220)(DAT_0066d694);
  if (CONCAT31(extraout_var,uVar1) < 0x40) {
    uVar1 = (*this->vftptr_0x0->FUN_00477aa0_220)(DAT_0066d694);
    if (CONCAT31(extraout_var_00,uVar1) == 0) {
      uVar6 = 1;
      goto LAB_00515a0b;
    }
  }
  uVar1 = (*this->vftptr_0x0->FUN_00477aa0_220)(DAT_0066d694);
  if (CONCAT31(extraout_var_01,uVar1) < 0x40) {
    uVar1 = (*this->vftptr_0x0->FUN_00477aa0_220)(DAT_0066d694);
    uVar6 = CONCAT31(extraout_var_02,uVar1);
  }
  else {
    uVar6 = 0x40;
  }
LAB_00515a0b:
  iVar2 = (int)*(short *)((int)&this->mbr_0x4 + 2);
  if (((&DAT_0066d584)[uVar6 + iVar2 * 0x41] != 0) && (this->mbr_0x64 == DAT_0065d674)) {
    if (uVar6 < 0x40) {
      uVar5 = uVar6;
      if (uVar6 == 0) {
        uVar5 = 1;
      }
    }
    else {
      uVar5 = 0x40;
    }
    iVar2 = uVar5 + iVar2 * 0x41;
    if ((((&DAT_0066d584)[iVar2] != 0) &&
        (iVar3 = (&DAT_0066d584)[iVar2] + -1, (&DAT_0066d584)[iVar2] = iVar3, iVar3 < 1)) &&
       (uVar5 * 0x104 != -0x66d6a0)) {
      FUN_004830f0((LPCVOID)(&DAT_0066d6a0)[iVar2]);
      (&DAT_0066d6a0)[iVar2] = 0;
    }
  }
  iVar2 = (int)*(short *)((int)&this->mbr_0x4 + 2);
  if ((&DAT_0066d480)[uVar6 + iVar2 * 0x41] != 0) {
    if (uVar6 < 0x40) {
      if (uVar6 == 0) {
        uVar6 = 1;
      }
    }
    else {
      uVar6 = 0x40;
    }
    iVar2 = uVar6 + iVar2 * 0x41;
    if ((((&DAT_0066d480)[iVar2] != 0) &&
        (iVar3 = (&DAT_0066d480)[iVar2] + -1, (&DAT_0066d480)[iVar2] = iVar3, iVar3 < 1)) &&
       (uVar6 * 0x104 != -0x66d7e4)) {
      FUN_004830f0((LPCVOID)(&DAT_0066d7e4)[iVar2]);
      (&DAT_0066d7e4)[iVar2] = 0;
    }
  }
  puStack_8 = (undefined *)0xffffffff;
  pcVar4 = (cls_0x5b4a7c *)TScreen::~TScreen((TScreen *)this);
  ExceptionList = this;
  return pcVar4;
}



// Function at 00515b50

undefined4 __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x515b50(cls_0x5b4a7c *this)

{
  undefined uVar1;
  int *piVar2;
  int iVar3;
  undefined3 extraout_var;
  uint uVar4;
  undefined4 *puVar5;
  uint uVar6;
  int *in_stack_00000004;
  undefined4 uVar7;
  
  SoftwareRenderer::virt_meth_0x46fee0((SoftwareRenderer *)this);
  piVar2 = (int *)(**(code **)(*in_stack_00000004 + 0xa8))(&DAT_005e1d98);
  if (((piVar2 != (int *)0x0) && (*(short *)&this->mbr_0x4 == *(short *)(piVar2 + 1))) &&
     (*(short *)((int)&this->mbr_0x4 + 2) == *(short *)((int)piVar2 + 6))) {
    iVar3 = (**(code **)(*piVar2 + 0x198))();
    uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    (**(code **)(*piVar2 + 0x19c))(iVar3 + CONCAT31(extraout_var,uVar1));
    return 1;
  }
  if ((this->mbr_0x64 == DAT_0065d674) && (piVar2 = (int *)this->mbr_0x54, piVar2 != (int *)0x0)) {
    uVar6 = (uint)*(ushort *)&this->mbr_0xc;
    (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    uVar4 = (uint)*(short *)((int)&this->mbr_0x4 + 2);
    uVar7 = 0;
    puVar5 = (undefined4 *)(**(code **)(*piVar2 + 0xd4))();
    FUN_00515fb0(puVar5,uVar6,uVar7,uVar4);
  }
  return 0;
}



// Function at 00515c00

void __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x515c00(cls_0x5b4a7c *this)

{
  undefined uVar1;
  undefined3 extraout_var;
  uint uVar2;
  int iVar3;
  int iVar4;
  
  SoftwareRenderer::virt_meth_0x46faf0((SoftwareRenderer *)this);
  if (this->mbr_0x64 == DAT_0065d674) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    uVar2 = CONCAT31(extraout_var,uVar1);
    if (uVar2 < 0x40) {
      if (uVar2 == 0) {
        uVar2 = 1;
      }
    }
    else {
      uVar2 = 0x40;
    }
    iVar4 = uVar2 + *(short *)((int)&this->mbr_0x4 + 2) * 0x41;
    if ((((&DAT_0066d584)[iVar4] != 0) &&
        (iVar3 = (&DAT_0066d584)[iVar4] + -1, (&DAT_0066d584)[iVar4] = iVar3, iVar3 < 1)) &&
       (uVar2 * 0x104 != -0x66d6a0)) {
      FUN_004830f0((LPCVOID)(&DAT_0066d6a0)[iVar4]);
      (&DAT_0066d6a0)[iVar4] = 0;
    }
  }
  return;
}



// Function at 00515c90

void __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x515c90(cls_0x5b4a7c *this)

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  uint uVar2;
  undefined4 *puVar3;
  undefined3 extraout_var_01;
  cls_0x5b4a7c__vftable_5b4a7c *pcVar4;
  int iVar5;
  uint unaff_EDI;
  int iVar6;
  uint in_stack_00000004;
  undefined4 uVar7;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
  if (CONCAT31(extraout_var,uVar1) == in_stack_00000004) {
    return;
  }
  if ((int)in_stack_00000004 < 1) {
    return;
  }
  if (this->mbr_0x64 == DAT_0065d674) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    uVar2 = CONCAT31(extraout_var_00,uVar1);
    if (uVar2 < 0x40) {
      if (uVar2 == 0) {
        uVar2 = 1;
      }
    }
    else {
      uVar2 = 0x40;
    }
    iVar6 = uVar2 + *(short *)((int)&this->mbr_0x4 + 2) * 0x41;
    if ((((&DAT_0066d584)[iVar6] != 0) &&
        (iVar5 = (&DAT_0066d584)[iVar6] + -1, (&DAT_0066d584)[iVar6] = iVar5, iVar5 < 1)) &&
       (uVar2 * 0x104 != -0x66d6a0)) {
      FUN_004830f0((LPCVOID)(&DAT_0066d6a0)[iVar6]);
      (&DAT_0066d6a0)[iVar6] = 0;
    }
    if ((int *)this->mbr_0x54 != (int *)0x0) {
      iVar6 = (int)*(short *)((int)&this->mbr_0x4 + 2);
      uVar2 = in_stack_00000004;
      puVar3 = (undefined4 *)
               (**(code **)(*(int *)this->mbr_0x54 + 0xd4))(*(undefined2 *)&this->mbr_0xc,0);
      FUN_00515fb0(puVar3,iVar6,uVar2,unaff_EDI);
    }
  }
  if (this->mbr_0x64 == 0) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    uVar2 = CONCAT31(extraout_var_01,uVar1);
    if (uVar2 < 0x40) {
      if (uVar2 == 0) {
        uVar2 = 1;
      }
    }
    else {
      uVar2 = 0x40;
    }
    iVar6 = uVar2 + *(short *)((int)&this->mbr_0x4 + 2) * 0x41;
    if ((((&DAT_0066d480)[iVar6] != 0) &&
        (iVar5 = (&DAT_0066d480)[iVar6] + -1, (&DAT_0066d480)[iVar6] = iVar5, iVar5 < 1)) &&
       (uVar2 * 0x104 != -0x66d7e4)) {
      FUN_004830f0((LPCVOID)(&DAT_0066d7e4)[iVar6]);
      (&DAT_0066d7e4)[iVar6] = 0;
    }
    if ((int *)this->mbr_0x54 != (int *)0x0) {
      iVar6 = (int)*(short *)((int)&this->mbr_0x4 + 2);
      uVar2 = in_stack_00000004;
      puVar3 = (undefined4 *)
               (**(code **)(*(int *)this->mbr_0x54 + 0xcc))(*(undefined2 *)&this->mbr_0xc,0);
      FUN_00516130(puVar3,iVar6,uVar2);
    }
  }
  if ((int)in_stack_00000004 < 0xb) {
    pcVar4 = this->vftptr_0x0;
    uVar7 = 0;
  }
  else {
    if ((int)in_stack_00000004 < 0x65) {
      (*this->vftptr_0x0->FUN_0046f250_24)(2);
      goto LAB_00515e4b;
    }
    if ((int)in_stack_00000004 < 0x12d) {
      pcVar4 = this->vftptr_0x0;
      uVar7 = 4;
    }
    else {
      if ((int)in_stack_00000004 < 0x1f5) {
        (*this->vftptr_0x0->FUN_0046f250_24)(6);
        goto LAB_00515e4b;
      }
      pcVar4 = this->vftptr_0x0;
      uVar7 = 8;
    }
  }
  (*pcVar4->FUN_0046f250_24)(uVar7);
LAB_00515e4b:
  (*this->vftptr_0x0->virt_meth_0x477b00_232)(this);
  return;
}



// Function at 00515f40

void __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x515f40(cls_0x5b4a7c *this)

{
  int iVar1;
  int unaff_retaddr;
  int in_stack_00000004;
  
  (*this->vftptr_0x0->FUN_00472e90_64)(this->mbr_0x8 & 0xfbffffff);
  SoftwareRenderer::virt_meth_0x472430((SoftwareRenderer *)this,unaff_retaddr);
  if (in_stack_00000004 < 5) {
    iVar1 = **(int **)(unaff_retaddr + 4);
    *(int **)(unaff_retaddr + 4) = *(int **)(unaff_retaddr + 4) + 1;
    (*this->vftptr_0x0->FUN_004736f0_224)(s_Amount_005e1da0,iVar1 + 1);
  }
  return;
}



// Function at 005162c0

void __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x5162c0(cls_0x5b4a7c *this)

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int iVar2;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined4 *puVar3;
  int iVar4;
  uint unaff_ESI;
  undefined4 in_stack_00000004;
  undefined4 in_stack_00000008;
  undefined *in_stack_0000000c;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
  if (CONCAT31(extraout_var,uVar1) < 0x40) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    if (CONCAT31(extraout_var_00,uVar1) == 0) {
      iVar2 = 1;
      goto LAB_00516306;
    }
  }
  uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
  if (CONCAT31(extraout_var_01,uVar1) < 0x40) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    iVar2 = CONCAT31(extraout_var_02,uVar1);
  }
  else {
    iVar2 = 0x40;
  }
LAB_00516306:
  iVar4 = (int)*(short *)((int)&this->mbr_0x4 + 2);
  if (((int)(&DAT_0066d584)[iVar2 + iVar4 * 0x41] < 1) && ((int *)this->mbr_0x54 != (int *)0x0)) {
    puVar3 = (undefined4 *)
             (**(code **)(*(int *)this->mbr_0x54 + 0xd4))(*(undefined2 *)&this->mbr_0xc,0);
    FUN_00515fb0(puVar3,iVar4,iVar2,unaff_ESI);
  }
  if (in_stack_0000000c == (undefined *)0x0) {
    in_stack_0000000c = PTR_DAT_005d79e0;
  }
  (**(code **)(*(int *)this->mbr_0x54 + 0x24))
            (this,in_stack_00000004,in_stack_00000008,in_stack_0000000c);
  return;
}



// Function at 00516370

undefined4 __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x516370(cls_0x5b4a7c *this)

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int iVar2;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined4 *puVar3;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  undefined3 extraout_var_05;
  undefined3 extraout_var_06;
  int iVar4;
  uint unaff_ESI;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
  if (CONCAT31(extraout_var,uVar1) < 0x40) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    if (CONCAT31(extraout_var_00,uVar1) != 0) goto LAB_00516396;
    iVar2 = 1;
  }
  else {
LAB_00516396:
    uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    if (CONCAT31(extraout_var_01,uVar1) < 0x40) {
      uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
      iVar2 = CONCAT31(extraout_var_02,uVar1);
    }
    else {
      iVar2 = 0x40;
    }
  }
  iVar4 = (int)*(short *)((int)&this->mbr_0x4 + 2);
  if (((int)(&DAT_0066d584)[iVar2 + iVar4 * 0x41] < 1) && ((int *)this->mbr_0x54 != (int *)0x0)) {
    puVar3 = (undefined4 *)
             (**(code **)(*(int *)this->mbr_0x54 + 0xd4))(*(undefined2 *)&this->mbr_0xc,0);
    FUN_00515fb0(puVar3,iVar4,iVar2,unaff_ESI);
  }
  uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
  if (CONCAT31(extraout_var_03,uVar1) < 0x40) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    if (CONCAT31(extraout_var_04,uVar1) == 0) {
      iVar2 = 1;
      goto LAB_0051643d;
    }
  }
  uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
  if (CONCAT31(extraout_var_05,uVar1) < 0x40) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    iVar2 = CONCAT31(extraout_var_06,uVar1);
  }
  else {
    iVar2 = 0x40;
  }
LAB_0051643d:
  return (&DAT_0066d6a0)[iVar2 + *(short *)((int)&this->mbr_0x4 + 2) * 0x41];
}



// Function at 005164d0

void __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x5164d0(cls_0x5b4a7c *this)

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  uint uVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined4 in_stack_00000004;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
  if (CONCAT31(extraout_var,uVar1) < 0x40) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    uVar2 = CONCAT31(extraout_var_00,uVar1);
  }
  else {
    uVar2 = 0x40;
  }
  iVar4 = (int)*(short *)((int)&this->mbr_0x4 + 2);
  if (((int)(&DAT_0066d480)[uVar2 + iVar4 * 0x41] < 1) && ((int *)this->mbr_0x54 != (int *)0x0)) {
    puVar3 = (undefined4 *)
             (**(code **)(*(int *)this->mbr_0x54 + 0xcc))(*(undefined2 *)&this->mbr_0xc,0);
    FUN_00516130(puVar3,iVar4,uVar2);
  }
  (**(code **)(*(int *)this->mbr_0x54 + 0x10))(this,in_stack_00000004);
  return;
}



// Function at 00516550

uint __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x516550(cls_0x5b4a7c *this)

{
  int *piVar1;
  undefined uVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int iVar3;
  undefined4 *puVar4;
  uint uVar5;
  uint uVar6;
  
  uVar2 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
  if (CONCAT31(extraout_var,uVar2) < 0x40) {
    uVar2 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    iVar3 = CONCAT31(extraout_var_00,uVar2);
  }
  else {
    iVar3 = 0x40;
  }
  uVar6 = (&DAT_0066d7e4)[iVar3 + *(short *)((int)&this->mbr_0x4 + 2) * 0x41];
  if ((uVar6 == 0) && (piVar1 = (int *)this->mbr_0x54, piVar1 != (int *)0x0)) {
    uVar5 = (uint)*(ushort *)&this->mbr_0xc;
    uVar2 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    puVar4 = (undefined4 *)
             (**(code **)(*piVar1 + 0xcc))(uVar5,0,(int)*(short *)((int)&this->mbr_0x4 + 2),uVar2);
    FUN_00516130(puVar4,uVar5,uVar6);
    return 0;
  }
  return uVar6;
}



// Function at 00516660

void __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x516660(cls_0x5b4a7c *this)

{
  undefined uVar1;
  undefined3 extraout_var;
  int iVar2;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
  iVar2 = CONCAT31(extraout_var,uVar1);
  if (iVar2 < 0xb) {
    (*this->vftptr_0x0->FUN_0046f250_24)(0);
    SoftwareRenderer::virt_meth_0x46f3a0((SoftwareRenderer *)this);
    return;
  }
  if (iVar2 < 0x65) {
    (*this->vftptr_0x0->FUN_0046f250_24)(2);
    SoftwareRenderer::virt_meth_0x46f3a0((SoftwareRenderer *)this);
    return;
  }
  if (iVar2 < 0x12d) {
    (*this->vftptr_0x0->FUN_0046f250_24)(4);
    SoftwareRenderer::virt_meth_0x46f3a0((SoftwareRenderer *)this);
    return;
  }
  if (iVar2 < 0x1f5) {
    (*this->vftptr_0x0->FUN_0046f250_24)(6);
    SoftwareRenderer::virt_meth_0x46f3a0((SoftwareRenderer *)this);
    return;
  }
  (*this->vftptr_0x0->FUN_0046f250_24)(8);
  SoftwareRenderer::virt_meth_0x46f3a0((SoftwareRenderer *)this);
  return;
}



// Function at 00516760

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x516760(cls_0x5b4a7c *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a100e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0xfc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x516800_005b4c70;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00516830

void __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x516830(cls_0x5b4a7c *this)

{
  int *piVar1;
  undefined4 *puVar2;
  uint uVar3;
  uint uVar4;
  undefined4 uVar5;
  
  piVar1 = (int *)this->mbr_0x54;
  if (piVar1 != (int *)0x0) {
    uVar4 = (uint)*(ushort *)&this->mbr_0xc;
    (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
    uVar3 = (uint)*(short *)((int)&this->mbr_0x4 + 2);
    uVar5 = 0;
    puVar2 = (undefined4 *)(**(code **)(*piVar1 + 0xd4))();
    FUN_00515fb0(puVar2,uVar4,uVar5,uVar3);
  }
  return;
}



// Function at 00516870

void __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x516870(cls_0x5b4a7c *this)

{
  undefined uVar1;
  undefined3 extraout_var;
  uint uVar2;
  int iVar3;
  int iVar4;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x516920_408)(this);
  uVar2 = CONCAT31(extraout_var,uVar1);
  if (uVar2 < 0x40) {
    if (uVar2 == 0) {
      uVar2 = 1;
    }
  }
  else {
    uVar2 = 0x40;
  }
  iVar4 = uVar2 + *(short *)((int)&this->mbr_0x4 + 2) * 0x41;
  if ((((&DAT_0066d584)[iVar4] != 0) &&
      (iVar3 = (&DAT_0066d584)[iVar4] + -1, (&DAT_0066d584)[iVar4] = iVar3, iVar3 < 1)) &&
     (uVar2 * 0x104 != -0x66d6a0)) {
    FUN_004830f0((LPCVOID)(&DAT_0066d6a0)[iVar4]);
    (&DAT_0066d6a0)[iVar4] = 0;
  }
  return;
}



// Function at 00516930

cls_0x5b4a7c * __thiscall OOAnalyzer::cls_0x5b4a7c::virt_meth_0x516930(cls_0x5b4a7c *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b4a7c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00516950

TScreen * __thiscall
OOAnalyzer::cls_0x5b4a7c::virt_meth_0x516950(cls_0x5b4a7c *this,undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a102b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xd8);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&cls_0x5b4a7c__vftable_5b4a7c_005b4a7c;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



