// Decompiled methods and structure for class: cls_0x5a5808_TBottomBarPane

/*
/OOAnalyzer/cls_0x5a5808_TBottomBarPane
pack(disabled)
Structure cls_0x5a5808_TBottomBarPane {
   0   cls_0x5a5808_TBottomBarPane::vftable_5a5808 *   4   vftptr_0x0   "pointer to cls_0x5a5808_TBottomBarPane::vftable_5a5808"
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
   384   dword   4   mbr_0x180   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 392 Alignment: 1

*/

// Function at 00487da0

cls_0x5a5808_TBottomBarPane * __thiscall OOAnalyzer::cls_0x5a5808_TBottomBarPane::cls_0x5a5808_TBottomBarPane(cls_0x5a5808_TBottomBarPane *this)

{
  this->mbr_0x8 = 0x1a4;
  this->mbr_0x18 = 0x1a4;
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->mbr_0x10 = 0x3c;
  this->mbr_0x20 = 0x3c;
  this->vftptr_0x0 = (cls_0x5a5808__vftable_5a5808 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a5808__vftable_5a5808_005a5808;
  return this;
}



// Function at 00487e00

cls_0x5a5808_TBottomBarPane * __thiscall OOAnalyzer::cls_0x5a5808_TBottomBarPane::~cls_0x5a5808_TBottomBarPane(cls_0x5a5808_TBottomBarPane *this)

{
  LPCVOID pvVar1;
  SIZE_T SVar2;
  cls_0x5a5808_TBottomBarPane *pcVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d5a6;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a5808__vftable_5a5808 *)&PTR_virt_meth_0x434e40_005a45c8;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  this->vftptr_0x0 = (cls_0x5a5808__vftable_5a5808 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  pvVar1 = *(LPCVOID *)&this->field_0x98;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  pcVar3 = (cls_0x5a5808_TBottomBarPane *)FUN_0058cfab(pvVar1);
  ExceptionList = local_c;
  return pcVar3;
}



// Function at 00487ef0

cls_0x5a5808_TBottomBarPane * __thiscall OOAnalyzer::cls_0x5a5808_TBottomBarPane::virt_meth_0x487ef0(cls_0x5a5808_TBottomBarPane *this)

{
  SIZE_T SVar1;
  byte in_stack_00000004;
  
  ~cls_0x5a5808_TBottomBarPane(this);
  if ((in_stack_00000004 & 1) != 0) {
    if (this != (cls_0x5a5808_TBottomBarPane *)0x0) {
      SVar1 = FUN_0058cff3(this);
      DAT_0065ba08 = DAT_0065ba08 - SVar1;
    }
    FUN_0058cfab(this);
  }
  return this;
}



// Function at 0052c780

undefined4 __thiscall OOAnalyzer::cls_0x5a5808_TBottomBarPane::virt_meth_0x52c780_TBottomBarPane_Initialize(cls_0x5a5808_TBottomBarPane *this)

{
  cls_0x46d6b0 *this_00;
  dword dVar1;
  
  cls_0x5a4494_TPane::virt_meth_0x434e40((cls_0x5a4494_TPane *)this);
  dVar1 = cls_0x46d6b0::meth_0x46d710(DAT_0065a570);
  this_00 = DAT_0065a570;
  this->mbr_0x180 = dVar1;
  dVar1 = cls_0x46d6b0::meth_0x46d710(this_00);
  this->mbr_0x184 = dVar1;
  return 1;
}



// Function at 0052c7c0

void __thiscall OOAnalyzer::cls_0x5a5808_TBottomBarPane::virt_meth_0x52c7c0(cls_0x5a5808_TBottomBarPane *this)

{
  TCharacter::virt_meth_0x491970((TCharacter *)&DAT_0065b028);
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)&DAT_0065c6f8);
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  return;
}



// Function at 0052c7e0

void __thiscall OOAnalyzer::cls_0x5a5808_TBottomBarPane::virt_meth_0x52c7e0(cls_0x5a5808_TBottomBarPane *this)

{
  DAT_0065b02c._68_4_ = 1;
  DAT_0065b02c._72_4_ = 1;
  UNK_0065c6fc._68_4_ = 1;
  UNK_0065c6fc._72_4_ = 1;
  this->mbr_0x48 = 1;
  this->mbr_0x4c = 1;
  return;
}



// Function at 0052c800

void __thiscall OOAnalyzer::cls_0x5a5808_TBottomBarPane::virt_meth_0x52c800(cls_0x5a5808_TBottomBarPane *this)

{
  if (this->mbr_0x50 != 0) {
    (*this->vftptr_0x0->virt_meth_0x52c880_84)(this);
  }
  cls_0x5a5658::virt_meth_0x52ca70((cls_0x5a5658 *)&DAT_0065b028,PTR_DAT_005d79e0);
  cls_0x5a5a30_TQuickSpellPane::virt_meth_0x5444c0((cls_0x5a5a30_TQuickSpellPane *)&DAT_0065c6f8);
  cls_0x5b98b8::virt_meth_0x435de0((cls_0x5b98b8 *)this);
  if (this->mbr_0x50 != 0) {
    FUN_004aacb0(this->mbr_0x4,this->mbr_0x8,this->mbr_0xc,this->mbr_0x10);
  }
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  return;
}



// Function at 0052c880

void __thiscall OOAnalyzer::cls_0x5a5808_TBottomBarPane::virt_meth_0x52c880(cls_0x5a5808_TBottomBarPane *this)

{
  cls_0x5a68d8 *in_stack_00000004;
  
  cls_0x5a68d8::meth_0x4bd5e0(in_stack_00000004,0,0,this->mbr_0x180,0,0,this->mbr_0xc,0x3c);
  cls_0x5a68d8::meth_0x4bd680(in_stack_00000004,this->mbr_0xc - 10,0,this->mbr_0x184,0x80000000);
  (*this->vftptr_0x0->FUN_004361b0_144)();
  if (in_stack_00000004 != (cls_0x5a68d8 *)PTR_DAT_005d79e0) {
    cls_0x5b93c4::virt_meth_0x435cb0((cls_0x5b93c4 *)this);
    cls_0x5a5658::virt_meth_0x52ca70((cls_0x5a5658 *)&DAT_0065b028,in_stack_00000004);
    cls_0x5a5a30_TQuickSpellPane::virt_meth_0x5444c0((cls_0x5a5a30_TQuickSpellPane *)&DAT_0065c6f8);
  }
  return;
}



// Function at 0052c910

void __thiscall OOAnalyzer::cls_0x5a5808_TBottomBarPane::virt_meth_0x52c910(cls_0x5a5808_TBottomBarPane *this)

{
  this->mbr_0x4 = this->mbr_0x14;
  this->mbr_0x8 = this->mbr_0x18;
  this->mbr_0xc = this->mbr_0x1c;
  this->mbr_0x10 = this->mbr_0x20;
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  return;
}



// Function at 0052c930

void __thiscall OOAnalyzer::cls_0x5a5808_TBottomBarPane::virt_meth_0x52c930(cls_0x5a5808_TBottomBarPane *this,dword param_1)

{
  dword in_stack_00000008;
  
  this->mbr_0x1c = param_1;
  this->mbr_0x20 = in_stack_00000008;
  DAT_0065b02c._24_4_ = param_1;
  DAT_0065b02c._28_4_ = in_stack_00000008;
  UNK_0065c6fc._24_4_ = param_1;
  UNK_0065c6fc._28_4_ = in_stack_00000008;
  return;
}



