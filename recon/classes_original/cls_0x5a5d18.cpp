// Decompiled methods and structure for class: cls_0x5a5d18_TLogoScreen

/*
/OOAnalyzer/cls_0x5a5d18_TLogoScreen
pack(disabled)
Structure cls_0x5a5d18_TLogoScreen {
   0   cls_0x5a5ed4_TScreen   112   cls_0x5a5ed4_TScreen   "Component (member) class."
   112   dword   4   mbr_0x70   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   136   dword   4   mbr_0x88   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 140 Alignment: 1

*/

// Function at 00488e70

cls_0x5a5d18_TLogoScreen * __thiscall OOAnalyzer::cls_0x5a5d18_TLogoScreen::cls_0x5a5d18_TLogoScreen(cls_0x5a5d18_TLogoScreen *this)

{
  cls_0x5a5ed4_TScreen::cls_0x5a5ed4_TScreen(&this->cls_0x5a5ed4_TScreen);
  this->mbr_0x70 = (dword)&cls_0x5a4c30__vftable_5a4c30_005a4c30;
  this->mbr_0x78 = 0;
  this->mbr_0x7c = 0;
  this->mbr_0x80 = 0;
  this->mbr_0x74 = 0;
  (this->cls_0x5a5ed4_TScreen).vftptr_0x0 =
       (cls_0x5a5ed4__vftable_5a5ed4 *)&cls_0x5a5d18__vftable_5a5d18_005a5d18;
  return this;
}



// Function at 00488ea0

cls_0x5a5d18_TLogoScreen * __thiscall OOAnalyzer::cls_0x5a5d18_TLogoScreen::virt_meth_0x488ea0_TLogoScreen_Dtor(cls_0x5a5d18_TLogoScreen *this)

{
  int iVar1;
  byte in_stack_00000004;
  
  cls_0x5a5ed4_TScreen::~cls_0x5a5ed4_TScreen(&this->cls_0x5a5ed4_TScreen);
  if ((in_stack_00000004 & 1) != 0) {
    if (this != (cls_0x5a5d18_TLogoScreen *)0x0) {
      iVar1 = FUN_0058cff3(this);
      DAT_0065ba08 = DAT_0065ba08 - iVar1;
    }
    FUN_0058cfab(this);
  }
  return this;
}



// Function at 0053a2c0

undefined4 __thiscall OOAnalyzer::cls_0x5a5d18_TLogoScreen::virt_meth_0x53a2c0_TLogoScreen_Initialize(cls_0x5a5d18_TLogoScreen *this)

