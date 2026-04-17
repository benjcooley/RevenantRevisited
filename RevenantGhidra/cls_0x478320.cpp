// Decompiled methods and structure for class: cls_0x478320

/*
/OOAnalyzer/cls_0x478320
pack(disabled)
Structure cls_0x478320 {
   0   word   2   mbr_0x0   "Unsigned Word (dw, 2-bytes)"
   2   word   2   mbr_0x2   "Unsigned Word (dw, 2-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 00478320

cls_0x478320 * __thiscall OOAnalyzer::cls_0x478320::cls_0x478320(cls_0x478320 *this)

{
  this->mbr_0x0 = 0;
  this->mbr_0x2 = 0;
  this->mbr_0x4 = 0;
  return this;
}



// Function at 00478330

void __thiscall OOAnalyzer::cls_0x478320::meth_0x478330(cls_0x478320 *this)

{
  if ((LPCVOID)this->mbr_0x4 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x4);
  }
  return;
}



// Function at 00478340

void __thiscall OOAnalyzer::cls_0x478320::meth_0x478340(cls_0x478320 *this)

{
  if ((LPCVOID)this->mbr_0x4 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x4);
  }
  this->mbr_0x4 = 0;
  this->mbr_0x0 = 0;
  this->mbr_0x2 = 0;
  return;
}



