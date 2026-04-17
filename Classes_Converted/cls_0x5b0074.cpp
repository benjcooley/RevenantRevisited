#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b0074



// Function at 0040dcf0

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059c694;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  local_4 = 2;
  this->virt_meth_0x40de10();
  local_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  local_4 = (uint)local_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 004e6920

undefined4 cls_0x5b0074::virt_meth_0x4e6920()

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  
  cls_0x411eb0::meth_0x4178e0((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),2);
  puVar3 = (uint *)cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  puVar3[4] = this->mbr_0x78;
  uVar1 = this->mbr_0x80;
  *puVar3 = *puVar3 | 0x400048;
  uVar2 = this->mbr_0x7c;
  puVar3[6] = uVar1;
  puVar3[5] = uVar2;
  puVar3[0x10] = 0x3f000000;
  puVar3[0x11] = 0x3f000000;
  puVar3[0x12] = 0x3f000000;
  cls_0x5a486c::meth_0x40a8f0((cls_0x5a486c *)this->mbr_0x8);
  cls_0x411eb0::meth_0x417b00((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  return 1;
}



// Function at 004f61e0

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x4f61e0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f6200

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 004f6210

TScreen * cls_0x5b0074::virt_meth_0x4f6210(int param_1)

{
  uint uVar1;
  uint uVar2;
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f25b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x198);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    uVar1 = *(uint *)(param_1 + 4);
    uVar2 = this_00->mbr_0x8;
    *(undefined4 *)&this_00->field_0xe4 = 0;
    this_00[1].mbr_0x38 = 0;
    this_00->mbr_0x8 = uVar2 | uVar1 | 0x48001;
    this_00[1].mbr_0x1c = 0;
    this_00[1].mbr_0xc = 0;
    this_00[1].mbr_0xe = 0;
    *(undefined *)&this_00[1].mbr_0x48 = 0;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4f62e0_005aa200;
    *(undefined4 *)&this_00[1].field_0x80 = 1;
    *(undefined4 *)&this_00[1].field_0x70 = 0;
    *(undefined4 *)&this_00[1].field_0x74 = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 004fa4e0

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x4fa4e0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004fa500

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 004fb600

void cls_0x5b0074::virt_meth_0x4fb600()

{
  undefined4 uVar1;
  
  this->virt_meth_0x40dd60();
  uVar1 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  cls_0x5a486c::meth_0x40a0c0((cls_0x5a486c *)this->mbr_0x8,uVar1);
  uVar1 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  cls_0x5a486c::meth_0x40a0c0((cls_0x5a486c *)this->mbr_0x8,uVar1);
  uVar1 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  cls_0x5a486c::meth_0x40a0c0((cls_0x5a486c *)this->mbr_0x8,uVar1);
  return;
}



// Function at 004fbf50

void cls_0x5b0074::virt_meth_0x4fbf50()

{
  int iVar1;
  int iVar2;
  dword dVar3;
  dword *pdVar4;
  
  this->virt_meth_0x40dd60();
  if (*(int *)(this->mbr_0x4 + 400) == 1) {
    iVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
    if (iVar1 < 0) goto LAB_004fbfc7;
    iVar2 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548);
  }
  else {
    iVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
    if (iVar1 < 0) goto LAB_004fbfc7;
    iVar2 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548);
  }
  if (iVar2 != 0) {
    cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar1,0x7f,1);
  }
LAB_004fbfc7:
  dVar3 = 1;
  pdVar4 = &this->mbr_0x124;
  do {
    *pdVar4 = dVar3;
    dVar3 = dVar3 + 3;
    pdVar4 = pdVar4 + 1;
  } while ((int)dVar3 < 0x2e);
  this->mbr_0x11c = 0;
  this->mbr_0x120 = 0;
  return;
}



// Function at 004fcc00

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 cls_0x5b0074::virt_meth_0x4fcc00()

