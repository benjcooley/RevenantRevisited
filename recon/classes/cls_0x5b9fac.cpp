// Decompiled methods and structure for class: cls_0x5b9fac

/*
/OOAnalyzer/cls_0x5b9fac
pack(disabled)
Structure cls_0x5b9fac {
   0   cls_0x5b9fac::vftable_5b9fac *   4   vftptr_0x0   "pointer to cls_0x5b9fac::vftable_5b9fac"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
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
   284   dword   4   mbr_0x11c   "Unsigned Double-Word (ddw, 4-bytes)"
   288   dword   4   mbr_0x120   "Unsigned Double-Word (ddw, 4-bytes)"
   292   dword   4   mbr_0x124   "Unsigned Double-Word (ddw, 4-bytes)"
   296   dword   4   mbr_0x128   "Unsigned Double-Word (ddw, 4-bytes)"
   372   dword   4   mbr_0x174   "Unsigned Double-Word (ddw, 4-bytes)"
   452   dword   4   mbr_0x1c4   "Unsigned Double-Word (ddw, 4-bytes)"
   492   dword   4   mbr_0x1ec   "Unsigned Double-Word (ddw, 4-bytes)"
   1488   dword   4   mbr_0x5d0   "Unsigned Double-Word (ddw, 4-bytes)"
   1688   dword   4   mbr_0x698   "Unsigned Double-Word (ddw, 4-bytes)"
   1888   dword   4   mbr_0x760   "Unsigned Double-Word (ddw, 4-bytes)"
   2088   dword   4   mbr_0x828   "Unsigned Double-Word (ddw, 4-bytes)"
   2288   dword   4   mbr_0x8f0   "Unsigned Double-Word (ddw, 4-bytes)"
   2488   dword   4   mbr_0x9b8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 2492 Alignment: 1

*/

// Function at 004e4120

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4e4120(cls_0x5b9fac *this)

{
  int iVar1;
  float fVar2;
  int iVar3;
  dword *pdVar4;
  dword *pdVar5;
  
  cls_0x5a7e38::virt_meth_0x40e2e0((cls_0x5a7e38 *)this);
  (**(code **)(*(int *)this->mbr_0x4 + 0x158))(0);
  pdVar4 = &this->mbr_0x174;
  pdVar5 = &this->mbr_0x104;
  iVar3 = 10;
  do {
    fVar2 = (float)pdVar4[0x14];
    pdVar4[0x14] = (int)fVar2 + 1;
    if (0 < (int)fVar2 + 1) {
      *pdVar5 = (dword)((float)pdVar4[10] + (float)*pdVar5);
      fVar2 = (float)*pdVar4 - _DAT_005a8a84;
      *pdVar4 = (dword)fVar2;
      if (fVar2 <= _DAT_005a3530) {
        iVar1 = FUN_00483300_RandomRange(0xffffffec,0x14);
        pdVar5[-2] = (dword)(float)iVar1;
        iVar1 = FUN_00483300_RandomRange(0xffffffec,0x14);
        pdVar5[-1] = (dword)(float)iVar1;
        *pdVar5 = 0;
        iVar1 = FUN_00483300_RandomRange(1,3);
        pdVar4[10] = (dword)((float)iVar1 * _DAT_005a356c);
        *pdVar4 = 0x40000000;
        fVar2 = (float)FUN_00483300_RandomRange(0xfffffffb,0);
        pdVar4[0x14] = (dword)fVar2;
      }
    }
    pdVar4 = pdVar4 + 1;
    pdVar5 = pdVar5 + 3;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return;
}



// Function at 004f5940

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f5940(cls_0x5b9fac *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f02e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x1f0);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x4f7400_005a99cc;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f59e0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f59e0(cls_0x5b9fac *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f05e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x1f0);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x4f74a0_005a9a34;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f5a80

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f5a80(cls_0x5b9fac *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f08e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x1f0);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x4f75e0_005a9a9c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f5b20

cls_0x5a50e8 * __thiscall
OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f5b20(cls_0x5b9fac *this,int param_1)

{
  uint uVar1;
  uint uVar2;
  cls_0x5a50e8 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f0b3;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a50e8 *)FUN_00482fb0(0x194);
  local_4 = 0;
  if (this_00 != (cls_0x5a50e8 *)0x0) {
    cls_0x5a50e8::cls_0x5a50e8(this_00);
    uVar1 = *(uint *)(param_1 + 4);
    uVar2 = this_00->mbr_0x8;
    *(undefined4 *)&this_00->field_0xe4 = 0;
    this_00[1].mbr_0x38 = 0;
    this_00->mbr_0x8 = uVar2 | uVar1 | 0x48001;
    this_00[1].mbr_0x1c = 0;
    this_00[1].mbr_0xc = 0;
    this_00[1].mbr_0xe = 0;
    *(undefined *)&this_00[1].mbr_0x48 = 0;
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->vftptr_0x0 = (cls_0x5a50e8__vftable_5a50e8 *)&PTR_virt_meth_0x4f5c00_005a9b04;
    (*(code *)PTR_virt_meth_0x471b50_005a9c5c)(0);
    *(undefined4 *)&this_00[1].field_0x7c = 0;
    *(undefined4 *)&this_00[1].field_0x78 = 0;
    *(undefined4 *)&this_00[1].field_0x70 = 0;
    *(undefined4 *)&this_00[1].field_0x74 = 0x14;
    ExceptionList = this_00;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a50e8 *)0x0;
}



