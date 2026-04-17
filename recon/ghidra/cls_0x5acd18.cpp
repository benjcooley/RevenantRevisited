// Decompiled methods and structure for class: cls_0x5acd18

/*
/OOAnalyzer/cls_0x5acd18
pack(disabled)
Structure cls_0x5acd18 {
   0   cls_0x5acd18::vftable_5acd18 *   4   vftptr_0x0   "pointer to cls_0x5acd18::vftable_5acd18"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   264   dword   4   mbr_0x108   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
   276   dword   4   mbr_0x114   "Unsigned Double-Word (ddw, 4-bytes)"
   280   dword   4   mbr_0x118   "Unsigned Double-Word (ddw, 4-bytes)"
   284   dword   4   mbr_0x11c   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   1332   dword   4   mbr_0x534   "Unsigned Double-Word (ddw, 4-bytes)"
   1336   dword   4   mbr_0x538   "Unsigned Double-Word (ddw, 4-bytes)"
   1340   dword   4   mbr_0x53c   "Unsigned Double-Word (ddw, 4-bytes)"
   1344   dword   4   mbr_0x540   "Unsigned Double-Word (ddw, 4-bytes)"
   1348   dword   4   mbr_0x544   "Unsigned Double-Word (ddw, 4-bytes)"
   1352   dword   4   mbr_0x548   "Unsigned Double-Word (ddw, 4-bytes)"
   1356   dword   4   mbr_0x54c   "Unsigned Double-Word (ddw, 4-bytes)"
   1360   dword   4   mbr_0x550   "Unsigned Double-Word (ddw, 4-bytes)"
   1364   dword   4   mbr_0x554   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 1368 Alignment: 1

*/

// Function at 004f9400

void __thiscall OOAnalyzer::cls_0x5acd18::virt_meth_0x4f9400(cls_0x5acd18 *this)

{
  dword dVar1;
  undefined uVar2;
  int *piVar3;
  uint uVar4;
  undefined3 extraout_var;
  uint uVar5;
  
  dVar1 = this->mbr_0xd8;
  if ((((dVar1 != 0) && (*(int *)(dVar1 + 0x11c) != 0)) &&
      (*(int *)(*(int *)(dVar1 + 0x11c) + 200) != 0)) &&
     ((0 < *(int *)(dVar1 + 0xc) && (*(int *)(dVar1 + 0x10) != 0)))) {
    if (*(int *)(dVar1 + 0xc) < 1) {
      piVar3 = (int *)0x0;
    }
    else {
      piVar3 = *(int **)(dVar1 + 0x10);
    }
    (**(code **)(*piVar3 + 0x1b4))(0);
  }
  uVar5 = this->mbr_0x8 | 0x1000;
  (*this->vftptr_0x0->virt_meth_0x410bd0_64)(this);
  uVar4 = this->mbr_0x8 | 0x8000;
  (*this->vftptr_0x0->virt_meth_0x410bd0_64)(this);
  if (this->mbr_0xd8 != 0) {
    uVar2 = (*this->vftptr_0x0->virt_meth_0x4f2130_60)(this);
    if (CONCAT31(extraout_var,uVar2) != 0) {
      (**(code **)(*(int *)this->mbr_0xd8 + 0xc))(uVar4,uVar5);
    }
  }
  return;
}



// Function at 004f9520

cls_0x5acd18 * __thiscall OOAnalyzer::cls_0x5acd18::virt_meth_0x4f9520(cls_0x5acd18 *this)

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9550

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5acd18::virt_meth_0x4f9550(cls_0x5acd18 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fcfe;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x558);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5acd18__vftable_5acd18_005acd18;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f95f0

cls_0x5acd18 * __thiscall OOAnalyzer::cls_0x5acd18::virt_meth_0x4f95f0(cls_0x5acd18 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5acd18(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9610

cls_0x5acd18 * __thiscall OOAnalyzer::cls_0x5acd18::~cls_0x5acd18(cls_0x5acd18 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059fd18;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5acd18__vftable_5acd18_005acd18;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5acd18 *)pcVar1;
}



