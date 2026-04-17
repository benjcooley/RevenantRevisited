// Decompiled methods and structure for class: cls_0x4a1120

/*
/OOAnalyzer/cls_0x4a1120
pack(disabled)
Structure cls_0x4a1120 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 20 Alignment: 1

*/

// Function at 004a1120

int __thiscall OOAnalyzer::cls_0x4a1120::meth_0x4a1120(cls_0x4a1120 *this)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  if (0 < (int)this->mbr_0x0) {
    do {
      pbVar1 = *(byte **)(this->mbr_0x10 + iVar3 * 4);
      if ((((pbVar1 != (byte *)0x0) && (*(int *)(pbVar1 + 400) != 0)) && ((*pbVar1 & 1) != 0)) &&
         (iVar2 = FUN_0059a530(pbVar1 + 0x110), iVar2 == 0)) {
        return iVar3;
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)this->mbr_0x0);
  }
  return -1;
}



