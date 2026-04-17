// Decompiled methods and structure for class: cls_0x5a9900

/*
/OOAnalyzer/cls_0x5a9900
pack(disabled)
Structure cls_0x5a9900 {
   0   cls_0x5a9900::vftable_5a9900 *   4   vftptr_0x0   "pointer to cls_0x5a9900::vftable_5a9900"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   280   dword   4   mbr_0x118   "Unsigned Double-Word (ddw, 4-bytes)"
   292   dword   4   mbr_0x124   "Unsigned Double-Word (ddw, 4-bytes)"
   296   dword   4   mbr_0x128   "Unsigned Double-Word (ddw, 4-bytes)"
   300   dword   4   mbr_0x12c   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 396 Alignment: 1

*/

// Function at 004e3960

void __thiscall OOAnalyzer::cls_0x5a9900::virt_meth_0x4e3960(cls_0x5a9900 *this,int param_1)

{
  dword dVar1;
  dword in_stack_00000008;
  
  SoftwareRenderer::virt_meth_0x4df0f0((SoftwareRenderer *)this,param_1);
  if (param_1 == 1) {
    if (this->mbr_0x184 != in_stack_00000008) {
      return;
    }
  }
  else {
    if (param_1 != 2) {
      return;
    }
    dVar1 = this->mbr_0x184;
    if (dVar1 == 0) {
      return;
    }
    if (*(dword *)(dVar1 + 0x44) != in_stack_00000008) {
      return;
    }
    if ((*(uint *)(dVar1 + 8) & 0x80000) != 0) {
      return;
    }
  }
  (*this->vftptr_0x0->virt_meth_0x410bd0_64)(this);
  this->mbr_0x184 = 0;
  return;
}



// Function at 004e3f40

undefined4 __thiscall OOAnalyzer::cls_0x5a9900::virt_meth_0x4e3f40(cls_0x5a9900 *this)

{
  cls_0x411eb0::meth_0x4178e0((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),8);
  (**(code **)(this->mbr_0xfc + 0xc))(0,0);
  cls_0x411eb0::meth_0x417b00((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  return 1;
}



// Function at 004f57c0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5a9900::virt_meth_0x4f57c0(cls_0x5a9900 *this)

{
  cls_0x5a47f0 *this_00;
  undefined4 *puVar1;
  int iVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059efd6;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x130);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4._0_1_ = 2;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x410ca0_005a370c;
    local_4 = CONCAT31(local_4._1_3_,3);
    this_00[4].mbr_0xc = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
    puVar1 = FUN_00482fb0(0x2260);
    this_00[4].mbr_0x10 = (dword)puVar1;
    puVar1 = puVar1 + 0x15;
    iVar2 = 100;
    do {
      *puVar1 = 0;
      puVar1 = puVar1 + 0x16;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    this_00[4].mbr_0x28 = 100;
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5a9900__vftable_5a9900_005a9900;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f58a0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5a9900::virt_meth_0x4f58a0(cls_0x5a9900 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059effe;
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
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x4f72f0_005a9964;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f7c60

cls_0x5a9900 * __thiscall OOAnalyzer::cls_0x5a9900::virt_meth_0x4f7c60(cls_0x5a9900 *this)

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7c90

cls_0x5a9900 * __thiscall OOAnalyzer::cls_0x5a9900::virt_meth_0x4f7c90(cls_0x5a9900 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a9900(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7cb0

cls_0x5a9900 * __thiscall OOAnalyzer::cls_0x5a9900::~cls_0x5a9900(cls_0x5a9900 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  uint local_4;
  
  puStack_8 = &LAB_0059f806;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a9900__vftable_5a9900_005a9900;
  local_4 = 1;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = local_4 & 0xffffff00;
  this->mbr_0xfc = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0((LPCVOID)this->mbr_0x100);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5a9900 *)pcVar1;
}



