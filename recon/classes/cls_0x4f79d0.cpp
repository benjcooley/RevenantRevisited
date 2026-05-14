// Decompiled methods and structure for class: cls_0x4f79d0

/*
/OOAnalyzer/cls_0x4f79d0
pack(disabled)
Structure cls_0x4f79d0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 404 Alignment: 1

*/

// Function at 004f79d0

cls_0x4f79d0 * __thiscall OOAnalyzer::cls_0x4f79d0::~cls_0x4f79d0(cls_0x4f79d0 *this)

{
  cls_0x4f79d0 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f788;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x4f79b0_005aa89c;
  local_4 = 0;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (this->mbr_0x18c != 0) {
    FUN_004830f0(this->mbr_0x18c);
  }
  if (this->mbr_0x190 != 0) {
    FUN_004830f0(this->mbr_0x190);
  }
  if (this->mbr_0x188 != 0) {
    FUN_004830f0(this->mbr_0x188);
  }
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x4f79d0 *)cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  ExceptionList = local_c;
  return pcVar1;
}



