// Decompiled methods and structure for class: cls_0x5b96f4

/*
/OOAnalyzer/cls_0x5b96f4
pack(disabled)
Structure cls_0x5b96f4 {
   0   cls_0x5a5ed4   112   cls_0x5a5ed4   "Component (member) class."
}
Length: 112 Alignment: 1

*/

// Function at 0048ed90

int __thiscall OOAnalyzer::cls_0x5b96f4::meth_0x48ed90(cls_0x5b96f4 *this)

{
  cls_0x41c7f0 *this_00;
  int iVar1;
  int in_stack_00000004;
  int in_stack_00000008;
  
  if (*(int *)(in_stack_00000004 + 0x40) == 0) {
    FUN_00481c10(s_Attempted_to_add_an_uninitialize_005d9d8c,0);
  }
  this_00 = &(this->cls_0x5a5ed4).cls_0x41c7f0;
  iVar1 = 0;
  if (0 < (int)(this->cls_0x5a5ed4).cls_0x41c7f0.mbr_0x0) {
    do {
      if (*(int *)((this->cls_0x5a5ed4).cls_0x41c7f0.mbr_0x10 + iVar1 * 4) == in_stack_00000004) {
        if (in_stack_00000008 < 0) {
          return iVar1;
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < (int)this_00->mbr_0x0);
  }
  iVar1 = cls_0x45f7c0::meth_0x41c910((cls_0x45f7c0 *)this_00,in_stack_00000004);
  *(cls_0x5b96f4 **)(in_stack_00000004 + 0x3c) = this;
  *(undefined4 *)(in_stack_00000004 + 0x44) = 1;
  return iVar1;
}



// Function at 0053a800

undefined4 __thiscall OOAnalyzer::cls_0x5b96f4::virt_meth_0x53a800(cls_0x5b96f4 *this)

{
  int iVar1;
  undefined4 uVar2;
  
  (this->cls_0x5a5ed4).mbr_0x54 = 0;
  iVar1 = cls_0x5b9744::virt_meth_0x53a8b0((cls_0x5b9744 *)&UNK_0066fcbc.field_0x4);
  if (iVar1 == 0) {
    FUN_00481c10(s_Trouble_initializing_Options_pan_005e4450,0);
  }
  UNK_0066fcbc._384_4_ = 0;
  uVar2 = cls_0x46d6b0::meth_0x46d710(DAT_0065abc0);
  FUN_0043a020(uVar2);
  meth_0x48ed90(this);
  return 1;
}



// Function at 0053bd60

cls_0x5b96f4 * __thiscall OOAnalyzer::cls_0x5b96f4::cls_0x5b96f4(cls_0x5b96f4 *this)

{
  cls_0x5a5ed4::cls_0x5a5ed4(&this->cls_0x5a5ed4);
  (this->cls_0x5a5ed4).vftptr_0x0 =
       (cls_0x5a5ed4__vftable_5a5ed4 *)&cls_0x5b96f4__vftable_5b96f4_005b96f4;
  return this;
}



// Function at 0053bd80

cls_0x5b96f4 * __thiscall OOAnalyzer::cls_0x5b96f4::~cls_0x5b96f4(cls_0x5b96f4 *this)

{
  cls_0x5a5ed4 *pcVar1;
  
  (this->cls_0x5a5ed4).vftptr_0x0 =
       (cls_0x5a5ed4__vftable_5a5ed4 *)&cls_0x5b96f4__vftable_5b96f4_005b96f4;
  pcVar1 = cls_0x5a5ed4::~cls_0x5a5ed4(&this->cls_0x5a5ed4);
  return (cls_0x5b96f4 *)pcVar1;
}



// Function at 0053bda0

cls_0x5b96f4 * __thiscall OOAnalyzer::cls_0x5b96f4::virt_meth_0x53bda0(cls_0x5b96f4 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b96f4(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



