// Decompiled methods and structure for class: cls_0x5a4b2c

/*
/OOAnalyzer/cls_0x5a4b2c
pack(disabled)
Structure cls_0x5a4b2c {
   0   cls_0x5a4b2c::vftable_5a4b2c *   4   vftptr_0x0   "pointer to cls_0x5a4b2c::vftable_5a4b2c"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   384   dword   4   mbr_0x180   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 400 Alignment: 1

*/

// Function at 0046a660

undefined4 __thiscall OOAnalyzer::cls_0x5a4b2c::virt_meth_0x46a660(cls_0x5a4b2c *this)

{
  int iVar1;
  dword dVar2;
  
  iVar1 = cls_0x5b93c4::virt_meth_0x435150
                    ((cls_0x5b93c4 *)this,s_joingame_005d3ed0,0x659cc4,0,0,0,0x280,0x1e0,0x1c2,0xa0,
                     s_widgets_005d3ec8);
  if (iVar1 == 0) {
    return 0;
  }
  dVar2 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x17c = dVar2;
  dVar2 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x180 = dVar2;
  dVar2 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x184 = dVar2;
  dVar2 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x188 = dVar2;
  dVar2 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x18c = dVar2;
  return 1;
}



// Function at 0046d470

cls_0x5a4b2c * __thiscall OOAnalyzer::cls_0x5a4b2c::virt_meth_0x46d470(cls_0x5a4b2c *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a4b2c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0046d490

cls_0x5a4b2c * __thiscall OOAnalyzer::cls_0x5a4b2c::~cls_0x5a4b2c(cls_0x5a4b2c *this)

{
  cls_0x5a4b2c *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d1fe;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a4b2c__vftable_5a4b2c *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a4b2c *)FUN_004830f0(this->mbr_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



