// Decompiled methods and structure for class: cls_0x5a9f2c

/*
/OOAnalyzer/cls_0x5a9f2c
pack(disabled)
Structure cls_0x5a9f2c {
   0   cls_0x5a9f2c::vftable_5a9f2c *   4   vftptr_0x0   "pointer to cls_0x5a9f2c::vftable_5a9f2c"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   264   dword   4   mbr_0x108   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
   276   dword   4   mbr_0x114   "Unsigned Double-Word (ddw, 4-bytes)"
   280   dword   4   mbr_0x118   "Unsigned Double-Word (ddw, 4-bytes)"
   292   dword   4   mbr_0x124   "Unsigned Double-Word (ddw, 4-bytes)"
   296   dword   4   mbr_0x128   "Unsigned Double-Word (ddw, 4-bytes)"
   300   dword   4   mbr_0x12c   "Unsigned Double-Word (ddw, 4-bytes)"
   308   dword   4   mbr_0x134   "Unsigned Double-Word (ddw, 4-bytes)"
   312   dword   4   mbr_0x138   "Unsigned Double-Word (ddw, 4-bytes)"
   316   dword   4   mbr_0x13c   "Unsigned Double-Word (ddw, 4-bytes)"
   320   dword   4   mbr_0x140   "Unsigned Double-Word (ddw, 4-bytes)"
   324   dword   4   mbr_0x144   "Unsigned Double-Word (ddw, 4-bytes)"
   328   dword   4   mbr_0x148   "Unsigned Double-Word (ddw, 4-bytes)"
   332   dword   4   mbr_0x14c   "Unsigned Double-Word (ddw, 4-bytes)"
   336   dword   4   mbr_0x150   "Unsigned Double-Word (ddw, 4-bytes)"
   340   dword   4   mbr_0x154   "Unsigned Double-Word (ddw, 4-bytes)"
   344   dword   4   mbr_0x158   "Unsigned Double-Word (ddw, 4-bytes)"
   348   dword   4   mbr_0x15c   "Unsigned Double-Word (ddw, 4-bytes)"
   352   dword   4   mbr_0x160   "Unsigned Double-Word (ddw, 4-bytes)"
   356   dword   4   mbr_0x164   "Unsigned Double-Word (ddw, 4-bytes)"
   360   dword   4   mbr_0x168   "Unsigned Double-Word (ddw, 4-bytes)"
   412   dword   4   mbr_0x19c   "Unsigned Double-Word (ddw, 4-bytes)"
   416   dword   4   mbr_0x1a0   "Unsigned Double-Word (ddw, 4-bytes)"
   420   dword   4   mbr_0x1a4   "Unsigned Double-Word (ddw, 4-bytes)"
   424   dword   4   mbr_0x1a8   "Unsigned Double-Word (ddw, 4-bytes)"
   428   dword   4   mbr_0x1ac   "Unsigned Double-Word (ddw, 4-bytes)"
   432   dword   4   mbr_0x1b0   "Unsigned Double-Word (ddw, 4-bytes)"
   436   dword   4   mbr_0x1b4   "Unsigned Double-Word (ddw, 4-bytes)"
   440   dword   4   mbr_0x1b8   "Unsigned Double-Word (ddw, 4-bytes)"
   444   dword   4   mbr_0x1bc   "Unsigned Double-Word (ddw, 4-bytes)"
   448   dword   4   mbr_0x1c0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 452 Alignment: 1

*/

// Function at 004e53c0

void __thiscall OOAnalyzer::cls_0x5a9f2c::virt_meth_0x4e53c0(cls_0x5a9f2c *this)

{
  int iVar1;
  dword *pdVar2;
  
  cls_0x5a7e38::virt_meth_0x40dd60((cls_0x5a7e38 *)this);
  pdVar2 = &this->mbr_0xfc;
  for (iVar1 = 0x2c; iVar1 != 0; iVar1 = iVar1 + -1) {
    *pdVar2 = 0;
    pdVar2 = pdVar2 + 1;
  }
  this->mbr_0x1b0 = 0;
  this->mbr_0x1ac = 0;
  this->mbr_0x1b4 = 0;
  this->mbr_0x1b8 = 0;
  this->mbr_0x1bc = 0;
  this->mbr_0x1c0 = 0;
  return;
}



// Function at 004e5400

void __thiscall OOAnalyzer::cls_0x5a9f2c::virt_meth_0x4e5400(cls_0x5a9f2c *this)

{
  if ((LPCVOID)this->mbr_0x1b0 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x1b0);
  }
  if ((LPCVOID)this->mbr_0x1ac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x1ac);
  }
  if ((LPCVOID)this->mbr_0x1b4 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x1b4);
  }
  if ((LPCVOID)this->mbr_0x1b8 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x1b8);
  }
  if ((LPCVOID)this->mbr_0x1bc != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x1bc);
  }
  if ((LPCVOID)this->mbr_0x1c0 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x1c0);
  }
  this->mbr_0x1b0 = 0;
  this->mbr_0x1ac = 0;
  this->mbr_0x1b4 = 0;
  this->mbr_0x1b8 = 0;
  this->mbr_0x1bc = 0;
  this->mbr_0x1c0 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  return;
}



