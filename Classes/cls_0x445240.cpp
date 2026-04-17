// Decompiled methods and structure for class: cls_0x445240

/*
/OOAnalyzer/cls_0x445240
pack(disabled)
Structure cls_0x445240 {
   1404   dword   4   mbr_0x57c   "Unsigned Double-Word (ddw, 4-bytes)"
   1412   dword   4   mbr_0x584   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 1416 Alignment: 1

*/

// Function at 00445240

undefined4 __thiscall OOAnalyzer::cls_0x445240::meth_0x445240(cls_0x445240 *this)

{
  dword dVar1;
  dword dVar2;
  
  dVar1 = this->mbr_0x584;
  dVar2 = dVar1 + 1;
  this->mbr_0x584 = dVar2;
  if ((int)dVar2 < (int)this->mbr_0x57c) {
    return *(undefined4 *)(&this->field_0x180 + dVar1 * 4);
  }
  return 0xffffffff;
}



