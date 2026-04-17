// Decompiled methods and structure for class: cls_0x5ad4e4

/*
/OOAnalyzer/cls_0x5ad4e4
pack(disabled)
Structure cls_0x5ad4e4 {
   0   cls_0x5ad4e4::vftable_5ad4e4 *   4   vftptr_0x0   "pointer to cls_0x5ad4e4::vftable_5ad4e4"
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
}
Length: 272 Alignment: 1

*/

// Function at 004f3410

void __thiscall OOAnalyzer::cls_0x5ad4e4::virt_meth_0x4f3410(cls_0x5ad4e4 *this)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  undefined4 uVar4;
  undefined4 *puVar5;
  int iVar6;
  
  cls_0x5a7e38::virt_meth_0x40dd60((cls_0x5a7e38 *)this);
  uVar4 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  cls_0x5a486c::meth_0x40a0c0((cls_0x5a486c *)this->mbr_0x8,uVar4);
  dVar1 = this->mbr_0x4;
  this->mbr_0x100 = 0x19;
  dVar2 = *(dword *)(dVar1 + 0x14);
  dVar3 = *(dword *)(dVar1 + 0x18);
  this->mbr_0x104 = *(dword *)(dVar1 + 0x10);
  this->mbr_0x108 = dVar2;
  this->mbr_0x10c = dVar3;
  puVar5 = FUN_00482fb0(1000);
  this->mbr_0xfc = (dword)puVar5;
  iVar6 = 0;
  if (0 < (int)this->mbr_0x100) {
    do {
      (*this->vftptr_0x0->virt_meth_0x4f32c0_96)(this);
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)this->mbr_0x100);
  }
  iVar6 = 0;
  if (0 < (int)this->mbr_0x100) {
    do {
      (*this->vftptr_0x0->virt_meth_0x4f3380_100)(this);
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)this->mbr_0x100);
  }
  return;
}



// Function at 004f34b0

void __thiscall OOAnalyzer::cls_0x5ad4e4::virt_meth_0x4f34b0(cls_0x5ad4e4 *this)

{
  cls_0x5a7e38::virt_meth_0x40e2e0((cls_0x5a7e38 *)this);
  (**(code **)(*(int *)this->mbr_0x4 + 0x158))(0);
  (*this->vftptr_0x0->virt_meth_0x4f3380_100)(this);
  return;
}



// Function at 004f9c00

cls_0x5ad4e4 * __thiscall OOAnalyzer::cls_0x5ad4e4::virt_meth_0x4f9c00(cls_0x5ad4e4 *this)

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9c30

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5ad4e4::virt_meth_0x4f9c30(cls_0x5ad4e4 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fe9e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x110);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ad4e4__vftable_5ad4e4_005ad4e4;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f9cd0

cls_0x5ad4e4 * __thiscall OOAnalyzer::cls_0x5ad4e4::virt_meth_0x4f9cd0(cls_0x5ad4e4 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5ad4e4(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9cf0

cls_0x5ad4e4 * __thiscall OOAnalyzer::cls_0x5ad4e4::~cls_0x5ad4e4(cls_0x5ad4e4 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059feb8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ad4e4__vftable_5ad4e4_005ad4e4;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ad4e4 *)pcVar1;
}



// Function at 004f9d40

TScreen * __thiscall
OOAnalyzer::cls_0x5ad4e4::virt_meth_0x4f9d40(cls_0x5ad4e4 *this,int param_1)

{
  uint uVar1;
  uint uVar2;
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fedb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x184);
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
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4f9df0_005ad554;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



