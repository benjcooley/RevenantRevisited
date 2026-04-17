// Decompiled methods and structure for class: cls_0x5a50e4

/*
/OOAnalyzer/cls_0x5a50e4
pack(disabled)
Structure cls_0x5a50e4 {
   0   cls_0x5a50e4::vftable_5a50e4 *   4   vftptr_0x0   "pointer to cls_0x5a50e4::vftable_5a50e4"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 0046df00

cls_0x5a50e4 * __thiscall OOAnalyzer::cls_0x5a50e4::cls_0x5a50e4(cls_0x5a50e4 *this)

{
  int iVar1;
  dword in_stack_00000004;
  
  iVar1 = DAT_0065a248;
  this->vftptr_0x0 = &cls_0x5a50e4__vftable_5a50e4_005a50e4;
  if (iVar1 < 0x100) {
    (&DAT_00659d48)[iVar1] = this;
    DAT_0065a248 = iVar1 + 1;
    this->mbr_0x4 = in_stack_00000004;
    return this;
  }
  this->mbr_0x4 = in_stack_00000004;
  return this;
}



