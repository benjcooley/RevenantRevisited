// Decompiled methods and structure for class: cls_0x573e90

/*
/OOAnalyzer/cls_0x573e90
pack(disabled)
Structure cls_0x573e90 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 00573e90

void __thiscall OOAnalyzer::cls_0x573e90::meth_0x573e90(cls_0x573e90 *this)

{
  dword dVar1;
  dword *pdVar2;
  
  dVar1 = this->mbr_0x0;
  if (dVar1 != 0) {
    pdVar2 = (dword *)this->mbr_0x4;
    this->mbr_0x0 = 0;
    *(dword **)(dVar1 + 4) = pdVar2;
    *pdVar2 = dVar1;
  }
  return;
}



