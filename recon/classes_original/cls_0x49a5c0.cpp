// Decompiled methods and structure for class: cls_0x49a5c0

/*
/OOAnalyzer/cls_0x49a5c0
pack(disabled)
Structure cls_0x49a5c0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 0049a5c0

void __thiscall OOAnalyzer::cls_0x49a5c0::meth_0x49a5c0(cls_0x49a5c0 *this)

{
  dword dVar1;
  int in_stack_00000004;
  
  if (this->mbr_0x0 != 0) {
    if (in_stack_00000004 < 0x60) {
      if (in_stack_00000004 < 0) {
        in_stack_00000004 = 0;
      }
    }
    else {
      in_stack_00000004 = 0x60;
    }
    if (this->mbr_0x8 != 0) {
      _AIL_redbook_set_volume_8(this->mbr_0x0,in_stack_00000004);
      dVar1 = _AIL_redbook_volume_4(this->mbr_0x0);
      this->mbr_0x4 = dVar1;
      this->mbr_0xc = dVar1;
    }
  }
  return;
}