// Function at 004f72c0

void __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f72c0(cls_0x5b9fac *this)

{
  virt_meth_0x4e4120(this);
  (**(code **)(*(int *)this->mbr_0x4 + 0x158))(0);
  return;
}



// Function at 004f72f0

cls_0x5b9fac * __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f72f0(cls_0x5b9fac *this)

{
  byte in_stack_00000004;
  
  cls_0x4f7310::~cls_0x4f7310((cls_0x4f7310 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7360

cls_0x5b9fac * __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f7360(cls_0x5b9fac *this)

{
  byte in_stack_00000004;
  
  cls_0x4f7380::~cls_0x4f7380((cls_0x4f7380 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f73d0

void __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f73d0(cls_0x5b9fac *this)

{
  virt_meth_0x4e4120(this);
  (**(code **)(*(int *)this->mbr_0x4 + 0x158))(0);
  return;
}



// Function at 004f7400

cls_0x5b9fac * __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f7400(cls_0x5b9fac *this)

{
  byte in_stack_00000004;
  
  cls_0x4f7420::~cls_0x4f7420((cls_0x4f7420 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7470

void __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f7470(cls_0x5b9fac *this)

{
  virt_meth_0x4e4120(this);
  (**(code **)(*(int *)this->mbr_0x4 + 0x158))(0);
  return;
}



// Function at 004f74a0

cls_0x5b9fac * __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f74a0(cls_0x5b9fac *this)

{
  byte in_stack_00000004;
  
  cls_0x4f74c0::~cls_0x4f74c0((cls_0x4f74c0 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7510

void __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f7510(cls_0x5b9fac *this)

{
  virt_meth_0x4e4120(this);
  (**(code **)(*(int *)this->mbr_0x4 + 0x158))(0);
  return;
}



// Function at 004f7530

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f7530(cls_0x5b9fac *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f66e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0xb48);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x4f7d50_005ab1f4;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f75e0

cls_0x5b9fac * __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f75e0(cls_0x5b9fac *this)

{
  byte in_stack_00000004;
  
  cls_0x4f7600::~cls_0x4f7600((cls_0x4f7600 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7d50

cls_0x5b9fac * __thiscall OOAnalyzer::cls_0x5b9fac::virt_meth_0x4f7d50(cls_0x5b9fac *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b9fac(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7d70

cls_0x5b9fac * __thiscall OOAnalyzer::cls_0x5b9fac::~cls_0x5b9fac(cls_0x5b9fac *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f818;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5b9fac__vftable_5b9fac *)&PTR_virt_meth_0x4f7d50_005ab1f4;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b9fac *)pcVar1;
}



