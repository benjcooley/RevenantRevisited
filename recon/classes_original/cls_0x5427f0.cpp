// Decompiled methods and structure for class: cls_0x5427f0

/*
/OOAnalyzer/cls_0x5427f0
pack(disabled)
Structure cls_0x5427f0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   328   dword   4   mbr_0x148   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 332 Alignment: 1

*/

// Function at 005427f0

void __thiscall OOAnalyzer::cls_0x5427f0::meth_0x5427f0(cls_0x5427f0 *this)

{
  if (DAT_00667fcc != (cls_0x5a7b98_TCharacter *)0x0) {
    cls_0x5a7b98_TCharacter::meth_0x51b580(DAT_00667fcc,this->mbr_0x148);
    (**(code **)(this->mbr_0x0 + 0x1c))(this->mbr_0x14 | 0x20);
  }
  return;
}



