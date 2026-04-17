// Decompiled methods and structure for class: cls_0x49e350

/*
/OOAnalyzer/cls_0x49e350
pack(disabled)
Structure cls_0x49e350 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 0049e350

void __thiscall OOAnalyzer::cls_0x49e350::meth_0x49e350(cls_0x49e350 *this)

{
  if (this->mbr_0x0 != 0) {
    timeEndPeriod(DAT_005dacf4);
    timeKillEvent(this->mbr_0x0);
  }
  return;
}



