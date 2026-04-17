// Decompiled methods and structure for class: cls_0x5aaf28

/*
/OOAnalyzer/cls_0x5aaf28
pack(disabled)
Structure cls_0x5aaf28 {
   0   cls_0x5aaf28::vftable_5aaf28 *   4   vftptr_0x0   "pointer to cls_0x5aaf28::vftable_5aaf28"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   54   byte   1   mbr_0x36   "Unsigned Byte (db)"
   176   dword   4   mbr_0xb0   "Unsigned Double-Word (ddw, 4-bytes)"
   228   dword   4   mbr_0xe4   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   280   dword   4   mbr_0x118   "Unsigned Double-Word (ddw, 4-bytes)"
   292   dword   4   mbr_0x124   "Unsigned Double-Word (ddw, 4-bytes)"
   296   dword   4   mbr_0x128   "Unsigned Double-Word (ddw, 4-bytes)"
   332   dword   4   mbr_0x14c   "Unsigned Double-Word (ddw, 4-bytes)"
   336   dword   4   mbr_0x150   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 340 Alignment: 1

*/

// Function at 004eb130

undefined4 __thiscall OOAnalyzer::cls_0x5aaf28::virt_meth_0x4eb130(cls_0x5aaf28 *this)

{
  cls_0x411eb0::meth_0x4178e0((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),8);
  (**(code **)(this->mbr_0xfc + 0xc))(0,0);
  (**(code **)(this->mbr_0x124 + 0xc))(0,0);
  cls_0x411eb0::meth_0x417b00((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  return 1;
}



// Function at 004f6fd0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5aaf28::virt_meth_0x4f6fd0(cls_0x5aaf28 *this)

{
  cls_0x5a47f0 *this_00;
  dword dVar1;
  undefined4 *puVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f564;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x154);
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
    dVar1 = FUN_00482fb0(0x6720);
    this_00[4].mbr_0x10 = dVar1;
    puVar2 = (undefined4 *)(dVar1 + 0x54);
    iVar3 = 300;
    do {
      *puVar2 = 0;
      puVar2 = puVar2 + 0x16;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    this_00[4].mbr_0x28 = 300;
    local_4 = CONCAT31(local_4._1_3_,4);
    *(cls_0x5a96d8__vftable_5a96d8 **)&this_00[4].mbr_0x34 = &cls_0x5a96d8__vftable_5a96d8_005a96d8;
    dVar1 = FUN_00482fb0(0x6720);
    this_00[4].mbr_0x38 = dVar1;
    puVar2 = (undefined4 *)(dVar1 + 0x54);
    iVar3 = 300;
    do {
      *puVar2 = 0;
      puVar2 = puVar2 + 0x16;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    this_00[5].mbr_0x14 = 300;
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5aaf28__vftable_5aaf28_005aaf28;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f7200

cls_0x5a50e8 * __thiscall
OOAnalyzer::cls_0x5aaf28::virt_meth_0x4f7200(cls_0x5aaf28 *this,int param_1)

{
  uint uVar1;
  uint uVar2;
  cls_0x5a50e8 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f5bb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a50e8 *)FUN_00482fb0(0x18c);
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
    this_00->vftptr_0x0 = (cls_0x5a50e8__vftable_5a50e8 *)&PTR_virt_meth_0x4f7d20_005aaf8c;
    *(undefined4 *)&this_00[1].field_0x70 = 0;
    *(undefined4 *)&this_00->field_0xdc = 3;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a50e8 *)0x0;
}



// Function at 004f81c0

cls_0x5aaf28 * __thiscall OOAnalyzer::cls_0x5aaf28::virt_meth_0x4f81c0(cls_0x5aaf28 *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f81f0

cls_0x5aaf28 * __thiscall OOAnalyzer::cls_0x5aaf28::virt_meth_0x4f81f0(cls_0x5aaf28 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5aaf28(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8210

cls_0x5aaf28 * __thiscall OOAnalyzer::cls_0x5aaf28::~cls_0x5aaf28(cls_0x5aaf28 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f934;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5aaf28__vftable_5aaf28_005aaf28;
  local_4 = 2;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4._0_1_ = 1;
  this->mbr_0x124 = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0(this->mbr_0x128);
  local_4 = (uint)local_4._1_3_ << 8;
  this->mbr_0xfc = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0(this->mbr_0x100);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5aaf28 *)pcVar1;
}



