// Decompiled methods and structure for class: cls_0x5b9480

/*
/OOAnalyzer/cls_0x5b9480
pack(disabled)
Structure cls_0x5b9480 {
   0   cls_0x5b9480::vftable_5b9480 *   4   vftptr_0x0   "pointer to cls_0x5b9480::vftable_5b9480"
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
}
Length: 160 Alignment: 1

*/

// Function at 00537110

int __thiscall OOAnalyzer::cls_0x5b9480::virt_meth_0x537110(cls_0x5b9480 *this)

{
  char *pcVar1;
  int iVar2;
  
  pcVar1 = s_mpingamemenu_005e4070;
  if (DAT_0066829c == 0) {
    pcVar1 = s_ingamemenu_005e4080;
  }
  iVar2 = cls_0x5b93c4::virt_meth_0x435150
                    ((cls_0x5b93c4 *)this,s_ingamemenu_005e40a0,pcVar1,0x11,0x7e,0x41,0x18a,0x13c,
                     200,0x50,s_widgets_005e4098);
  if (iVar2 != 0) {
    cls_0x49c830::meth_0x49c830((cls_0x49c830 *)&DAT_00667548);
  }
  return iVar2;
}



// Function at 00537170

void __thiscall OOAnalyzer::cls_0x5b9480::virt_meth_0x537170(cls_0x5b9480 *this)

{
  cls_0x49c830::meth_0x49c890((cls_0x49c830 *)&DAT_00667548);
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  return;
}



// Function at 00537420

cls_0x5b9480 * __thiscall OOAnalyzer::cls_0x5b9480::cls_0x5b9480(cls_0x5b9480 *this)

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->vftptr_0x0 = (cls_0x5b9480__vftable_5b9480 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5b9480__vftable_5b9480_005b9480;
  return this;
}



// Function at 00537480

cls_0x5b9480 * __thiscall OOAnalyzer::cls_0x5b9480::virt_meth_0x537480(cls_0x5b9480 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b9480(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005374a0

cls_0x5b9480 * __thiscall OOAnalyzer::cls_0x5b9480::~cls_0x5b9480(cls_0x5b9480 *this)

{
  cls_0x5b9480 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1abe;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5b9480__vftable_5b9480 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5b9480 *)FUN_004830f0(*(undefined4 *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



