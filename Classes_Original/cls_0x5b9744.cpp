// Decompiled methods and structure for class: cls_0x5b9744

/*
/OOAnalyzer/cls_0x5b9744
pack(disabled)
Structure cls_0x5b9744 {
   0   cls_0x5b9744::vftable_5b9744 *   4   vftptr_0x0   "pointer to cls_0x5b9744::vftable_5b9744"
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
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   384   dword   4   mbr_0x180   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   412   dword   4   mbr_0x19c   "Unsigned Double-Word (ddw, 4-bytes)"
   416   dword   4   mbr_0x1a0   "Unsigned Double-Word (ddw, 4-bytes)"
   420   dword   4   mbr_0x1a4   "Unsigned Double-Word (ddw, 4-bytes)"
   424   dword   4   mbr_0x1a8   "Unsigned Double-Word (ddw, 4-bytes)"
   428   dword   4   mbr_0x1ac   "Unsigned Double-Word (ddw, 4-bytes)"
   432   dword   4   mbr_0x1b0   "Unsigned Double-Word (ddw, 4-bytes)"
   436   dword   4   mbr_0x1b4   "Unsigned Double-Word (ddw, 4-bytes)"
   440   dword   4   mbr_0x1b8   "Unsigned Double-Word (ddw, 4-bytes)"
   444   dword   4   mbr_0x1bc   "Unsigned Double-Word (ddw, 4-bytes)"
   448   dword   4   mbr_0x1c0   "Unsigned Double-Word (ddw, 4-bytes)"
   452   dword   4   mbr_0x1c4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 456 Alignment: 1

*/

// Function at 0053a8b0

undefined4 __thiscall OOAnalyzer::cls_0x5b9744::virt_meth_0x53a8b0(cls_0x5b9744 *this)

{
  undefined4 uVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  int iVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined4 *local_58;
  int local_54;
  int local_50;
  undefined4 local_40 [16];
  
  iVar6 = 0;
  cls_0x5b93c4::virt_meth_0x435150
            ((cls_0x5b93c4 *)this,s_options_005e4494,s_default_005e448c,
             -(uint)(this->mbr_0x17c != 0) & 0x11,0,0,0x280,0x1e0,0x1c2,0xa0,s_widgets_005e4484);
  dVar2 = DAT_005d7a68;
  dVar4 = DAT_005d7a60;
  this->mbr_0x180 = DAT_005d7a18;
  dVar3 = DAT_005e91c0;
  this->mbr_0x188 = dVar4;
  iVar5 = DAT_006682a8;
  this->mbr_0x190 = dVar3;
  dVar4 = DAT_005d79e8;
  this->mbr_0x184 = dVar2;
  dVar2 = DAT_00668194;
  this->mbr_0x19c = dVar4;
  dVar4 = DAT_005d7a48;
  this->mbr_0x198 = dVar2;
  dVar2 = DAT_005d7aa0;
  this->mbr_0x1a8 = dVar4;
  this->mbr_0x194 = (uint)(iVar5 == 0);
  this->mbr_0x1a0 = DAT_005d7a9c;
  dVar4 = DAT_005d7a64;
  this->mbr_0x1b4 = 0xffffffff;
  this->mbr_0x1b8 = 0xffffffff;
  this->mbr_0x1bc = 0xffffffff;
  this->mbr_0x1c0 = 0xffffffff;
  this->mbr_0x1a4 = dVar2;
  this->mbr_0x18c = dVar4;
  this->mbr_0x1b0 = 0;
  if (this->mbr_0x1ac != 0) {
    FUN_004830f0(this->mbr_0x1ac);
  }
  dVar4 = FUN_00482fb0(DAT_005d697c * 0x24);
  this->mbr_0x1ac = dVar4;
  local_54 = 0;
  if (0 < DAT_005d697c) {
    do {
      local_58 = local_40;
      local_50 = 3;
      do {
        iVar5 = 3;
        puVar7 = local_58;
        do {
          cls_0x439000::meth_0x439060((cls_0x439000 *)&UNK_0065a904.field_0xa4,local_54);
          uVar1 = *puVar7;
          puVar7 = puVar7 + 1;
          *(undefined4 *)(iVar6 + this->mbr_0x1ac) = uVar1;
          iVar6 = iVar6 + 4;
          iVar5 = iVar5 + -1;
        } while (iVar5 != 0);
        local_58 = local_58 + 4;
        local_50 = local_50 + -1;
      } while (local_50 != 0);
      local_54 = local_54 + 1;
    } while (local_54 < DAT_005d697c);
  }
  cls_0x5b98b8::meth_0x437620((cls_0x5b98b8 *)this);
  return 1;
}



// Function at 0053aa60

void __thiscall OOAnalyzer::cls_0x5b9744::virt_meth_0x53aa60(cls_0x5b9744 *this)

{
  if (this->mbr_0x1ac != 0) {
    FUN_004830f0(this->mbr_0x1ac);
  }
  this->mbr_0x1ac = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  return;
}



// Function at 0053b9e0

void __thiscall OOAnalyzer::cls_0x5b9744::virt_meth_0x53b9e0(cls_0x5b9744 *this,undefined4 param_1)

{
  int iVar1;
  
  iVar1 = cls_0x5b98b8::meth_0x436980((cls_0x5b98b8 *)this);
  if (-1 < *(int *)(iVar1 + 0xa0)) {
    cls_0x5b98b8::meth_0x53ba80((cls_0x5b98b8 *)this,param_1);
    return;
  }
  cls_0x5b93c4::virt_meth_0x4361f0((cls_0x5b93c4 *)this,param_1);
  return;
}



// Function at 0053bdc0

cls_0x5b9744 * __thiscall OOAnalyzer::cls_0x5b9744::cls_0x5b9744(cls_0x5b9744 *this)

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->vftptr_0x0 = (cls_0x5b9744__vftable_5b9744 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->mbr_0x17c = 0;
  this->vftptr_0x0 = &cls_0x5b9744__vftable_5b9744_005b9744;
  return this;
}



// Function at 0053be20

cls_0x5b9744 * __thiscall OOAnalyzer::cls_0x5b9744::~cls_0x5b9744(cls_0x5b9744 *this)

{
  cls_0x5b9744 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1c06;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b9744__vftable_5b9744_005b9744;
  local_4 = 0;
  if (this->mbr_0x1ac != 0) {
    FUN_004830f0(this->mbr_0x1ac);
  }
  this->mbr_0x1ac = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  this->vftptr_0x0 = (cls_0x5b9744__vftable_5b9744 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5b9744 *)FUN_004830f0(*(undefined4 *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 0053beb0

cls_0x5b9744 * __thiscall OOAnalyzer::cls_0x5b9744::virt_meth_0x53beb0(cls_0x5b9744 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b9744(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