{
  uint *puVar1;
  dword dVar2;
  dword dVar3;
  cls_0x5a4494__vftable_5a4494 *pcVar4;
  cls_0x5b98b8 *pcVar5;
  undefined *puVar6;
  cls_0x5a4494_TPane *this_00;
  cls_0x5a3c68 *pcVar7;
  int iVar8;
  void *unaff_EBX;
  cls_0x419dd0 cStack_2c;
  char *pcStack_28;
  cls_0x419dd0 local_14;
  cls_0x419dd0 *local_10;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_005a1be2;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  (this->cls_0x5a5ed4_TScreen).mbr_0x54 = 0;
  pcStack_28 = (char *)0x53a2ed;
  cls_0x45f7c0::meth_0x51eda0_TPlayerManager_Clear((cls_0x45f7c0 *)&DAT_0065a890);
  pcStack_28 = (char *)0x53a2f2;
  FUN_00446cb0();
  pcStack_28 = (char *)0x0;
  cStack_2c.mbr_0x0 = 0xffffffff;
  DAT_0066fcac = FUN_0047f670(s_menus_dat_005e43dc);
  pcStack_28 = s_cursor_005e43e8;
  cStack_2c.mbr_0x0 = 0x53a317;
  pcStack_28 = (char *)cls_0x46d6b0::meth_0x46d710(DAT_0065abc0);
  cStack_2c.mbr_0x0 = 0x53a31d;
  FUN_0043a020();
  dVar2 = this->mbr_0x70;
  pcStack_28 = (char *)0x0;
  cStack_2c.mbr_0x0 = 0;
  cls_0x419dd0::cls_0x419dd0(&local_14);
  local_10 = &cStack_2c;
  cls_0x419dd0::cls_0x419dd0(&cStack_2c);
  (**(code **)(dVar2 + 0x30))(8);
  (this->cls_0x5a5ed4_TScreen).mbr_0x40 = (dword)&this->mbr_0x70;
  (this->cls_0x5a5ed4_TScreen).mbr_0x44 = (dword)&this->mbr_0x70;
  this_00 = (cls_0x5a4494_TPane *)FUN_00482fb0(0x17c);
  puVar6 = PTR_DAT_005d79e0;
  local_10 = (cls_0x419dd0 *)0x0;
  if (this_00 == (cls_0x5a4494_TPane *)0x0) {
    this_00 = (cls_0x5a4494_TPane *)0x0;
  }
  else {
    this_00->vftptr_0x0 = &cls_0x5a4494__vftable_5a4494_005a4494;
    dVar2 = *(dword *)(puVar6 + 8);
    dVar3 = *(dword *)(puVar6 + 4);
    this_00->mbr_0xc = dVar3;
    this_00->mbr_0x1c = dVar3;
    this_00->mbr_0x4 = 0;
    this_00->mbr_0x14 = 0;
    this_00->mbr_0x8 = 0;
    this_00->mbr_0x18 = 0;
    this_00->mbr_0x10 = dVar2;
    this_00->mbr_0x20 = dVar2;
    this_00->mbr_0x48 = 0;
    this_00->mbr_0x4c = 0;
    this_00->mbr_0x50 = 0;
    this_00->mbr_0x54 = 0;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00->field_0x88,0x10);
    this_00->vftptr_0x0 = (cls_0x5a4494__vftable_5a4494 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  }
  pcVar4 = this_00->vftptr_0x0;
  local_10 = (cls_0x419dd0 *)0xffffffff;
  this->mbr_0x88 = (dword)this_00;
  (*pcVar4->virt_meth_0x491900_0)(this_00);
  puVar1 = (uint *)(this->mbr_0x88 + 0x60);
  *puVar1 = *puVar1 | 2;
  pcVar7 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_10 = (cls_0x419dd0 *)0x1;
  if (pcVar7 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar7,DAT_0066fcac,s_MenuNewGame_005e43f0,0,0,0,0,0xffffffff,0x10,0xffffffff);
  }
  local_10 = (cls_0x419dd0 *)0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this->mbr_0x88);
  pcVar7 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_10 = (cls_0x419dd0 *)0x2;
  if (pcVar7 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar7,DAT_0066fcac,s_MenuLoadGame_005e43fc,0,0,0,0,0xffffffff,0x10,0xffffffff);
  }
  local_10 = (cls_0x419dd0 *)0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this->mbr_0x88);
  pcVar7 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_10 = (cls_0x419dd0 *)0x3;
  if (pcVar7 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar7,DAT_0066fcac,s_MenuMulti_005e440c,0,0,0,0,0xffffffff,0x10,0xffffffff);
  }
  local_10 = (cls_0x419dd0 *)0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this->mbr_0x88);
  pcVar7 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_10 = (cls_0x419dd0 *)0x4;
  if (pcVar7 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar7,DAT_0066fcac,s_MenuOptions_005e4418,0,0,0,0,0xffffffff,0x10,0xffffffff);
  }
  local_10 = (cls_0x419dd0 *)0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this->mbr_0x88);
  pcVar7 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_10 = (cls_0x419dd0 *)0x5;
  if (pcVar7 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar7,DAT_0066fcac,s_MenuExit_005e4424,0,0,0,0,0xffffffff,0x10,0xffffffff);
  }
  local_10 = (cls_0x419dd0 *)0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this->mbr_0x88);
  iVar8 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this->mbr_0x88);
  pcVar5 = (cls_0x5b98b8 *)this->mbr_0x88;
  *(undefined **)(iVar8 + 0x80) = &LAB_0053a1f0;
  *(uint *)(iVar8 + 0x14) = *(uint *)(iVar8 + 0x14) | 0x40;
  iVar8 = cls_0x5b98b8::meth_0x436900(pcVar5);
  *(undefined **)(iVar8 + 0x80) = &LAB_0053a220;
  *(uint *)(iVar8 + 0x14) = *(uint *)(iVar8 + 0x14) | 0x40;
  iVar8 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this->mbr_0x88);
  pcVar5 = (cls_0x5b98b8 *)this->mbr_0x88;
  *(undefined **)(iVar8 + 0x80) = &LAB_0053a240;
  *(uint *)(iVar8 + 0x14) = *(uint *)(iVar8 + 0x14) | 0x40;
  iVar8 = cls_0x5b98b8::meth_0x436900(pcVar5);
  *(undefined **)(iVar8 + 0x80) = &LAB_0053a260;
  *(uint *)(iVar8 + 0x14) = *(uint *)(iVar8 + 0x14) | 0x40;
  iVar8 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this->mbr_0x88);
  *(uint *)(iVar8 + 0x14) = *(uint *)(iVar8 + 0x14) | 0x40;
  *(undefined **)(iVar8 + 0x80) = &LAB_0053a2a0;
  cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
  ExceptionList = unaff_EBX;
  return 1;
}



// Function at 0053a680

void __thiscall OOAnalyzer::cls_0x5a5d18_TLogoScreen::virt_meth_0x53a680_TLogoScreen_Close(cls_0x5a5d18_TLogoScreen *this)

{
  cls_0x5a4f28::meth_0x48ee10((cls_0x5a4f28 *)this);
  (**(code **)(*(int *)this->mbr_0x88 + 4))();
  if ((int *)this->mbr_0x88 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x88 + 0x7c))(1);
  }
  FUN_00482f80(DAT_0066fcac);
  DAT_0066fcac = 0;
  FUN_0043a020(0);
  return;
}



