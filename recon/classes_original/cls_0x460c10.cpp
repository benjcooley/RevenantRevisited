// Decompiled methods and structure for class: cls_0x460c10

/*
/OOAnalyzer/cls_0x460c10
pack(disabled)
Structure cls_0x460c10 {
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 32 Alignment: 1

*/

// Function at 00460c10

void __thiscall OOAnalyzer::cls_0x460c10::meth_0x460c10(cls_0x460c10 *this)

{
  if (-1 < (int)this->mbr_0x1c) {
    if (DAT_0065b488 != 0) {
      cls_0x49ead0::meth_0x49eff0((cls_0x49ead0 *)&UNK_0065b2dc.field_0x1c);
    }
    DAT_0065b8f4 = 0;
    cls_0x49d2a0::meth_0x49d650((cls_0x49d2a0 *)&UNK_0065d1c0.field_0x310);
    cls_0x41c7d0::meth_0x49b400((cls_0x41c7d0 *)&DAT_00667548);
    this->mbr_0x1c = 0xffffffff;
  }
  return;
}



