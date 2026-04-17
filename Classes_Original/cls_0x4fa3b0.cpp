// Decompiled methods and structure for class: cls_0x4fa3b0

/*
/OOAnalyzer/cls_0x4fa3b0
pack(disabled)
Structure cls_0x4fa3b0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   420   dword   4   mbr_0x1a4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 424 Alignment: 1

*/

// Function at 004fa3b0

cls_0x4fa3b0 * __thiscall OOAnalyzer::cls_0x4fa3b0::~cls_0x4fa3b0(cls_0x4fa3b0 *this)

{
  cls_0x4fa3b0 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0018;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x4fa390_005ada3c;
  local_4 = 0;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30::meth_0x46e630((cls_0x5b4f30 *)this);
  }
  if (0 < (int)this->mbr_0x1a4) {
    cls_0x41c7d0::meth_0x49bd90((cls_0x41c7d0 *)&DAT_00667548);
    cls_0x41c7d0::meth_0x49b8e0((cls_0x41c7d0 *)&DAT_00667548,this->mbr_0x1a4);
  }
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x4fa3b0 *)cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  ExceptionList = local_c;
  return pcVar1;
}



