// Decompiled methods and structure for class: cls_0x5ad26c

/*
/OOAnalyzer/cls_0x5ad26c
pack(disabled)
Structure cls_0x5ad26c {
   0   cls_0x5ad26c::vftable_5ad26c *   4   vftptr_0x0   "pointer to cls_0x5ad26c::vftable_5ad26c"
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

// Function at 004f2e60

void __thiscall OOAnalyzer::cls_0x5ad26c::virt_meth_0x4f2e60(cls_0x5ad26c *this)

{
  dword dVar1;
  dword dVar2;
  undefined4 uVar3;
  dword dVar4;
  int iVar5;
  
  cls_0x5a7e38::virt_meth_0x40dd60((cls_0x5a7e38 *)this);
  uVar3 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  cls_0x5a486c::meth_0x40a0c0((cls_0x5a486c *)this->mbr_0x8,uVar3);
  dVar4 = this->mbr_0x4;
  this->mbr_0x100 = 100;
  dVar1 = *(dword *)(dVar4 + 0x14);
  dVar2 = *(dword *)(dVar4 + 0x18);
  this->mbr_0x104 = *(dword *)(dVar4 + 0x10);
  this->mbr_0x108 = dVar1;
  this->mbr_0x10c = dVar2;
  dVar4 = FUN_00482fb0(4000);
  this->mbr_0xfc = dVar4;
  iVar5 = 0;
  if (0 < (int)this->mbr_0x100) {
    do {
      (*this->vftptr_0x0->virt_meth_0x4f2cb0_96)(this);
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)this->mbr_0x100);
  }
  iVar5 = 0;
  if (0 < (int)this->mbr_0x100) {
    do {
      (*this->vftptr_0x0->virt_meth_0x4f2d70_100)(this);
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)this->mbr_0x100);
  }
  return;
}



// Function at 004f2f00

void __thiscall OOAnalyzer::cls_0x5ad26c::virt_meth_0x4f2f00(cls_0x5ad26c *this)

{
  cls_0x5a7e38::virt_meth_0x40e2e0((cls_0x5a7e38 *)this);
  (**(code **)(*(int *)this->mbr_0x4 + 0x158))(0);
  (*this->vftptr_0x0->virt_meth_0x4f2d70_100)(this);
  return;
}



// Function at 004f9a10

cls_0x5ad26c * __thiscall OOAnalyzer::cls_0x5ad26c::virt_meth_0x4f9a10(cls_0x5ad26c *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9a40

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5ad26c::virt_meth_0x4f9a40(cls_0x5ad26c *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fe2e;
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
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ad26c__vftable_5ad26c_005ad26c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f9ae0

cls_0x5ad26c * __thiscall OOAnalyzer::cls_0x5ad26c::virt_meth_0x4f9ae0(cls_0x5ad26c *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5ad26c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9b00

cls_0x5ad26c * __thiscall OOAnalyzer::cls_0x5ad26c::~cls_0x5ad26c(cls_0x5ad26c *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059fe48;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ad26c__vftable_5ad26c_005ad26c;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ad26c *)pcVar1;
}