{
  float fVar1;
  cls_0x5a486c *this_00;
  float fVar2;
  uint *puVar3;
  int iVar4;
  dword *pdVar5;
  
  if (0xf < *(int *)(this->mbr_0x4 + 0x184)) {
    FUN_004de1f0();
    FUN_004de210();
    iVar4 = 0;
    pdVar5 = &this->mbr_0xfc;
    do {
      puVar3 = (uint *)cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
      fVar1 = (float)*pdVar5;
      puVar3[0x12] = (uint)fVar1;
      puVar3[0x11] = (uint)fVar1;
      puVar3[0x10] = (uint)fVar1;
      fVar2 = (float)*pdVar5 * _DAT_005a9eb8;
      *puVar3 = *puVar3 | 0x48;
      puVar3[4] = (uint)fVar2;
      fVar1 = (float)pdVar5[0x10];
      this_00 = (cls_0x5a486c *)this->mbr_0x8;
      puVar3[6] = 0;
      puVar3[5] = (uint)(fVar2 - fVar1);
      cls_0x5a486c::meth_0x40a8f0(this_00);
      iVar4 = iVar4 + 1;
      pdVar5 = pdVar5 + 1;
    } while (iVar4 < 8);
    FUN_004de200();
    return 1;
  }
  return 1;
}



// Function at 004fcd70

void cls_0x5b0074::virt_meth_0x4fcd70()

{
  this->virt_meth_0x40dd60();
  this->mbr_0x3c8 = 0x2d;
  this->mbr_0x3c4 = 0x3dcccccd;
  this->mbr_0x3cc = 1;
  return;
}



// Function at 004fe590

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5b0074::virt_meth_0x4fe590()

{
  float fVar1;
  dword dVar2;
  dword *pdVar3;
  int iVar4;
  longlong lVar5;
  
  this->virt_meth_0x40e2e0();
  dVar2 = this->mbr_0x4c4 + 1;
  this->mbr_0x4c4 = dVar2;
  if (0xf < (int)dVar2) {
    this->mbr_0x4c8 = 2;
  }
  if (this->mbr_0x4c8 == 1) {
    iVar4 = 0x28;
    pdVar3 = &this->mbr_0x2e4;
    do {
      lVar5 = __ftol();
      *pdVar3 = *pdVar3 - (int)lVar5;
      lVar5 = __ftol();
      pdVar3[1] = pdVar3[1] - (int)lVar5;
      lVar5 = __ftol();
      iVar4 = iVar4 + -1;
      pdVar3[2] = pdVar3[2] - (int)lVar5;
      pdVar3 = pdVar3 + 3;
    } while (iVar4 != 0);
  }
  else if (this->mbr_0x4c8 == 2) {
    this->mbr_0xfc = (dword)((float)this->mbr_0xfc + _DAT_005aedb0);
    fVar1 = (float)this->mbr_0x100 - _DAT_005aedac;
    this->mbr_0x100 = (dword)fVar1;
    if (fVar1 <= _DAT_005a3530) {
      this->mbr_0x100 = 0;
      SoftwareRenderer::meth_0x4defe0((SoftwareRenderer *)this->mbr_0x4);
      return;
    }
  }
  return;
}



// Function at 004ff9d0

void cls_0x5b0074::virt_meth_0x4ff9d0()

{
  int iVar1;
  dword *pdVar2;
  dword *pdVar3;
  int iVar4;
  
  this->virt_meth_0x40dd60();
  iVar1 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  cls_0x5a486c::meth_0x40a0c0((cls_0x5a486c *)this->mbr_0x8,iVar1);
  iVar4 = *(int *)(iVar1 + 0xa0);
  if (0 < iVar4) {
    pdVar3 = &this->mbr_0x10c;
    pdVar2 = (dword *)(*(int *)(iVar1 + 0xa4) + 0x1c);
    do {
      pdVar3[-4] = pdVar2[-1];
      *pdVar3 = *pdVar2;
      pdVar3 = pdVar3 + 1;
      iVar4 = iVar4 + -1;
      pdVar2 = pdVar2 + 8;
    } while (iVar4 != 0);
  }
  this->mbr_0x120 = 0;
  this->mbr_0x11c = 0;
  this->mbr_0x124 = 0x3e800000;
  this->mbr_0x128 = 0xbe800000;
  this->mbr_0x12c = 0;
  this->mbr_0x130 = 1;
  return;
}



// Function at 004ffae0

undefined4 cls_0x5b0074::virt_meth_0x4ffae0()

