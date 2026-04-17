// Decompiled methods and structure for class: cls_0x5ac83c

/*
/OOAnalyzer/cls_0x5ac83c
pack(disabled)
Structure cls_0x5ac83c {
   0   cls_0x5ac83c::vftable_5ac83c *   4   vftptr_0x0   "pointer to cls_0x5ac83c::vftable_5ac83c"
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
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 400 Alignment: 1

*/

// Function at 004f13c0

undefined4 __thiscall OOAnalyzer::cls_0x5ac83c::virt_meth_0x4f13c0(cls_0x5ac83c *this)

{
  undefined4 *puVar1;
  dword dVar2;
  undefined4 *puVar3;
  
  cls_0x411eb0::meth_0x4178e0((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),2);
  if (this->mbr_0x114 == 0) {
    puVar3 = (undefined4 *)cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
    puVar1 = puVar3 + 0x16;
    *puVar3 = 0x100;
    FUN_0043a9f0(puVar1);
    puVar3[0x10] = 0x3e99999a;
    puVar3[0x11] = 0x3e99999a;
    puVar3[0x12] = 0x3e99999a;
    FUN_0043b130(puVar1,puVar3 + 0x10);
    dVar2 = this->mbr_0xfc;
    puVar3[5] = this->mbr_0x100;
    puVar3[4] = dVar2;
    puVar3[6] = this->mbr_0x104;
    FUN_0043b090(puVar1,puVar3 + 4);
    FUN_0040c960();
    this->mbr_0xec = 0;
    cls_0x5a486c::meth_0x40a8f0((cls_0x5a486c *)this->mbr_0x8);
    FUN_0040c9c0(&this->mbr_0x9c);
    if ((this->mbr_0x3c & 1) != 0) {
      FUN_0040ca20(&this->mbr_0x9c,1);
    }
    this->mbr_0xec = 1;
    if ((this->mbr_0x3c & 2) != 0) {
      cls_0x5a486c::meth_0x40ca90((cls_0x5a486c *)this->mbr_0x8,this->mbr_0x4,this->mbr_0xc);
    }
    this->mbr_0xec = 1;
  }
  cls_0x411eb0::meth_0x417b00((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  return 1;
}



// Function at 004f9010

cls_0x5ac83c * __thiscall OOAnalyzer::cls_0x5ac83c::virt_meth_0x4f9010(cls_0x5ac83c *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9040

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5ac83c::virt_meth_0x4f9040(cls_0x5ac83c *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fc1e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x128);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ac83c__vftable_5ac83c_005ac83c;
    this_00[4].mbr_0x2c = 0x40;
    this_00[4].mbr_0x30 = 0x80;
    this_00[4].mbr_0x34 = 0x30;
    this_00[4].mbr_0x35 = 0;
    *(undefined2 *)&this_00[4].field_0x36 = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f90f0

cls_0x5ac83c * __thiscall OOAnalyzer::cls_0x5ac83c::virt_meth_0x4f90f0(cls_0x5ac83c *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5ac83c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9110

cls_0x5ac83c * __thiscall OOAnalyzer::cls_0x5ac83c::~cls_0x5ac83c(cls_0x5ac83c *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059fc38;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ac83c__vftable_5ac83c_005ac83c;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ac83c *)pcVar1;
}



// Function at 004f9160

cls_0x5a50e8 * __thiscall
OOAnalyzer::cls_0x5ac83c::virt_meth_0x4f9160(cls_0x5ac83c *this,int param_1)

{
  uint uVar1;
  uint uVar2;
  cls_0x5a50e8 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fc5b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a50e8 *)FUN_00482fb0(0x184);
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
    this_00->vftptr_0x0 = (cls_0x5a50e8__vftable_5a50e8 *)&PTR_virt_meth_0x4f9210_005ac8a4;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a50e8 *)0x0;
}



