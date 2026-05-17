// Decompiled methods and structure for class: cls_0x5b9374

/*
/OOAnalyzer/cls_0x5b9374
pack(disabled)
Structure cls_0x5b9374 {
   0   cls_0x5a5ed4_TScreen   112   cls_0x5a5ed4_TScreen   "Component (member) class."
}
Length: 112 Alignment: 1

*/

// Function at 005338a0

undefined4 __thiscall OOAnalyzer::cls_0x5b9374::virt_meth_0x5338a0(cls_0x5b9374 *this)

{
  int iVar1;
  undefined4 uVar2;
  
  (this->cls_0x5a5ed4_TScreen).mbr_0x54 = 0;
  DAT_0066f6f4 = FUN_0047f670(s_death_dat_005e3ebc,0xffffffff,0);
  iVar1 = cls_0x5b93c4::virt_meth_0x5339b0_TDeathPane_Initialize((cls_0x5b93c4 *)&DAT_0066f500);
  if (iVar1 == 0) {
    FUN_00481c10(s_Trouble_initializing_Death_pane_005e3ec8,0);
  }
  uVar2 = cls_0x46d6b0::meth_0x46d710(DAT_0065abc0);
  FUN_0043a020(uVar2);
  cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
  return 1;
}



// Function at 00533c60

cls_0x5b9374 * __thiscall OOAnalyzer::cls_0x5b9374::cls_0x5b9374(cls_0x5b9374 *this)

{
  cls_0x5a5ed4_TScreen::cls_0x5a5ed4_TScreen(&this->cls_0x5a5ed4_TScreen);
  (this->cls_0x5a5ed4_TScreen).vftptr_0x0 =
       (cls_0x5a5ed4__vftable_5a5ed4 *)&cls_0x5b9374__vftable_5b9374_005b9374;
  return this;
}



// Function at 00533c90

cls_0x5b9374 * __thiscall OOAnalyzer::cls_0x5b9374::virt_meth_0x533c90(cls_0x5b9374 *this)

{
  byte in_stack_00000004;
  
  cls_0x5a5ed4_TScreen::~cls_0x5a5ed4_TScreen(&this->cls_0x5a5ed4_TScreen);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



