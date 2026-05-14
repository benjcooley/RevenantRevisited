#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a7e98



// Function at 004d90b0

void cls_0x5a7e98::meth_0x4d90b0()

{
  dword dVar1;
  LPCVOID pvVar2;
  short sVar3;
  dword dVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  int iVar7;
  int iVar8;
  dword *in_stack_00000004;
  
  if (this->mbr_0x380 == 0) {
    this->mbr_0x380 = 1;
    dVar4 = in_stack_00000004[7];
    this->mbr_0x3a4 = in_stack_00000004[8];
    dVar1 = in_stack_00000004[6];
    this->mbr_0x3a0 = dVar4;
    dVar4 = in_stack_00000004[9];
    this->mbr_0x39c = dVar1;
    dVar1 = in_stack_00000004[0xb];
    this->mbr_0x3a8 = dVar4;
    dVar4 = in_stack_00000004[10];
    this->mbr_0x3b0 = dVar1;
    dVar1 = in_stack_00000004[0xd];
    this->mbr_0x3ac = dVar4;
    dVar4 = in_stack_00000004[0xc];
    this->mbr_0x3b8 = dVar1;
    this->mbr_0x3b4 = dVar4;
    this->mbr_0x384 = *in_stack_00000004;
    this->mbr_0x388 = in_stack_00000004[1];
    this->mbr_0x38c = in_stack_00000004[2];
    this->mbr_0x390 = in_stack_00000004[3];
    dVar4 = in_stack_00000004[5];
    this->mbr_0x394 = in_stack_00000004[4];
    this->mbr_0x398 = dVar4;
    this->mbr_0x350 = (dword)&this->mbr_0x4;
    if ((&this->mbr_0x4 != (dword *)0x0) && ((LPCVOID)this->mbr_0xa8 != (LPCVOID)0x0)) {
      FUN_004830f0((LPCVOID)this->mbr_0xa8);
    }
    if ((this->mbr_0x350 != 0) &&
       (pvVar2 = *(LPCVOID *)(this->mbr_0x350 + 0xac), pvVar2 != (LPCVOID)0x0)) {
      FUN_004830f0(pvVar2);
    }
    puVar5 = (undefined4 *)this->mbr_0x350;
    for (iVar7 = 0xd3; iVar7 != 0; iVar7 = iVar7 + -1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    dVar4 = (int)this->mbr_0x3ac / (int)this->mbr_0x3b0 + 3;
    this->mbr_0x354 = dVar4;
    puVar5 = FUN_00482fb0(dVar4 * 0xc);
    this->mbr_0x35c = (dword)puVar5;
    puVar6 = FUN_00482fb0(this->mbr_0x354 * 0xc);
    puVar5 = (undefined4 *)this->mbr_0x35c;
    this->mbr_0x360 = (dword)puVar6;
    *puVar5 = 0;
    puVar5[1] = 0;
    puVar5 = (undefined4 *)this->mbr_0x360;
    *puVar5 = 0;
    puVar5[1] = 0;
    iVar7 = (this->mbr_0x354 - 1) * this->mbr_0x3b0;
    dVar4 = iVar7 * 2;
    this->mbr_0x358 = dVar4;
    *(dword *)(this->mbr_0x350 + 0xa0) = dVar4;
    puVar5 = FUN_00482fb0(iVar7 * 0x40);
    dVar4 = this->mbr_0x350;
    *(undefined4 **)(dVar4 + 0xa4) = puVar5;
    dVar1 = this->mbr_0x3ac;
    *(undefined4 *)(dVar4 + 0x9c) = 0x1e2;
    *(dword *)(dVar4 + 0xa8) = dVar1 * 2;
    puVar5 = FUN_00482fb0(dVar1 * 0xc);
    dVar4 = this->mbr_0x350;
    iVar8 = 0;
    *(undefined4 **)(dVar4 + 0xac) = puVar5;
    iVar7 = *(int *)(dVar4 + 0xa8);
    if (0 < iVar7) {
      do {
        sVar3 = (short)iVar8;
        *(short *)puVar5 = sVar3;
        *(short *)((int)puVar5 + 2) = sVar3 + 1;
        *(short *)(puVar5 + 1) = sVar3 + 2;
        iVar8 = iVar8 + 1;
        puVar5 = (undefined4 *)((int)puVar5 + 6);
      } while (iVar8 < iVar7);
    }
    dVar1 = this->mbr_0x3ac;
    *(undefined4 *)(dVar4 + 0xb0) = 0;
    iVar7 = 0;
    *(undefined4 *)(dVar4 + 0x134) = *(undefined4 *)(dVar4 + 0xa8);
    *(undefined4 *)(dVar4 + 0x98) = 4;
    if (dVar1 != 0xffffffff && -1 < (int)(dVar1 + 1)) {
      do {
        meth_0x4d96c0(this);
        iVar7 = iVar7 + 1;
      } while (iVar7 < (int)(this->mbr_0x3ac + 1));
    }
    meth_0x4d9820(this);
  }
  return;
}



// Function at 004d92d0

void cls_0x5a7e98::meth_0x4d92d0()

{
  dword dVar1;
  
  if (this->mbr_0x380 != 0) {
    dVar1 = this->mbr_0x350;
    this->mbr_0x380 = 0;
    if (dVar1 != 0) {
      if (*(LPCVOID *)(dVar1 + 0xac) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)(dVar1 + 0xac));
        dVar1 = this->mbr_0x350;
        *(undefined4 *)(dVar1 + 0xac) = 0;
      }
      if (*(LPCVOID *)(dVar1 + 0xa4) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)(dVar1 + 0xa4));
        *(undefined4 *)(this->mbr_0x350 + 0xa4) = 0;
      }
    }
    if ((LPCVOID)this->mbr_0x35c != (LPCVOID)0x0) {
      FUN_004830f0((LPCVOID)this->mbr_0x35c);
    }
    if ((LPCVOID)this->mbr_0x360 != (LPCVOID)0x0) {
      FUN_004830f0((LPCVOID)this->mbr_0x360);
    }
    this->mbr_0x360 = 0;
    this->mbr_0x35c = 0;
    if ((LPCVOID)this->mbr_0x3b4 != (LPCVOID)0x0) {
      FUN_00482f80((LPCVOID)this->mbr_0x3b4);
      this->mbr_0x3b4 = 0;
    }
  }
  return;
}



