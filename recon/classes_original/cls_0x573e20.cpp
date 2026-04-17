// Decompiled methods and structure for class: cls_0x573e20

/*
/OOAnalyzer/cls_0x573e20
pack(disabled)
Structure cls_0x573e20 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 00573e20

void __thiscall OOAnalyzer::cls_0x573e20::meth_0x573e20(cls_0x573e20 *this)

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



