// Decompiled methods and structure for class: cls_0x5a4edc

/*
/OOAnalyzer/cls_0x5a4edc
pack(disabled)
Structure cls_0x5a4edc {
   0   TObjectInstance   112   TObjectInstance   "Component (member) class."
   112   dword   4   mbr_0x70   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   136   dword   4   mbr_0x88   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 140 Alignment: 1

*/

// Function at 00469910

undefined4 __thiscall OOAnalyzer::cls_0x5a4edc::virt_meth_0x469910(cls_0x5a4edc *this)

{
  dword dVar1;
  cls_0x5a4494__vftable_5a4494 *pcVar2;
  cls_0x46d6b0 *this_00;
  int iVar3;
  cls_0x5a4494_TPane *this_01;
  void *unaff_EBX;
  cls_0x419dd0 cStack_2c;
  cls_0x419dd0 local_14;
  cls_0x419dd0 *local_10;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  this_00 = DAT_0065abc0;
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059d150;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  (this->TObjectInstance).mbr_0x54 = 0;
  cStack_2c.mbr_0x0 = 0x469943;
  iVar3 = cls_0x46d6b0::meth_0x46d710(this_00);
  cStack_2c.mbr_0x0 = 0x469949;
  FUN_0043a020(iVar3);
  dVar1 = this->mbr_0x70;
  cStack_2c.mbr_0x0 = 0;
  cls_0x419dd0::cls_0x419dd0(&local_14);
  local_10 = &cStack_2c;
  cls_0x419dd0::cls_0x419dd0(&cStack_2c);
  (**(code **)(dVar1 + 0x30))(8);
  (this->TObjectInstance).mbr_0x40 = (dword)&this->mbr_0x70;
  (this->TObjectInstance).mbr_0x44 = (dword)&this->mbr_0x70;
  this_01 = (cls_0x5a4494_TPane *)FUN_00482fb0(0x18c);
  local_10 = (cls_0x419dd0 *)0x0;
  if (this_01 == (cls_0x5a4494_TPane *)0x0) {
    this_01 = (cls_0x5a4494_TPane *)0x0;
  }
  else {
    this_01->mbr_0xc = 0x280;
    this_01->mbr_0x1c = 0x280;
    this_01->vftptr_0x0 = &cls_0x5a4494__vftable_5a4494_005a4494;
    this_01->mbr_0x4 = 0;
    this_01->mbr_0x14 = 0;
    this_01->mbr_0x8 = 0;
    this_01->mbr_0x18 = 0;
    this_01->mbr_0x10 = 0x1e0;
    this_01->mbr_0x20 = 0x1e0;
    this_01->mbr_0x48 = 0;
    this_01->mbr_0x4c = 0;
    this_01->mbr_0x50 = 0;
    this_01->mbr_0x54 = 0;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_01->field_0x88,0x10);
    this_01->vftptr_0x0 = (cls_0x5a4494__vftable_5a4494 *)&cls_0x5a4a74__vftable_5a4a74_005a4a74;
  }
  pcVar2 = this_01->vftptr_0x0;
  local_10 = (cls_0x419dd0 *)0xffffffff;
  this->mbr_0x88 = (dword)this_01;
  (*pcVar2->virt_meth_0x491900_0)(this_01);
  cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
  ExceptionList = unaff_EBX;
  return 1;
}



// Function at 00469a20

void __thiscall OOAnalyzer::cls_0x5a4edc::virt_meth_0x469a20(cls_0x5a4edc *this)

{
  cls_0x5a4f28::meth_0x48ee10((cls_0x5a4f28 *)this);
  (**(code **)(*(int *)this->mbr_0x88 + 4))();
  if ((int *)this->mbr_0x88 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x88 + 0x7c))(1);
  }
  this->mbr_0x88 = 0;
  return;
}



// Function at 0046d330

cls_0x5a4edc * __thiscall OOAnalyzer::cls_0x5a4edc::cls_0x5a4edc(cls_0x5a4edc *this)

{
  TObjectInstance::TObjectInstance(&this->TObjectInstance);
  this->mbr_0x70 = (dword)&cls_0x5a4c30__vftable_5a4c30_005a4c30;
  this->mbr_0x78 = 0;
  this->mbr_0x7c = 0;
  this->mbr_0x80 = 0;
  this->mbr_0x74 = 0;
  (this->TObjectInstance).vftptr_0x0 =
       (TObjectInstance__vftable_5a5ed4 *)&cls_0x5a4edc__vftable_5a4edc_005a4edc;
  return this;
}



// Function at 0046d360

cls_0x5a4edc * __thiscall OOAnalyzer::cls_0x5a4edc::virt_meth_0x46d360(cls_0x5a4edc *this)

{
  byte in_stack_00000004;
  
  TObjectInstance::~TObjectInstance(&this->TObjectInstance);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