// Function at 004d9380

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5a7e98::meth_0x4d9380()

{
  undefined4 *puVar1;
  float fVar2;
  float fVar3;
  dword dVar4;
  int iVar5;
  int iVar6;
  float *pfVar7;
  longlong lVar8;
  longlong lVar9;
  longlong lVar10;
  longlong lVar11;
  float local_34;
  uint local_30;
  int local_2c;
  dword *local_28;
  float local_24;
  int local_1c;
  int local_18;
  float local_c;
  float local_8;
  float local_4;
  
  if (((this->mbr_0x380 != 0) &&
      (puVar1 = (undefined4 *)this->mbr_0x350, puVar1 != (undefined4 *)0x0)) && (puVar1[0x28] != 0))
  {
    *puVar1 = 0x5a110;
    FUN_0043a9f0(puVar1 + 0x16);
    dVar4 = this->mbr_0x3b0;
    local_34 = 0.7;
    local_2c = 0;
    fVar2 = _DAT_005a34e4 / (float)dVar4;
    if (1 < (int)(this->mbr_0x354 - 2)) {
      local_30 = 0;
      iVar5 = 0x18;
      do {
        local_24 = 0.0;
        local_18 = 0;
        if (0 < (int)dVar4) {
          do {
            lVar8 = __ftol();
            local_28 = &this->mbr_0x35c;
            iVar6 = local_2c << 5;
            local_2c = local_2c + 2;
            local_1c = 2;
            do {
              dVar4 = *local_28;
              FUN_0043b280(&local_c,local_24,
                           (float *)(dVar4 + (((int)local_30 < 0) - 1 & local_30) * 0xc),
                           (float *)((dVar4 - 0xc) + iVar5),(float *)(iVar5 + dVar4),
                           (float *)(dVar4 + 0xc + iVar5));
              pfVar7 = (float *)(iVar6 + *(int *)(this->mbr_0x350 + 0xa4));
              *pfVar7 = local_c;
              pfVar7[1] = local_8;
              pfVar7[2] = local_4;
              lVar9 = __ftol();
              lVar10 = __ftol();
              lVar11 = __ftol();
              pfVar7[4] = (float)((((uint)lVar9 | (int)lVar8 << 8) << 8 | (uint)lVar10) << 8 |
                                 (uint)lVar11);
              local_28 = local_28 + 1;
              iVar6 = iVar6 + 0x20;
              local_1c = local_1c + -1;
            } while (local_1c != 0);
            dVar4 = this->mbr_0x3b0;
            local_24 = fVar2 + local_24;
            fVar3 = _DAT_005a7ec0 / (float)((this->mbr_0x354 - 2) * dVar4);
            if (local_34 < fVar3) {
              local_34 = 0.0;
            }
            else {
              local_34 = local_34 - fVar3;
            }
            local_18 = local_18 + 1;
          } while (local_18 < (int)dVar4);
        }
        iVar5 = iVar5 + 0xc;
        iVar6 = local_30 + 2;
        local_30 = local_30 + 1;
      } while (iVar6 < (int)(this->mbr_0x354 - 2));
    }
  }
  return;
}



