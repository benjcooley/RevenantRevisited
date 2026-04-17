// Decompiled methods and structure for class: cls_0x5ad9d4

/*
/OOAnalyzer/cls_0x5ad9d4
pack(disabled)
Structure cls_0x5ad9d4 {
   0   cls_0x5ad9d4::vftable_5ad9d4 *   4   vftptr_0x0   "pointer to cls_0x5ad9d4::vftable_5ad9d4"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   292   dword   4   mbr_0x124   "Unsigned Double-Word (ddw, 4-bytes)"
   296   dword   4   mbr_0x128   "Unsigned Double-Word (ddw, 4-bytes)"
   332   dword   4   mbr_0x14c   "Unsigned Double-Word (ddw, 4-bytes)"
   336   dword   4   mbr_0x150   "Unsigned Double-Word (ddw, 4-bytes)"
   372   dword   4   mbr_0x174   "Unsigned Double-Word (ddw, 4-bytes)"
   376   dword   4   mbr_0x178   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 380 Alignment: 1

*/

// Function at 004f3f10

void __thiscall OOAnalyzer::cls_0x5ad9d4::virt_meth_0x4f3f10(cls_0x5ad9d4 *this)

{
  int iVar1;
  int iVar2;
  
  iVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
  if (-1 < iVar1) {
    iVar2 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548);
    if (iVar2 != 0) {
      cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar1,0x7f,1);
    }
  }
  return;
}



// Function at 004f4440

undefined4 __thiscall OOAnalyzer::cls_0x5ad9d4::virt_meth_0x4f4440(cls_0x5ad9d4 *this)

{
  cls_0x411eb0::meth_0x4178e0((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),2);
  (**(code **)(this->mbr_0xfc + 0xc))(1,0);
  (**(code **)(this->mbr_0x124 + 0xc))(1,0);
  (**(code **)(this->mbr_0x14c + 0xc))(1,0);
  cls_0x411eb0::meth_0x417b00((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  return 1;
}



// Function at 004fa040

cls_0x5ad9d4 * __thiscall OOAnalyzer::cls_0x5ad9d4::virt_meth_0x4fa040(cls_0x5ad9d4 *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004fa070

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5ad9d4::virt_meth_0x4fa070(cls_0x5ad9d4 *this)

{
  cls_0x5a47f0 *this_00;
  dword dVar1;
  undefined4 *puVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ffa2;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x17c);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4._0_1_ = 2;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x410ca0_005a370c;
    local_4._0_1_ = 3;
    this_00[4].mbr_0xc = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
    dVar1 = FUN_00482fb0(0xa50);
    this_00[4].mbr_0x10 = dVar1;
    puVar2 = (undefined4 *)(dVar1 + 0x54);
    iVar3 = 0x1e;
    do {
      *puVar2 = 0;
      puVar2 = puVar2 + 0x16;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    this_00[4].mbr_0x28 = 0x1e;
    local_4._0_1_ = 4;
    *(cls_0x5a96d8__vftable_5a96d8 **)&this_00[4].mbr_0x34 = &cls_0x5a96d8__vftable_5a96d8_005a96d8;
    dVar1 = FUN_00482fb0(0xa50);
    this_00[4].mbr_0x38 = dVar1;
    puVar2 = (undefined4 *)(dVar1 + 0x54);
    iVar3 = 0x1e;
    do {
      *puVar2 = 0;
      puVar2 = puVar2 + 0x16;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    this_00[5].mbr_0x14 = 0x1e;
    local_4 = CONCAT31(local_4._1_3_,5);
    this_00[5].mbr_0x20 = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
    dVar1 = FUN_00482fb0(0xa50);
    this_00[5].mbr_0x24 = dVar1;
    puVar2 = (undefined4 *)(dVar1 + 0x54);
    iVar3 = 0x1e;
    do {
      *puVar2 = 0;
      puVar2 = puVar2 + 0x16;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    this_00[6].vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)0x1e;
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ad9d4__vftable_5ad9d4_005ad9d4;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004fa1d0

cls_0x5ad9d4 * __thiscall OOAnalyzer::cls_0x5ad9d4::virt_meth_0x4fa1d0(cls_0x5ad9d4 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5ad9d4(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004fa1f0

cls_0x5ad9d4 * __thiscall OOAnalyzer::cls_0x5ad9d4::~cls_0x5ad9d4(cls_0x5ad9d4 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059ffe2;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ad9d4__vftable_5ad9d4_005ad9d4;
  local_4 = 3;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4._0_1_ = 2;
  this->mbr_0x14c = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0(this->mbr_0x150);
  local_4._0_1_ = 1;
  this->mbr_0x124 = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0(this->mbr_0x128);
  local_4 = (uint)local_4._1_3_ << 8;
  this->mbr_0xfc = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0(this->mbr_0x100);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ad9d4 *)pcVar1;
}



