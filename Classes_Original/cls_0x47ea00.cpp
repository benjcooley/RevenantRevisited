// Decompiled methods and structure for class: cls_0x47ea00

/*
/OOAnalyzer/cls_0x47ea00
pack(disabled)
Structure cls_0x47ea00 {
   1680   dword   4   mbr_0x690   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 1684 Alignment: 1

*/

// Function at 0047ea00

int __thiscall OOAnalyzer::cls_0x47ea00::meth_0x47ea00(cls_0x47ea00 *this)

{
  int iVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  
  dVar2 = this->mbr_0x690;
  iVar4 = 0x438 - (DAT_0065d808 >> 1);
  iVar3 = 0x168 - (DAT_0065d808 >> 1);
  iVar1 = DAT_0065d808 + -1 + iVar3;
  if ((DAT_0065d808 + -1 + iVar4 < (int)dVar2) || ((int)dVar2 < iVar3)) {
    return 0;
  }
  if (iVar1 < (int)dVar2) {
    if ((int)dVar2 < iVar4) {
      return 0xff;
    }
    if (iVar1 < (int)dVar2) {
      if (DAT_005d7a18 != 0) {
        return (int)(((DAT_0065d808 - dVar2) + iVar4) * 0xff) / DAT_0065d808;
      }
      iVar3 = (DAT_0065d808 - dVar2) + iVar4;
      goto LAB_0047ea63;
    }
  }
  if (DAT_005d7a18 != 0) {
    return (int)((dVar2 - iVar3) * 0xff) / DAT_0065d808;
  }
  iVar3 = dVar2 - iVar3;
LAB_0047ea63:
  return (((iVar3 * DAT_0065d80c) / DAT_0065d808) * 0xff) / DAT_0065d80c;
}



// Function at 0047eab0

int __thiscall OOAnalyzer::cls_0x47ea00::meth_0x47eab0(cls_0x47ea00 *this)

{
  int iVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  
  dVar2 = this->mbr_0x690;
  iVar3 = 0x168 - (DAT_0065d808 >> 1);
  iVar4 = 0x438 - (DAT_0065d808 >> 1);
  iVar1 = DAT_0065d808 + -1 + iVar3;
  if (dVar2 == 0) {
    return 0;
  }
  if (dVar2 == 0x2d0) {
    return 3;
  }
  if ((DAT_0065d808 + -1 + iVar4 < (int)dVar2) || ((int)dVar2 < iVar3)) {
    return 5;
  }
  if ((iVar1 < (int)dVar2) && ((int)dVar2 < iVar4)) {
    return 2;
  }
  return ((iVar1 < (int)dVar2) - 1 & 0xfffffffd) + 4;
}



