// Decompiled methods and structure for class: cls_0x51da00

/*
/OOAnalyzer/cls_0x51da00
pack(disabled)
Structure cls_0x51da00 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 0051da00

int __thiscall OOAnalyzer::cls_0x51da00::meth_0x51da00(cls_0x51da00 *this)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = 0;
  iVar3 = 0;
  do {
    iVar1 = (**(code **)(this->mbr_0x0 + 0xdc))(iVar3 + 0x22);
    iVar2 = iVar2 + iVar1;
    iVar3 = iVar3 + 1;
  } while (iVar3 < 6);
  return iVar2;
}



