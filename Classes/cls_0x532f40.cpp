// Decompiled methods and structure for class: cls_0x532f40

/*
/OOAnalyzer/cls_0x532f40
pack(disabled)
Structure cls_0x532f40 {
   404   word   2   mbr_0x194   "Unsigned Word (dw, 2-bytes)"
   406   word   2   mbr_0x196   "Unsigned Word (dw, 2-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 412 Alignment: 1

*/

// Function at 00532f40

void __thiscall OOAnalyzer::cls_0x532f40::meth_0x532f40(cls_0x532f40 *this)

{
  int iVar1;
  
  iVar1 = 0;
  if (0 < (short)this->mbr_0x194) {
    do {
      FUN_0052f310();
      iVar1 = iVar1 + 1;
    } while (iVar1 < (short)this->mbr_0x194);
  }
  if (this->mbr_0x198 != 0) {
    FUN_004830f0(this->mbr_0x198);
  }
  this->mbr_0x198 = 0;
  this->mbr_0x194 = 0;
  this->mbr_0x196 = 0;
  return;
}



