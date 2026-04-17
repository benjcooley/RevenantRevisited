// Decompiled methods and structure for class: cls_0x4cdf30

/*
/OOAnalyzer/cls_0x4cdf30
pack(disabled)
Structure cls_0x4cdf30 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   276   dword   4   mbr_0x114   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 280 Alignment: 1

*/

// Function at 004cdf30

void __thiscall OOAnalyzer::cls_0x4cdf30::meth_0x4cdf30(cls_0x4cdf30 *this)

{
  (**(code **)(this->mbr_0x0 + 0x40))(this->mbr_0x8 | 0x100000);
  this->mbr_0x114 = *(dword *)(DAT_00667fd0 + 0x48);
  return;
}



