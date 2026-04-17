// Decompiled methods and structure for class: cls_0x5b4814

/*
/OOAnalyzer/cls_0x5b4814
pack(disabled)
Structure cls_0x5b4814 {
   0   cls_0x5b4814::vftable_5b4814 *   4   vftptr_0x0   "pointer to cls_0x5b4814::vftable_5b4814"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   224   dword   4   mbr_0xe0   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   264   dword   4   mbr_0x108   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
   332   dword   4   mbr_0x14c   "Unsigned Double-Word (ddw, 4-bytes)"
   344   dword   4   mbr_0x158   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
   420   dword   4   mbr_0x1a4   "Unsigned Double-Word (ddw, 4-bytes)"
   424   dword   4   mbr_0x1a8   "Unsigned Double-Word (ddw, 4-bytes)"
   444   dword   4   mbr_0x1bc   "Unsigned Double-Word (ddw, 4-bytes)"
   448   dword   4   mbr_0x1c0   "Unsigned Double-Word (ddw, 4-bytes)"
   452   dword   4   mbr_0x1c4   "Unsigned Double-Word (ddw, 4-bytes)"
   456   dword   4   mbr_0x1c8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 732 Alignment: 1

*/

// Function at 00512fd0

undefined4 __thiscall OOAnalyzer::cls_0x5b4814::virt_meth_0x512fd0(cls_0x5b4814 *this)

{
  uint uVar1;
  uint *puVar2;
  int iVar3;
  
  FUN_004de1f0();
  FUN_004de210();
  iVar3 = 0;
  do {
    if (-1 < (int)(&this->mbr_0x1a4)[iVar3]) {
      puVar2 = (uint *)cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
      FUN_0040c960();
      uVar1 = this->mbr_0x18c;
      puVar2[10] = 0xbfc90fdb;
      puVar2[0x12] = uVar1;
      puVar2[0x11] = uVar1;
      puVar2[0x10] = uVar1;
      puVar2[0xb] = 0;
      puVar2[0xc] = 0;
      if (iVar3 == 0) {
        *puVar2 = *puVar2 | 0x90;
      }
      else {
        *puVar2 = *puVar2 | 0x9c;
        puVar2[4] = (&this->mbr_0xfc)[iVar3 * 3];
        puVar2[5] = (&this->mbr_0x100)[iVar3 * 3];
        puVar2[6] = (&this->mbr_0x104)[iVar3 * 3];
      }
      this->mbr_0x14 = (int)(&this->mbr_0x1a4)[iVar3] % (int)this->mbr_0x1bc;
      cls_0x5a486c::meth_0x40a8f0((cls_0x5a486c *)this->mbr_0x8);
      FUN_0040c9c0(&this->mbr_0x9c);
      if ((this->mbr_0x3c & 1) != 0) {
        FUN_0040ca20((int *)&this->mbr_0x9c);
      }
      this->mbr_0xec = 1;
      if ((this->mbr_0x3c & 2) != 0) {
        cls_0x5a486c::meth_0x40ca90((cls_0x5a486c *)this->mbr_0x8,this->mbr_0x4,this->mbr_0xc);
      }
      this->mbr_0xec = 1;
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 < 6);
  FUN_004de200();
  return 1;
}



// Function at 00513100

void __thiscall OOAnalyzer::cls_0x5b4814::virt_meth_0x513100(cls_0x5b4814 *this)

{
  (*this->vftptr_0x0->FUN_0046f250_24)(0);
  this->mbr_0xe0 = 0;
  this->mbr_0x184 = 0x8000;
  this->mbr_0x18c = 1;
  this->mbr_0x188 = 0x80000;
  this->mbr_0x194 = 1;
  return;
}



// Function at 00515050

cls_0x5a85ac * __thiscall
OOAnalyzer::cls_0x5b4814::virt_meth_0x515050(cls_0x5b4814 *this,undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  void *unaff_ESI;
  void *local_c;
  undefined4 uStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  uStack_8 = &LAB_005a0e6b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x198);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    local_4 = CONCAT31(local_4._1_3_,1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x5152e0_005b3e18;
    (*(code *)PTR_virt_meth_0x46f250_005b3e30)(0);
    (this_00->TScreen).mbr_0xe0 = 0;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x24 + 3) = 0x8000;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x28 + 3) = 0x100000;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x2c + 3) = 1;
    uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,2);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&cls_0x5b4814__vftable_5b4814_005b4814;
    (*cls_0x5b4814__vftable_5b4814_005b4814.FUN_0046f250_24)(0);
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x2c + 3) = 1;
    *(undefined4 *)&this_00[1].TScreen.mbr_0x37 = 1;
    (this_00->TScreen).mbr_0xe0 = 0;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x24 + 3) = 0x8000;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x28 + 3) = 0x80000;
    ExceptionList = unaff_ESI;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 00515150

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b4814::virt_meth_0x515150(cls_0x5b4814 *this)

{
  cls_0x5a47f0 *this_00;
  undefined4 *puVar1;
  dword dVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0ec2;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x4cc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4._0_1_ = 2;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x410ca0_005a370c;
    local_4._0_1_ = 3;
    this_00[9].mbr_0x2c = (dword)&cls_0x5b4308__vftable_5b4308_005b4308;
    this_00[0xb].mbr_0x38 = 0x28;
    puVar1 = FUN_00482fb0(0xb40);
    *(undefined4 **)&this_00[0xb].mbr_0x34 = puVar1;
    this_00[0xc].mbr_0x8 = (dword)&cls_0x5b42f0__vftable_5b42f0_005b42f0;
    this_00[0xe].mbr_0xc = 1;
    local_4 = CONCAT31(local_4._1_3_,5);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x5156c0_005b4a18;
    dVar2 = SoftwareRenderer::meth_0x4df070((SoftwareRenderer *)this_00->mbr_0x4);
    this_00[0xe].mbr_0x14 = dVar2;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00515620

cls_0x5b4814 * __thiscall OOAnalyzer::cls_0x5b4814::virt_meth_0x515620(cls_0x5b4814 *this)

{
  byte in_stack_00000004;
  
  cls_0x5b45ac::~cls_0x5b45ac((cls_0x5b45ac *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00515690

cls_0x5b4814 * __thiscall OOAnalyzer::cls_0x5b4814::virt_meth_0x515690(cls_0x5b4814 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b4814(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005156b0

cls_0x5b4814 * __thiscall OOAnalyzer::cls_0x5b4814::~cls_0x5b4814(cls_0x5b4814 *this)

{
  cls_0x5b4814 *pcVar1;
  
  this->vftptr_0x0 = &cls_0x5b4814__vftable_5b4814_005b4814;
  pcVar1 = (cls_0x5b4814 *)TScreen::~TScreen((TScreen *)this);
  return pcVar1;
}



