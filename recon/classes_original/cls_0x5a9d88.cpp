// Decompiled methods and structure for class: cls_0x5a9d88

/*
/OOAnalyzer/cls_0x5a9d88
pack(disabled)
Structure cls_0x5a9d88 {
   0   cls_0x5a9d88::vftable_5a9d88 *   4   vftptr_0x0   "pointer to cls_0x5a9d88::vftable_5a9d88"
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
}
Length: 264 Alignment: 1

*/

// Function at 004e4d50

void __thiscall OOAnalyzer::cls_0x5a9d88::virt_meth_0x4e4d50(cls_0x5a9d88 *this)

{
  cls_0x5a7e38::virt_meth_0x40dd60((cls_0x5a7e38 *)this);
  this->mbr_0x104 = 0x42200000;
  this->mbr_0x100 = 0;
  this->mbr_0xfc = 0;
  return;
}



// Function at 004e4d90

undefined4 __thiscall OOAnalyzer::cls_0x5a9d88::virt_meth_0x4e4d90(cls_0x5a9d88 *this)

{
  undefined4 *puVar1;
  dword dVar2;
  undefined4 *puVar3;
  
  cls_0x411eb0::meth_0x4178e0((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),2);
  puVar3 = (undefined4 *)cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  FUN_0040c960();
  puVar3[0x12] = 0x40000000;
  puVar3[0x11] = 0x40000000;
  puVar3[0x10] = 0x40000000;
  dVar2 = this->mbr_0xfc;
  puVar1 = puVar3 + 0x16;
  *puVar3 = 0x100;
  puVar3[4] = dVar2;
  dVar2 = this->mbr_0x104;
  puVar3[5] = this->mbr_0x100;
  puVar3[6] = dVar2;
  FUN_0043a9f0(puVar1);
  FUN_0043b180(puVar1,0xbf860a92);
  FUN_0043b230(puVar1,0xbf490fdb);
  FUN_0043b130(puVar1,puVar3 + 0x10);
  FUN_0043b090(puVar1,puVar3 + 4);
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
  cls_0x411eb0::meth_0x417b00((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  return 1;
}



// Function at 004f5d70

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5a9d88::virt_meth_0x4f5d70(cls_0x5a9d88 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f13e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x100);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x4f7730_005a9dec;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f76c0

cls_0x5a9d88 * __thiscall OOAnalyzer::cls_0x5a9d88::virt_meth_0x4f76c0(cls_0x5a9d88 *this)

{
  byte in_stack_00000004;
  
  cls_0x4f76e0::~cls_0x4f76e0((cls_0x4f76e0 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7750

cls_0x5a9d88 * __thiscall OOAnalyzer::cls_0x5a9d88::~cls_0x5a9d88(cls_0x5a9d88 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f6e8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a9d88__vftable_5a9d88 *)&PTR_virt_meth_0x4f7730_005a9dec;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5a9d88 *)pcVar1;
}



