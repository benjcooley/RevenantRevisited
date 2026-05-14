// Decompiled methods and structure for class: cls_0x5b0a28

/*
/OOAnalyzer/cls_0x5b0a28
pack(disabled)
Structure cls_0x5b0a28 {
   0   cls_0x5b0a28::vftable_5b0a28 *   4   vftptr_0x0   "pointer to cls_0x5b0a28::vftable_5b0a28"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   264   dword   4   mbr_0x108   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
   356   dword   4   mbr_0x164   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   436   dword   4   mbr_0x1b4   "Unsigned Double-Word (ddw, 4-bytes)"
   516   dword   4   mbr_0x204   "Unsigned Double-Word (ddw, 4-bytes)"
   596   byte   1   mbr_0x254   "Unsigned Byte (db)"
   616   dword   4   mbr_0x268   "Unsigned Double-Word (ddw, 4-bytes)"
   636   byte   1   mbr_0x27c   "Unsigned Byte (db)"
   656   dword   4   mbr_0x290   "Unsigned Double-Word (ddw, 4-bytes)"
   736   dword   4   mbr_0x2e0   "Unsigned Double-Word (ddw, 4-bytes)"
   816   dword   4   mbr_0x330   "Unsigned Double-Word (ddw, 4-bytes)"
   996   dword   4   mbr_0x3e4   "Unsigned Double-Word (ddw, 4-bytes)"
   1196   dword   4   mbr_0x4ac   "Unsigned Double-Word (ddw, 4-bytes)"
   1296   dword   4   mbr_0x510   "Unsigned Double-Word (ddw, 4-bytes)"
   1396   dword   4   mbr_0x574   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 1400 Alignment: 1

*/

// Function at 00502b70

void __thiscall OOAnalyzer::cls_0x5b0a28::virt_meth_0x502b70(cls_0x5b0a28 *this)

{
  dword dVar1;
  
  cls_0x5b8e94_TObjectInstance::virt_meth_0x4de800((cls_0x5b8e94_TObjectInstance *)this);
  if (this->mbr_0x188 == 0) {
    dVar1 = this->mbr_0x184;
    this->mbr_0x184 = dVar1 + 1;
    if (dVar1 == 0) {
      this->mbr_0x188 = 0x3c;
    }
    else if (dVar1 == 1) {
      dVar1 = this->mbr_0xd8;
      if (dVar1 != 0) {
        FUN_004de250(*(undefined4 *)(dVar1 + 4),&this->mbr_0x10,300,
                     *(undefined4 *)(*(int *)(dVar1 + 0x120) + 0x54),
                     *(undefined4 *)(*(int *)(dVar1 + 0x120) + 0x58),6);
        FUN_004de610(*(undefined4 *)(this->mbr_0xd8 + 4),&this->mbr_0x10,300);
      }
      this->mbr_0x188 = 0x32;
    }
    else if (dVar1 == 2) {
      this->mbr_0x188 = 0x19;
    }
    if (3 < (int)this->mbr_0x184) {
      cls_0x5b8e94_TObjectInstance::meth_0x4defe0((cls_0x5b8e94_TObjectInstance *)this);
    }
  }
  this->mbr_0x188 = this->mbr_0x188 - 1;
  return;
}



// Function at 00509360

cls_0x5b0a28 * __thiscall OOAnalyzer::cls_0x5b0a28::virt_meth_0x509360(cls_0x5b0a28 *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509390

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b0a28::virt_meth_0x509390(cls_0x5b0a28 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a063e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x578);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b0a28__vftable_5b0a28_005b0a28;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00509430

cls_0x5b0a28 * __thiscall OOAnalyzer::cls_0x5b0a28::virt_meth_0x509430(cls_0x5b0a28 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b0a28(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509450

cls_0x5b0a28 * __thiscall OOAnalyzer::cls_0x5b0a28::~cls_0x5b0a28(cls_0x5b0a28 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0658;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b0a28__vftable_5b0a28_005b0a28;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b0a28 *)pcVar1;
}



