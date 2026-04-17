// Decompiled methods and structure for class: cls_0x5a4e24

/*
/OOAnalyzer/cls_0x5a4e24
pack(disabled)
Structure cls_0x5a4e24 {
   0   cls_0x5a4e24::vftable_5a4e24 *   4   vftptr_0x0   "pointer to cls_0x5a4e24::vftable_5a4e24"
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
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   384   dword   4   mbr_0x180   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 396 Alignment: 1

*/

// Function at 00469370

undefined4 __thiscall OOAnalyzer::cls_0x5a4e24::virt_meth_0x469370(cls_0x5a4e24 *this)

{
  int iVar1;
  
  iVar1 = cls_0x5b93c4::virt_meth_0x435150
                    ((cls_0x5b93c4 *)this,s_selstart_005d3bc4,0x659cb8,0x11,0x7e,0x41,0x18a,0x13c,
                     0x15c,0x54,s_widgets_005d3bbc);
  if (iVar1 == 0) {
    return 0;
  }
  this->mbr_0x180 = 0;
  this->mbr_0x17c = 0;
  this->mbr_0x184 = 0;
  this->mbr_0x188 = 0;
  return 1;
}



// Function at 004693e0

void __thiscall OOAnalyzer::cls_0x5a4e24::virt_meth_0x4693e0(cls_0x5a4e24 *this,int param_1)

{
  cls_0x5a3d44 *this_00;
  int in_stack_00000008;
  
  cls_0x5b98b8::meth_0x436930((cls_0x5b98b8 *)this);
  this_00 = (cls_0x5a3d44 *)cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  if (this_00 != (cls_0x5a3d44 *)0x0) {
    if (param_1 == 0x26) {
      if (in_stack_00000008 != 0) {
        cls_0x5a3d44::meth_0x430b80(this_00);
        return;
      }
    }
    else if ((param_1 == 0x28) && (in_stack_00000008 != 0)) {
      cls_0x5a3d44::meth_0x430b80(this_00);
      return;
    }
  }
  cls_0x5b93c4::virt_meth_0x4361f0((cls_0x5b93c4 *)this,param_1);
  return;
}



// Function at 004697a0

undefined4 __thiscall
OOAnalyzer::cls_0x5a4e24::virt_meth_0x4697a0
          (cls_0x5a4e24 *this,byte *param_1,char *param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  int in_stack_00000014;
  
  *param_2 = '\0';
  uVar1 = FUN_0059a530((uint)this,param_1,(byte *)s_startpos_005d3c74);
  if (uVar1 != 0) {
    uVar3 = cls_0x5b93c4::virt_meth_0x436ea0((cls_0x5b93c4 *)this);
    return uVar3;
  }
  iVar2 = cls_0x4609f0::meth_0x460ca0((cls_0x4609f0 *)&UNK_0065a630.field_0x138,DAT_00658da0);
  if (in_stack_00000014 < *(int *)(iVar2 + 0x88)) {
    cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    FUN_0058b100(param_2,(byte *)s__s___s__005d3c88);
    return 1;
  }
  cls_0x45f7c0::meth_0x51fdd0((cls_0x45f7c0 *)&DAT_0065a890,(char)DAT_00667fcc,0);
  cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  FUN_0058b100(param_2,(byte *)s__s___s__005d3c9c);
  return 1;
}



// Function at 00469890

void __thiscall OOAnalyzer::cls_0x5a4e24::virt_meth_0x469890(cls_0x5a4e24 *this)

{
  cls_0x5a4e24__vftable_5a4e24 *pcVar1;
  
  cls_0x5b93c4::virt_meth_0x435d70((cls_0x5b93c4 *)this);
  if (((DAT_00668128 != 0) && (this->mbr_0x188 != 0)) && (this->mbr_0x184 == 0)) {
    this->mbr_0x188 = 0;
    this->mbr_0x184 = 1;
    FUN_00483300(0,*(int *)(this->mbr_0x17c + 0x98) + -1);
    cls_0x5a3d44::meth_0x430b80((cls_0x5a3d44 *)this->mbr_0x17c);
    pcVar1 = this->vftptr_0x0;
    cls_0x5b98b8::meth_0x436980((cls_0x5b98b8 *)this);
    (*pcVar1->virt_meth_0x469460_148)(this);
  }
  return;
}



// Function at 0046d250

cls_0x5a4e24 * __thiscall OOAnalyzer::cls_0x5a4e24::cls_0x5a4e24(cls_0x5a4e24 *this)

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->vftptr_0x0 = (cls_0x5a4e24__vftable_5a4e24 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a4e24__vftable_5a4e24_005a4e24;
  return this;
}



// Function at 0046d2b0

cls_0x5a4e24 * __thiscall OOAnalyzer::cls_0x5a4e24::virt_meth_0x46d2b0(cls_0x5a4e24 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a4e24(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0046d2d0

cls_0x5a4e24 * __thiscall OOAnalyzer::cls_0x5a4e24::~cls_0x5a4e24(cls_0x5a4e24 *this)

{
  cls_0x5a4e24 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d1be;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a4e24__vftable_5a4e24 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a4e24 *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



