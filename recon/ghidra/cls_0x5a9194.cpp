// Decompiled methods and structure for class: cls_0x5a9194

/*
/OOAnalyzer/cls_0x5a9194
pack(disabled)
Structure cls_0x5a9194 {
   0   cls_0x5a9194::vftable_5a9194 *   4   vftptr_0x0   "pointer to cls_0x5a9194::vftable_5a9194"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   264   dword   4   mbr_0x108   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
   276   dword   4   mbr_0x114   "Unsigned Double-Word (ddw, 4-bytes)"
   280   dword   4   mbr_0x118   "Unsigned Double-Word (ddw, 4-bytes)"
   284   dword   4   mbr_0x11c   "Unsigned Double-Word (ddw, 4-bytes)"
   288   dword   4   mbr_0x120   "Unsigned Double-Word (ddw, 4-bytes)"
   292   dword   4   mbr_0x124   "Unsigned Double-Word (ddw, 4-bytes)"
   300   dword   4   mbr_0x12c   "Unsigned Double-Word (ddw, 4-bytes)"
   308   dword   4   mbr_0x134   "Unsigned Double-Word (ddw, 4-bytes)"
   312   dword   4   mbr_0x138   "Unsigned Double-Word (ddw, 4-bytes)"
   316   dword   4   mbr_0x13c   "Unsigned Double-Word (ddw, 4-bytes)"
   320   dword   4   mbr_0x140   "Unsigned Double-Word (ddw, 4-bytes)"
   336   dword   4   mbr_0x150   "Unsigned Double-Word (ddw, 4-bytes)"
   340   dword   4   mbr_0x154   "Unsigned Double-Word (ddw, 4-bytes)"
   344   dword   4   mbr_0x158   "Unsigned Double-Word (ddw, 4-bytes)"
   348   dword   4   mbr_0x15c   "Unsigned Double-Word (ddw, 4-bytes)"
   352   dword   4   mbr_0x160   "Unsigned Double-Word (ddw, 4-bytes)"
   356   dword   4   mbr_0x164   "Unsigned Double-Word (ddw, 4-bytes)"
   360   dword   4   mbr_0x168   "Unsigned Double-Word (ddw, 4-bytes)"
   364   dword   4   mbr_0x16c   "Unsigned Double-Word (ddw, 4-bytes)"
   368   dword   4   mbr_0x170   "Unsigned Double-Word (ddw, 4-bytes)"
   372   dword   4   mbr_0x174   "Unsigned Double-Word (ddw, 4-bytes)"
   376   dword   4   mbr_0x178   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   13500   dword   4   mbr_0x34bc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 13504 Alignment: 1

*/

// Function at 004e1770

void __thiscall OOAnalyzer::cls_0x5a9194::virt_meth_0x4e1770(cls_0x5a9194 *this)

{
  int iVar1;
  int *piVar2;
  TPlayScreen *this_00;
  
  if (((((this->mbr_0xd8 != 0) && (iVar1 = *(int *)(this->mbr_0xd8 + 4), iVar1 != 0)) &&
       (piVar2 = *(int **)(iVar1 + 0xe0), piVar2 != (int *)0x0)) &&
      ((*piVar2 == 3 || ((piVar2 != (int *)0x0 && (*piVar2 == 0x19)))))) &&
     (this_00 = (TPlayScreen *)piVar2[0x11], this_00 != (TPlayScreen *)0x0)) {
    TPlayScreen::meth_0x4d3590_Burn(this_00);
    TPlayScreen::meth_0x4d3750(this_00,&this->mbr_0x10);
    cls_0x5b99c0::meth_0x53f560((cls_0x5b99c0 *)this->mbr_0xd8);
  }
  this->mbr_0x184 = 0;
  return;
}



// Function at 004e17f0

void __thiscall OOAnalyzer::cls_0x5a9194::virt_meth_0x4e17f0(cls_0x5a9194 *this)

{
  undefined uVar1;
  int *piVar2;
  uint uVar3;
  undefined3 extraout_var;
  dword dVar4;
  uint uVar5;
  
  SoftwareRenderer::virt_meth_0x4de800((SoftwareRenderer *)this);
  dVar4 = this->mbr_0x184 + 1;
  this->mbr_0x184 = dVar4;
  if (99 < (int)dVar4) {
    dVar4 = this->mbr_0xd8;
    if ((((dVar4 != 0) && (*(int *)(dVar4 + 0x11c) != 0)) &&
        (*(int *)(*(int *)(dVar4 + 0x11c) + 200) != 0)) &&
       ((0 < *(int *)(dVar4 + 0xc) && (*(int *)(dVar4 + 0x10) != 0)))) {
      if (*(int *)(dVar4 + 0xc) < 1) {
        piVar2 = (int *)0x0;
      }
      else {
        piVar2 = *(int **)(dVar4 + 0x10);
      }
      (**(code **)(*piVar2 + 0x1b4))(0);
    }
    uVar5 = this->mbr_0x8 | 0x1000;
    (*this->vftptr_0x0->virt_meth_0x410bd0_64)(this);
    uVar3 = this->mbr_0x8 | 0x8000;
    (*this->vftptr_0x0->virt_meth_0x410bd0_64)(this);
    if (this->mbr_0xd8 != 0) {
      uVar1 = (*this->vftptr_0x0->virt_meth_0x4e2200_60)(this);
      if (CONCAT31(extraout_var,uVar1) != 0) {
        (**(code **)(*(int *)this->mbr_0xd8 + 0xc))(uVar3,uVar5);
      }
    }
  }
  return;
}



// Function at 004f51a0

cls_0x5a9194 * __thiscall OOAnalyzer::cls_0x5a9194::virt_meth_0x4f51a0(cls_0x5a9194 *this)

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f51d0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5a9194::virt_meth_0x4f51d0(cls_0x5a9194 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ee8e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x34c0);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5a9194__vftable_5a9194_005a9194;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f5270

cls_0x5a9194 * __thiscall OOAnalyzer::cls_0x5a9194::virt_meth_0x4f5270(cls_0x5a9194 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a9194(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f5290

cls_0x5a9194 * __thiscall OOAnalyzer::cls_0x5a9194::~cls_0x5a9194(cls_0x5a9194 *this)

{
  cls_0x5a9194 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059eea8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a9194__vftable_5a9194_005a9194;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a9194 *)cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return pcVar1;
}



