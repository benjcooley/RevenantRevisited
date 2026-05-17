// Decompiled methods and structure for class: cls_0x5b92bc

/*
/OOAnalyzer/cls_0x5b92bc
pack(disabled)
Structure cls_0x5b92bc {
   0   cls_0x5b92bc::vftable_5b92bc *   4   vftptr_0x0   "pointer to cls_0x5b92bc::vftable_5b92bc"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 384 Alignment: 1

*/

// Function at 00533350

undefined4 __thiscall OOAnalyzer::cls_0x5b92bc::virt_meth_0x533350(cls_0x5b92bc *this)

{
  int iVar1;
  undefined *puVar2;
  cls_0x5a3c68 *pcVar3;
  int *piVar4;
  bool bVar5;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1976;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a4494_TPane::virt_meth_0x434e40((cls_0x5a4494_TPane *)this);
  if (DAT_0066f4f8 == (cls_0x46d6b0 *)0x0) {
    DAT_0066f4f8 = (cls_0x46d6b0 *)FUN_0047f670(s_exitsure_dat_005e3e8c,0xffffffff,0);
    bVar5 = DAT_0066f4f8 == (cls_0x46d6b0 *)0x0;
    this->mbr_0x60 = this->mbr_0x60 | 0xe;
    if (bVar5) {
      ExceptionList = local_c;
      return 0;
    }
    pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
    local_4 = 0;
    if (pcVar3 != (cls_0x5a3c68 *)0x0) {
      cls_0x5a3c68::cls_0x5a3c68
                (pcVar3,DAT_0066f4f8,&DAT_005e3e9c,0x59,&LAB_00533680,0,0,0,0x10,0xffffffff);
    }
    local_4 = 0xffffffff;
    cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
    pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
    local_4 = 1;
    if (pcVar3 != (cls_0x5a3c68 *)0x0) {
      cls_0x5a3c68::cls_0x5a3c68
                (pcVar3,DAT_0066f4f8,&DAT_005e3ea0,0x4e,&LAB_005336b0,0,0,0,0x10,0xffffffff);
    }
    local_4 = 0xffffffff;
    cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
    piVar4 = (int *)cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
    (**(code **)(*piVar4 + 0x1c))(piVar4[5] & 0xffffffdf);
    piVar4 = (int *)cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
    (**(code **)(*piVar4 + 0x1c))(piVar4[5] & 0xffffffdf);
    piVar4 = (int *)cls_0x46d6b0::meth_0x46d710(DAT_0066f4f8);
    puVar2 = PTR_DAT_005d79e0;
    iVar1 = piVar4[1];
    this->mbr_0x14 = (*(int *)(PTR_DAT_005d79e0 + 4) - *piVar4) / 2;
    this->mbr_0x18 = (*(int *)(puVar2 + 8) - iVar1) / 2;
    (*this->vftptr_0x0->virt_meth_0x444f90_32)(this);
    this->mbr_0x17c = 3;
  }
  ExceptionList = local_c;
  return 1;
}



// Function at 00533580

void __thiscall OOAnalyzer::cls_0x5b92bc::virt_meth_0x533580(cls_0x5b92bc *this)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  
  if (this->mbr_0x50 != 0) {
    iVar1 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
    if (iVar1 != 0) {
      piVar2 = (int *)cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
      (**(code **)(*piVar2 + 0x1c))(piVar2[5] | 0x20);
    }
    iVar1 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
    if (iVar1 != 0) {
      piVar2 = (int *)cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
      (**(code **)(*piVar2 + 0x1c))(piVar2[5] | 0x20);
    }
    if (DAT_0066f4f8 != (cls_0x46d6b0 *)0x0) {
      uVar3 = cls_0x46d6b0::meth_0x46d710(DAT_0066f4f8);
      cls_0x5a68d8::meth_0x4bd680((cls_0x5a68d8 *)PTR_DAT_005d79e0,0,0,uVar3,0x2010);
      (*this->vftptr_0x0->virt_meth_0x4361b0_144)((cls_0x5b93c4 *)this);
    }
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  }
  cls_0x5b98b8::virt_meth_0x435de0((cls_0x5b98b8 *)this);
  return;
}



// Function at 00533620

void __thiscall OOAnalyzer::cls_0x5b92bc::virt_meth_0x533620(cls_0x5b92bc *this,int param_1)

{
  int in_stack_00000008;
  
  cls_0x5b93c4::virt_meth_0x4361f0((cls_0x5b93c4 *)this,param_1);
  if ((in_stack_00000008 != 0) && ((param_1 == 0xd || (param_1 == 0x1b)))) {
    UNK_0065cb3c._4_4_ = 1;
    cls_0x5a5320_TPlayScreen::meth_0x48efd0((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0);
    (*this->vftptr_0x0->virt_meth_0x435010_8)((cls_0x5b93c4 *)this);
    (*this->vftptr_0x0->virt_meth_0x533520_4)(this);
    cls_0x5a5ed4_TScreen::virt_meth_0x491870((cls_0x5a5ed4_TScreen *)&DAT_0065caf0);
  }
  return;
}



// Function at 00533700

cls_0x5b92bc * __thiscall OOAnalyzer::cls_0x5b92bc::cls_0x5b92bc(cls_0x5b92bc *this)

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->vftptr_0x0 = (cls_0x5b92bc__vftable_5b92bc *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5b92bc__vftable_5b92bc_005b92bc;
  return this;
}



// Function at 00533760

cls_0x5b92bc * __thiscall OOAnalyzer::cls_0x5b92bc::~cls_0x5b92bc(cls_0x5b92bc *this)

{
  cls_0x5b92bc *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1996;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b92bc__vftable_5b92bc_005b92bc;
  local_4 = 0;
  if (DAT_0066f4f8 != 0) {
    FUN_00482f80(DAT_0066f4f8);
    DAT_0066f4f8 = 0;
  }
  cls_0x5a5320_TPlayScreen::meth_0x48ed60((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0);
  cls_0x5a4f28::meth_0x48ef30((cls_0x5a4f28 *)&DAT_0065caf0);
  cls_0x5a4f28::meth_0x48ee10((cls_0x5a4f28 *)&DAT_0065caf0);
  cls_0x5a5320_TPlayScreen::meth_0x48efd0((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0);
  UNK_0065cb3c._4_4_ = 1;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  this->vftptr_0x0 = (cls_0x5b92bc__vftable_5b92bc *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5b92bc *)FUN_004830f0(*(undefined4 *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00533820

cls_0x5b92bc * __thiscall OOAnalyzer::cls_0x5b92bc::virt_meth_0x533820(cls_0x5b92bc *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b92bc(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