{
  undefined4 *puVar1;
  float *pfVar2;
  dword *pdVar3;
  int iVar4;
  
  cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),2);
  puVar1 = (undefined4 *)cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  iVar4 = puVar1[0x28];
  *puVar1 = 0x2000;
  if (0 < iVar4) {
    pfVar2 = (float *)(puVar1[0x29] + 0x1c);
    pdVar3 = &this->mbr_0x10c;
    do {
      iVar4 = iVar4 + -1;
      pfVar2[-1] = (float)pdVar3[-4] + (float)this->mbr_0x11c;
      *pfVar2 = (float)*pdVar3 + (float)this->mbr_0x120;
      pfVar2 = pfVar2 + 8;
      pdVar3 = pdVar3 + 1;
    } while (iVar4 != 0);
  }
  cls_0x5a486c::meth_0x40a8f0((cls_0x5a486c *)this->mbr_0x8);
  return 1;
}



// Function at 004fff90

cls_0x5a85ac * cls_0x5b0074::virt_meth_0x4fff90(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a006b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x548);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x500020_005adca4;
    this_00[2].field_0x147 = 0;
    *(undefined4 *)&this_00[3].field_0x12d = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 005000f0

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x5000f0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500110

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00500120

cls_0x5a85ac * cls_0x5b0074::virt_meth_0x500120(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a00c3;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x1a0);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    local_4 = CONCAT31(local_4._1_3_,1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x5001c0_005adf18;
    (*(code *)PTR_virt_meth_0x471b50_005ae070)(0);
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x38 + 3) = 0xffffffff;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x3c + 3) = 0;
    ExceptionList = this_00;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 00500290

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x500290()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005002b0

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00500420

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x500420()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500440

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 005005b0

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x5005b0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005005d0

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00500730

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x500730()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500750

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00500760

cls_0x5a85ac * cls_0x5b0074::virt_meth_0x500760(undefined4 param_1)

{
  uint uVar1;
  TScreen__vftable_5a50e8 *pcVar2;
  cls_0x5a85ac *this_00;
  void *unaff_ESI;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0203;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x188);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    local_4 = CONCAT31(local_4._1_3_,1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x500800_005ae8e0;
    (*(code *)PTR_virt_meth_0x471b50_005aea38)(0);
    uVar1 = (this_00->TScreen).mbr_0x8;
    pcVar2 = (this_00->TScreen).vftptr_0x0;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x24 + 3) = 0;
    (*pcVar2->FUN_00472e90_64)(uVar1 | 0x80000);
    ExceptionList = unaff_ESI;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 005008d0

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x5008d0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005008f0

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00500a60

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x500a60()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500a80

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00500bf0

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x500bf0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500c10

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00500c20

cls_0x5a85ac * cls_0x5b0074::virt_meth_0x500c20(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  cls_0x5a85ac *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a02eb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x184);
  local_4 = 0;
  pcVar1 = (cls_0x5a85ac *)0x0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x500c80_005af01c;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 005010b0

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x5010b0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005010d0

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 005010e0

cls_0x5a47f0 * cls_0x5b0074::virt_meth_0x5010e0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a040e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x134);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x501180_005afe80;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00501180

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x501180()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005011a0

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 005011b0

cls_0x5a47f0 * cls_0x5b0074::virt_meth_0x5011b0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a043e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x134);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x501250_005afee4;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00501250

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x501250()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00501270

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00501280

cls_0x5a47f0 * cls_0x5b0074::virt_meth_0x501280()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a046e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x134);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x501320_005aff48;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00501320

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x501320()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00501340

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00501350

cls_0x5a47f0 * cls_0x5b0074::virt_meth_0x501350()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a049e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x134);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x5013f0_005affac;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 005013f0

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x5013f0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00501410

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00501420

cls_0x5a47f0 * cls_0x5b0074::virt_meth_0x501420()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a04ce;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x134);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x5014c0_005b0010;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 005014c0

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x5014c0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005014e0

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 005014f0

cls_0x5a47f0 * cls_0x5b0074::virt_meth_0x5014f0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a04fe;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x134);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b0074__vftable_5b0074_005b0074;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00501590

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x501590()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005015b0

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 005015c0

cls_0x5a85ac * cls_0x5b0074::virt_meth_0x5015c0(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0523;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x188);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    local_4 = CONCAT31(local_4._1_3_,1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x501650_005b00d8;
    (*(code *)PTR_virt_meth_0x471b50_005b0230)(0);
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x24 + 3) = 0;
    ExceptionList = this_00;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 00501720

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x501720()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00501740

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 005086f0

