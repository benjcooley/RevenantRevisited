// Decompiled methods and structure for class: cls_0x51a5b0

/*
/OOAnalyzer/cls_0x51a5b0
pack(disabled)
Structure cls_0x51a5b0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 0051a5b0

int __thiscall OOAnalyzer::cls_0x51a5b0::meth_0x51a5b0(cls_0x51a5b0 *this)

{
  int iVar1;
  int iVar2;
  int unaff_ESI;
  
  (**(code **)(this->mbr_0x0 + 0xd4))(&PTR_LAB_005e2854);
  iVar1 = (**(code **)(this->mbr_0x0 + 0xd4))(s_Level_005e2858);
  iVar2 = unaff_ESI - iVar1;
  iVar1 = (iVar1 * 5 + 10) * 0x14;
  if (0 < iVar2) {
    return (iVar1 / 0xf) * iVar2;
  }
  if (iVar2 < -3) {
    return 1;
  }
  return iVar1 / (iVar2 * -0x1e + 0x1e);
}