// Function at 004e5fc0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x5a9f2c::virt_meth_0x4e5fc0(cls_0x5a9f2c *this)

{
  dword dVar1;
  float fVar2;
  dword *pdVar3;
  int iVar4;
  
  cls_0x5a7e38::virt_meth_0x40dd60((cls_0x5a7e38 *)this);
  dVar1 = this->mbr_0x4;
  iVar4 = 5;
  *(undefined *)(dVar1 + 0x36) = 0xe0;
  *(undefined4 *)(dVar1 + 0xb0) = 0xffffffe0;
  pdVar3 = &this->mbr_0x108;
  do {
    pdVar3[-1] = 0x41a00000;
    *pdVar3 = 0x42340000;
    pdVar3 = pdVar3 + 3;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  fVar2 = (float)this->mbr_0x12c + _DAT_005a8d0c;
  this->mbr_0x10c = 0xc0a00000;
  this->mbr_0x13c = 0;
  this->mbr_0x140 = 0;
  this->mbr_0x118 = 0x40a00000;
  this->mbr_0x12c = (dword)fVar2;
  fVar2 = (float)this->mbr_0x138 - _DAT_005a8d0c;
  this->mbr_0x148 = 0x3f800000;
  this->mbr_0x144 = 0x3f800000;
  this->mbr_0x14c = 0x3e99999a;
  this->mbr_0x150 = 0x3dcccccd;
  this->mbr_0x138 = (dword)fVar2;
  return;
}



// Function at 004e6070

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x5a9f2c::virt_meth_0x4e6070(cls_0x5a9f2c *this)

{
  float fVar1;
  dword *pdVar2;
  dword dVar3;
  int iVar4;
  
  cls_0x5a7e38::virt_meth_0x40e2e0((cls_0x5a7e38 *)this);
  (**(code **)(*(int *)this->mbr_0x4 + 0x158))(0);
  dVar3 = this->mbr_0x13c;
  this->mbr_0xfc = (dword)((float)this->mbr_0xfc + _DAT_005a4848);
  if ((int)dVar3 < 0x32) {
    if (0x18 < (int)dVar3) {
      this->mbr_0x10c = (dword)((float)this->mbr_0x10c + _DAT_005a356c);
      this->mbr_0x118 = (dword)((float)this->mbr_0x118 - _DAT_005a356c);
    }
    fVar1 = (float)this->mbr_0x12c - _DAT_005a34e4;
    this->mbr_0x13c = dVar3 + 1;
    this->mbr_0x12c = (dword)fVar1;
    this->mbr_0x138 = (dword)((float)this->mbr_0x138 + _DAT_005a34e4);
  }
  dVar3 = this->mbr_0x140 + 1;
  this->mbr_0x140 = dVar3;
  if (200 < (int)dVar3) {
    this->mbr_0x13c = 0;
    this->mbr_0x140 = 0;
    pdVar2 = &this->mbr_0x108;
    iVar4 = 5;
    do {
      pdVar2[-1] = 0x41a00000;
      *pdVar2 = 0x42340000;
      pdVar2 = pdVar2 + 3;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    fVar1 = (float)this->mbr_0x12c + _DAT_005a8d0c;
    this->mbr_0x10c = 0xc0a00000;
    this->mbr_0x118 = 0x40a00000;
    this->mbr_0x13c = 0;
    this->mbr_0x140 = 0;
    this->mbr_0x148 = 0x3f800000;
    this->mbr_0x12c = (dword)fVar1;
    fVar1 = (float)this->mbr_0x138 - _DAT_005a8d0c;
    this->mbr_0x144 = 0x3f800000;
    this->mbr_0x14c = 0x3e99999a;
    this->mbr_0x150 = 0x3dcccccd;
    this->mbr_0x138 = (dword)fVar1;
  }
  return;
}



// Function at 004f5f50

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5a9f2c::virt_meth_0x4f5f50(cls_0x5a9f2c *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f1ce;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x154);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5a9f2c__vftable_5a9f2c_005a9f2c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f5ff0

cls_0x5a9f2c * __thiscall OOAnalyzer::cls_0x5a9f2c::virt_meth_0x4f5ff0(cls_0x5a9f2c *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a9f2c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f6010

cls_0x5a9f2c * __thiscall OOAnalyzer::cls_0x5a9f2c::~cls_0x5a9f2c(cls_0x5a9f2c *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f1e8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a9f2c__vftable_5a9f2c_005a9f2c;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5a9f2c *)pcVar1;
}



// Function at 004f7810

cls_0x5a9f2c * __thiscall OOAnalyzer::cls_0x5a9f2c::virt_meth_0x4f7810(cls_0x5a9f2c *this)

{
  byte in_stack_00000004;
  
  cls_0x4f7830::~cls_0x4f7830((cls_0x4f7830 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