void cls_0x5b0074::virt_meth_0x5086f0()

{
  int iVar1;
  int iVar2;
  dword dVar3;
  dword *pdVar4;
  
  this->virt_meth_0x40dd60();
  if (*(int *)(this->mbr_0x4 + 0x18c) == 1) {
    iVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
    if (iVar1 < 0) goto LAB_00508767;
    iVar2 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548);
  }
  else {
    iVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
    if (iVar1 < 0) goto LAB_00508767;
    iVar2 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548);
  }
  if (iVar2 != 0) {
    cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar1,0x7f,1);
  }
LAB_00508767:
  dVar3 = 1;
  pdVar4 = &this->mbr_0x124;
  do {
    *pdVar4 = dVar3;
    dVar3 = dVar3 + 3;
    pdVar4 = pdVar4 + 1;
  } while ((int)dVar3 < 0x2e);
  this->mbr_0x11c = 0;
  this->mbr_0x120 = 0;
  return;
}



// Function at 00508f60

cls_0x5a85ac * cls_0x5b0074::virt_meth_0x508f60(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0573;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x188);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    local_4 = CONCAT31(local_4._1_3_,1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x508ff0_005b033c;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x24 + 3) = 0;
    (*(code *)PTR_virt_meth_0x471b50_005b0494)(0);
    ExceptionList = this_00;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 005090c0

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x5090c0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005090e0

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00509290

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x509290()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005092b0

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00509640

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00509910

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x509910()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509930

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00509ad0

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x509ad0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509af0

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00509b00

cls_0x5a85ac * cls_0x5b0074::virt_meth_0x509b00(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a07c3;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x184);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    local_4 = CONCAT31(local_4._1_3_,1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x509b90_005b1248;
    (*(code *)PTR_virt_meth_0x471b50_005b13a0)(0);
    ExceptionList = this_00;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 00509c60

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x509c60()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509c80

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00509c90

cls_0x5a85ac * cls_0x5b0074::virt_meth_0x509c90(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a080b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x194);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x509d20_005b14ac;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x24 + 3) = 0;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x28 + 3) = 0;
    (this_00->TScreen).mbr_0x8 = 0x4c009;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 0050a2c0

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x50a2c0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050a2e0

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 0050a2f0

cls_0x5a85ac * cls_0x5b0074::virt_meth_0x50a2f0(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0983;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x19c);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    local_4 = CONCAT31(local_4._1_3_,1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x50a390_005b1ff0;
    (*(code *)PTR_virt_meth_0x471b50_005b2148)(0);
    *(undefined4 *)&this_00[1].TScreen.mbr_0x37 = 0xffffffff;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x38 + 3) = 0;
    ExceptionList = this_00;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 0050a460

cls_0x5b0074 * cls_0x5b0074::virt_meth_0x50a460()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0074(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050a480

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 0050de90

TScreen * cls_0x5b0074::virt_meth_0x50de90(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0a1b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xf8);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    *(undefined4 *)&this_00->field_0xdc = 0;
    this_00->mbr_0xe0 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    *(undefined4 *)&this_00->field_0xe4 = 0;
    *(undefined4 *)&this_00->field_0xe8 = 0;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x50df50_005b2258;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 0050e360

TScreen * cls_0x5b0074::virt_meth_0x50e360(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0aab;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xf8);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    *(undefined4 *)&this_00->field_0xdc = 0;
    this_00->mbr_0xe0 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    *(undefined4 *)&this_00->field_0xe4 = 0;
    *(undefined4 *)&this_00->field_0xe8 = 0;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x50e400_005b27cc;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00516820

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00523900

cls_0x5b0074 * __thiscall cls_0x5b0074::~cls_0x5b0074(cls_0x5b0074 *this)

{
  cls_0x5b0074 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059c694;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = (cls_0x5b0074__vftable_5b0074 *)&PTR_virt_meth_0x410ca0_005a370c;
  uStack_4 = 2;
  this->virt_meth_0x40de10();
  uStack_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  uStack_4 = (uint)uStack_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  uStack_4 = 0xffffffff;
  pcVar1 = (cls_0x5b0074 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