// Function at 004d95b0

void cls_0x5a7e98::meth_0x4d95b0()

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

void cls_0x5a7e98::meth_0x4d96c0()

{
  byte *pbVar1;
  float *pfVar2;
  uint uVar3;
  int iVar4;
  float *pfVar5;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  float local_8;
  float local_4;
  
  if (this->mbr_0x380 == 0) {
    return;
  }
  if (this->mbr_0x350 == 0) {
    return;
  }
  if (*(int *)(this->mbr_0x350 + 0xa0) == 0) {
    return;
  }
  iVar4 = *(int *)(this->mbr_0x39c + 4);
  if ((*(short *)(iVar4 + 4) == 0xb) && (iVar4 = *(int *)(iVar4 + 0x2b0), iVar4 != 0)) {
    uVar3 = this->mbr_0x3b4;
    if (uVar3 == 0) {
      meth_0x4d92d0(this);
      return;
    }
    pbVar1 = *(byte **)(iVar4 + 0x38);
    if ((pbVar1 != (byte *)0x0) && (uVar3 = FUN_0059a530_stricmp(uVar3,(byte *)uVar3,pbVar1), uVar3 != 0)) {
      meth_0x4d92d0(this);
      return;
    }
  }
  meth_0x4d9ae0(this);
  meth_0x4d9b60(this);
  if ((this->mbr_0x380 == 0) ||
     (((iVar4 = cls_0x5a7e38::meth_0x40edc0((cls_0x5a7e38 *)this->mbr_0x39c), iVar4 < 0 &&
       (iVar4 = cls_0x5a7e38::meth_0x40edc0((cls_0x5a7e38 *)this->mbr_0x39c), iVar4 < 0)) &&
      (iVar4 = cls_0x5a7e38::meth_0x40edc0((cls_0x5a7e38 *)this->mbr_0x39c), iVar4 < 0)))) {
    pfVar5 = (float *)0x0;
  }
  else {
    iVar4 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this->mbr_0x39c);
    pfVar5 = (float *)(iVar4 + 0x58);
  }
  this->mbr_0x37c = (dword)pfVar5;
  FUN_0043ad80(pfVar5,(float *)&this->mbr_0x364,&local_18);
  FUN_0043ad80((float *)this->mbr_0x37c,(float *)&this->mbr_0x370,&local_c);
  pfVar5 = (float *)this->mbr_0x35c;
  pfVar2 = (float *)this->mbr_0x360;
  *pfVar5 = local_18;
  pfVar5[1] = local_14;
  pfVar5[2] = local_10;
  *pfVar2 = local_c;
  pfVar2[1] = local_8;
  pfVar2[2] = local_4;
  return;
}



