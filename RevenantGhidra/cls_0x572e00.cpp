// Decompiled methods and structure for class: cls_0x572e00

/*
/OOAnalyzer/cls_0x572e00
pack(disabled)
Structure cls_0x572e00 {
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 44 Alignment: 1

*/

// Function at 00572e00

void __thiscall OOAnalyzer::cls_0x572e00::meth_0x572e00(cls_0x572e00 *this)

{
  DWORD DVar1;
  
  DVar1 = timeGetTime();
  this->mbr_0x28 = DVar1 + 5000;
  return;
}



