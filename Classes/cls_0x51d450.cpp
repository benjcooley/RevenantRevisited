// Decompiled methods and structure for class: cls_0x51d450

/*
/OOAnalyzer/cls_0x51d450
pack(disabled)
Structure cls_0x51d450 {
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   844   word   2   mbr_0x34c   "Unsigned Word (dw, 2-bytes)"
   848   dword   4   mbr_0x350   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 852 Alignment: 1

*/

// Function at 0051d450

void __thiscall OOAnalyzer::cls_0x51d450::meth_0x51d450(cls_0x51d450 *this,undefined4 param_1)

{
  int iVar1;
  undefined4 in_stack_00000008;
  
  iVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(this->mbr_0x48 + 0x1c));
  if (iVar1 < (short)this->mbr_0x34c) {
    *(undefined4 *)(this->mbr_0x350 + 4 + iVar1 * 8) = in_stack_00000008;
  }
  return;
}