// Function at 004d9820

void cls_0x5a7e98::meth_0x4d9820()

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

void cls_0x5a7e98::meth_0x4d9ae0()

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

void cls_0x5a7e98::meth_0x4d9b60()

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

void cls_0x5a7e98::meth_0x4d9c60()

{
  dword dVar1;
  int iVar2;
  float *pfVar3;
  float *pfVar4;
  undefined4 *puVar5;
  dword dVar6;
  float local_18;
  undefined4 local_14;
  undefined4 local_10;
  float local_c;
  float local_8;
  float local_4;
  
  if (this->mbr_0x380 != 0) {
    iVar2 = cls_0x5a7e38::meth_0x40edc0((cls_0x5a7e38 *)this->mbr_0x39c);
    if (((iVar2 < 0) &&
        (iVar2 = cls_0x5a7e38::meth_0x40edc0((cls_0x5a7e38 *)this->mbr_0x39c), iVar2 < 0)) &&
       (iVar2 = cls_0x5a7e38::meth_0x40edc0((cls_0x5a7e38 *)this->mbr_0x39c), iVar2 < 0)) {
      pfVar3 = (float *)0x0;
    }
    else {
      iVar2 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this->mbr_0x39c);
      pfVar3 = (float *)(iVar2 + 0x58);
    }
    this->mbr_0x37c = (dword)pfVar3;
    FUN_0043ad80(pfVar3,(float *)&this->mbr_0x364,&local_18);
    FUN_0043ad80((float *)this->mbr_0x37c,(float *)&this->mbr_0x370,&local_c);
    dVar6 = this->mbr_0x354;
    if (0 < (int)dVar6) {
      dVar1 = this->mbr_0x35c;
      pfVar3 = (float *)this->mbr_0x360;
      pfVar4 = pfVar3;
      puVar5 = (undefined4 *)(dVar1 + 8);
      do {
        *(float *)((dVar1 - (int)pfVar3) + (int)pfVar4) = local_18;
        puVar5[-1] = local_14;
        *puVar5 = local_10;
        *pfVar4 = local_c;
        pfVar4[1] = local_8;
        pfVar4[2] = local_4;
        dVar6 = dVar6 - 1;
        pfVar4 = pfVar4 + 3;
        puVar5 = puVar5 + 3;
      } while (dVar6 != 0);
    }
    return;
  }
  return;
}



// Function at 004d9d50

void cls_0x5a7e98::meth_0x4d9d50()

{
  int iVar1;
  
  if (((this->mbr_0x380 != 0) && (this->mbr_0x350 != 0)) && (*(int *)(this->mbr_0x350 + 0xa0) != 0))
  {
    FUN_004de1f0();
    iVar1 = cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),2);
    if (iVar1 != 0) {
      FUN_004a90d0(iVar1);
    }
    meth_0x4d9380(this);
    cls_0x5a486c::meth_0x40a8f0(*(cls_0x5a486c **)(this->mbr_0x39c + 8));
    FUN_004de200();
  }
  return;
}



// Function at 004da930

cls_0x5a7e98 * __thiscall cls_0x5a7e98::~cls_0x5a7e98(cls_0x5a7e98 *this)

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
  if ((LPCVOID)this->mbr_0xb0 != (LPCVOID)0x0) {
    pcVar1 = (cls_0x5a7e98 *)FUN_004830f0((LPCVOID)this->mbr_0xb0);
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 004da9a0

cls_0x5a7e98 * cls_0x5a7e98::virt_meth_0x4da9a0()

{
  byte in_stack_00000004;
  
  ~cls_0x5a7e98(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



