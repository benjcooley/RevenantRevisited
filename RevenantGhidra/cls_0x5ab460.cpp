// Decompiled methods and structure for class: cls_0x5ab460

/*
/OOAnalyzer/cls_0x5ab460
pack(disabled)
Structure cls_0x5ab460 {
   0   cls_0x5ab460::vftable_5ab460 *   4   vftptr_0x0   "pointer to cls_0x5ab460::vftable_5ab460"
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
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   980   dword   4   mbr_0x3d4   "Unsigned Double-Word (ddw, 4-bytes)"
   984   dword   4   mbr_0x3d8   "Unsigned Double-Word (ddw, 4-bytes)"
   988   dword   4   mbr_0x3dc   "Unsigned Double-Word (ddw, 4-bytes)"
   1700   dword   4   mbr_0x6a4   "Unsigned Double-Word (ddw, 4-bytes)"
   1704   dword   4   mbr_0x6a8   "Unsigned Double-Word (ddw, 4-bytes)"
   1708   dword   4   mbr_0x6ac   "Unsigned Double-Word (ddw, 4-bytes)"
   2060   dword   4   mbr_0x80c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 2064 Alignment: 1

*/

// Function at 004ec890

void __thiscall OOAnalyzer::cls_0x5ab460::virt_meth_0x4ec890(cls_0x5ab460 *this)

{
  dword dVar1;
  int iVar2;
  
  SoftwareRenderer::virt_meth_0x4de800((SoftwareRenderer *)this);
  this->mbr_0x188 = this->mbr_0x188 + 1;
  if ((TPlayScreen *)this->mbr_0x184 != (TPlayScreen *)0x0) {
    TPlayScreen::meth_0x4cee70((TPlayScreen *)this->mbr_0x184);
    dVar1 = this->mbr_0x188;
    if ((int)dVar1 < 0xf0) {
      if (((((int *)this->mbr_0x184)[2] & 0x2000000U) == 0) ||
         (iVar2 = (**(code **)(*(int *)this->mbr_0x184 + 0x1c0))(), iVar2 < 1)) {
        this->mbr_0x188 = 0xf0;
      }
      dVar1 = this->mbr_0x188;
    }
    if ((dVar1 == 0xf0) && ((DAT_0066829c == 0 || (DAT_0067682c != 0)))) {
      (**(code **)(*(int *)this->mbr_0x184 + 0x40))(((int *)this->mbr_0x184)[2] & 0xff7fffff);
      (**(code **)(*(int *)this->mbr_0x184 + 0x40))(((int *)this->mbr_0x184)[2] & 0xfdffffff);
    }
    return;
  }
  return;
}



// Function at 004ec9d0

void __thiscall OOAnalyzer::cls_0x5ab460::virt_meth_0x4ec9d0(cls_0x5ab460 *this,int param_1)

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
  this->mbr_0x184 = 0;
  (*this->vftptr_0x0->virt_meth_0x410bd0_64)(this);
  return;
}



// Function at 004f7e80

cls_0x5ab460 * __thiscall OOAnalyzer::cls_0x5ab460::virt_meth_0x4f7e80(cls_0x5ab460 *this)

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7eb0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5ab460::virt_meth_0x4f7eb0(cls_0x5ab460 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f86e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x884);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ab460__vftable_5ab460_005ab460;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f7f50

cls_0x5ab460 * __thiscall OOAnalyzer::cls_0x5ab460::virt_meth_0x4f7f50(cls_0x5ab460 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5ab460(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7f70

cls_0x5ab460 * __thiscall OOAnalyzer::cls_0x5ab460::~cls_0x5ab460(cls_0x5ab460 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f888;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ab460__vftable_5ab460_005ab460;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ab460 *)pcVar1;
}



// Function at 004f7fc0

TScreen * __thiscall
OOAnalyzer::cls_0x5ab460::virt_meth_0x4f7fc0(cls_0x5ab460 *this,int param_1)

{
  uint uVar1;
  uint uVar2;
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f8ab;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x234);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    uVar1 = *(uint *)(param_1 + 4);
    uVar2 = this_00->mbr_0x8;
    *(undefined4 *)&this_00->field_0xe4 = 0;
    this_00[1].mbr_0x38 = 0;
    this_00->mbr_0x8 = uVar2 | uVar1 | 0x48001;
    this_00[1].mbr_0x1c = 0;
    this_00[1].mbr_0xc = 0;
    this_00[1].mbr_0xe = 0;
    *(undefined *)&this_00[1].mbr_0x48 = 0;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4f8080_005ab4c4;
    *(undefined4 *)&this_00[1].field_0x70 = 1;
    *(undefined4 *)&this_00[1].field_0xa4 = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



