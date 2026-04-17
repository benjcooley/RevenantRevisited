// Decompiled methods and structure for class: cls_0x50ad90

/*
/OOAnalyzer/cls_0x50ad90
pack(disabled)
Structure cls_0x50ad90 {
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   132   dword   4   mbr_0x84   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 136 Alignment: 1

*/

// Function at 0050ad90

int __thiscall OOAnalyzer::cls_0x50ad90::meth_0x50ad90(cls_0x50ad90 *this)

{
  int iVar1;
  int *piVar2;
  dword dVar3;
  
  dVar3 = this->mbr_0x84;
  iVar1 = 0;
  if (0 < (int)dVar3) {
    piVar2 = (int *)this->mbr_0x80;
    do {
      if (*piVar2 != 0) {
        iVar1 = iVar1 + 1;
      }
      piVar2 = piVar2 + 0x12;
      dVar3 = dVar3 - 1;
    } while (dVar3 != 0);
  }
  return iVar1;
}



