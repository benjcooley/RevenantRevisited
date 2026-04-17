// Decompiled methods and structure for class: cls_0x4cefb0

/*
/OOAnalyzer/cls_0x4cefb0
pack(disabled)
Structure cls_0x4cefb0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   224   dword   4   mbr_0xe0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 228 Alignment: 1

*/

// Function at 004cefb0

undefined4 __thiscall OOAnalyzer::cls_0x4cefb0::meth_0x4cefb0(cls_0x4cefb0 *this)

{
  (**(code **)(this->mbr_0x0 + 0x40))(this->mbr_0x8 | 0x40);
  if (this->mbr_0xd8 != this->mbr_0xe0) {
    (**(code **)(this->mbr_0x0 + 0x218))(this->mbr_0xe0,0,0);
  }
  return 1;
}



