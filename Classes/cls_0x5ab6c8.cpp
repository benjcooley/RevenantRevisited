// Decompiled methods and structure for class: cls_0x5ab6c8

/*
/OOAnalyzer/cls_0x5ab6c8
pack(disabled)
Structure cls_0x5ab6c8 {
   0   cls_0x5ab6c8::vftable_5ab6c8 *   4   vftptr_0x0   "pointer to cls_0x5ab6c8::vftable_5ab6c8"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
   104   dword   4   mbr_0x68   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   440   dword   4   mbr_0x1b8   "Unsigned Double-Word (ddw, 4-bytes)"
   444   dword   4   mbr_0x1bc   "Unsigned Double-Word (ddw, 4-bytes)"
   448   dword   4   mbr_0x1c0   "Unsigned Double-Word (ddw, 4-bytes)"
   452   dword   4   mbr_0x1c4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 456 Alignment: 1

*/

// Function at 004f8080

cls_0x5ab6c8 * __thiscall OOAnalyzer::cls_0x5ab6c8::virt_meth_0x4f8080(cls_0x5ab6c8 *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f80b0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5ab6c8::virt_meth_0x4f80b0(cls_0x5ab6c8 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f8de;
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
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ab6c8__vftable_5ab6c8_005ab6c8;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f8150

cls_0x5ab6c8 * __thiscall OOAnalyzer::cls_0x5ab6c8::virt_meth_0x4f8150(cls_0x5ab6c8 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5ab6c8(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8170

cls_0x5ab6c8 * __thiscall OOAnalyzer::cls_0x5ab6c8::~cls_0x5ab6c8(cls_0x5ab6c8 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f8f8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ab6c8__vftable_5ab6c8_005ab6c8;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ab6c8 *)pcVar1;
}



// Function at 004f82a0

cls_0x5a50e8 * __thiscall
OOAnalyzer::cls_0x5ab6c8::virt_meth_0x4f82a0(cls_0x5ab6c8 *this,int param_1)

{
  uint uVar1;
  uint uVar2;
  cls_0x5a50e8 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f94b;
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
    this_00->vftptr_0x0 = (cls_0x5a50e8__vftable_5a50e8 *)&PTR_virt_meth_0x4f8350_005ab72c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a50e8 *)0x0;
}



