// Decompiled methods and structure for class: cls_0x502420

/*
/OOAnalyzer/cls_0x502420
pack(disabled)
Structure cls_0x502420 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 396 Alignment: 1

*/

// Function at 00502420

void __thiscall OOAnalyzer::cls_0x502420::meth_0x502420(cls_0x502420 *this)

{
  dword dVar1;
  
  (**(code **)(this->mbr_0x0 + 0x158))(0);
  this->mbr_0x184 = 0;
  this->mbr_0x188 = 0xf;
  dVar1 = __ftol();
  this->mbr_0x18 = dVar1;
  return;
}



