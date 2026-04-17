// Decompiled methods and structure for class: cls_0x49a2e0

/*
/OOAnalyzer/cls_0x49a2e0
pack(disabled)
Structure cls_0x49a2e0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 12 Alignment: 1

*/

// Function at 0049a2e0

void __thiscall OOAnalyzer::cls_0x49a2e0::meth_0x49a2e0(cls_0x49a2e0 *this)

{
  if (this->mbr_0x0 != 0) {
    _AIL_redbook_close_4(this->mbr_0x0);
  }
  this->mbr_0x0 = 0;
  return;
}



// Function at 0049a560

void __thiscall OOAnalyzer::cls_0x49a2e0::meth_0x49a560(cls_0x49a2e0 *this)

{
  int iVar1;
  
  if (this->mbr_0x0 != 0) {
    if (this->mbr_0x8 != 0) {
      iVar1 = _AIL_redbook_status_4(this->mbr_0x0);
      if (iVar1 == 1) {
        _AIL_redbook_stop_4(this->mbr_0x0);
      }
    }
    this->mbr_0x8 = 0;
  }
  return;
}



