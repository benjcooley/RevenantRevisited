// Decompiled methods and structure for class: cls_0x5aa400

/*
/OOAnalyzer/cls_0x5aa400
pack(disabled)
Structure cls_0x5aa400 {
   0   cls_0x5aa400::vftable_5aa400 *   4   vftptr_0x0   "pointer to cls_0x5aa400::vftable_5aa400"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   264   dword   4   mbr_0x108   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   280   dword   4   mbr_0x118   "Unsigned Double-Word (ddw, 4-bytes)"
   316   dword   4   mbr_0x13c   "Unsigned Double-Word (ddw, 4-bytes)"
   320   dword   4   mbr_0x140   "Unsigned Double-Word (ddw, 4-bytes)"
   324   dword   4   mbr_0x144   "Unsigned Double-Word (ddw, 4-bytes)"
   344   dword   4   mbr_0x158   "Unsigned Double-Word (ddw, 4-bytes)"
   348   dword   4   mbr_0x15c   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 408 Alignment: 1

*/

// Function at 004e7160

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x5aa400::virt_meth_0x4e7160(cls_0x5aa400 *this)

{
  dword dVar1;
  float fVar2;
  dword *pdVar3;
  dword *pdVar4;
  int iVar5;
  
  cls_0x5a7e38::virt_meth_0x40dd60((cls_0x5a7e38 *)this);
  dVar1 = this->mbr_0x4;
  pdVar4 = &this->mbr_0x144;
  iVar5 = 5;
  *(undefined *)(dVar1 + 0x36) = 0xe0;
  *(undefined4 *)(dVar1 + 0xb0) = 0xffffffe0;
  pdVar3 = &this->mbr_0x108;
  do {
    pdVar3[-1] = 0x41a00000;
    *pdVar3 = 0x41a00000;
    *pdVar4 = 0;
    pdVar4 = pdVar4 + 1;
    pdVar3 = pdVar3 + 3;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  fVar2 = *(float *)&this->field_0x12c + _DAT_005a8d0c;
  this->mbr_0x10c = 0xc0a00000;
  this->mbr_0x118 = 0x40a00000;
  this->mbr_0x13c = 0;
  this->mbr_0x140 = 0;
  this->mbr_0x158 = 0x3e99999a;
  this->mbr_0x15c = 0x3dcccccd;
  *(float *)&this->field_0x12c = fVar2;
  *(float *)&this->field_0x138 = *(float *)&this->field_0x138 - _DAT_005a8d0c;
  return;
}



// Function at 004e7210

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x5aa400::virt_meth_0x4e7210(cls_0x5aa400 *this)

{
  dword dVar1;
  float fVar2;
  
  cls_0x5a7e38::virt_meth_0x40e2e0((cls_0x5a7e38 *)this);
  (**(code **)(*(int *)this->mbr_0x4 + 0x158))(0);
  dVar1 = this->mbr_0x13c;
  *(float *)&this->field_0xfc = *(float *)&this->field_0xfc + _DAT_005a37e8;
  if ((int)dVar1 < 0x18) {
    if (0xb < (int)dVar1) {
      this->mbr_0x10c = (dword)((float)this->mbr_0x10c + _DAT_005a34e4);
      this->mbr_0x118 = (dword)((float)this->mbr_0x118 - _DAT_005a34e4);
    }
    fVar2 = *(float *)&this->field_0x12c - _DAT_005a36d8;
    this->mbr_0x13c = dVar1 + 1;
    *(float *)&this->field_0x12c = fVar2;
    *(float *)&this->field_0x138 = *(float *)&this->field_0x138 + _DAT_005a36d8;
  }
  this->mbr_0x140 = this->mbr_0x140 + 1;
  return;
}



// Function at 004f62e0

cls_0x5aa400 * __thiscall OOAnalyzer::cls_0x5aa400::virt_meth_0x4f62e0(cls_0x5aa400 *this)

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f6310

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5aa400::virt_meth_0x4f6310(cls_0x5aa400 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f28e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x160);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5aa400__vftable_5aa400_005aa400;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f63b0

cls_0x5aa400 * __thiscall OOAnalyzer::cls_0x5aa400::virt_meth_0x4f63b0(cls_0x5aa400 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5aa400(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f63d0

cls_0x5aa400 * __thiscall OOAnalyzer::cls_0x5aa400::~cls_0x5aa400(cls_0x5aa400 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f2a8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5aa400__vftable_5aa400_005aa400;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5aa400 *)pcVar1;
}



