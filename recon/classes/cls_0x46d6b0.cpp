// Decompiled methods and structure for class: cls_0x46d6b0

/*
/OOAnalyzer/cls_0x46d6b0
pack(disabled)
Structure cls_0x46d6b0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   1028   dword   4   mbr_0x404   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 1032 Alignment: 1

*/

// Function at 0046d6b0

int __thiscall OOAnalyzer::cls_0x46d6b0::meth_0x46d6b0(cls_0x46d6b0 *this)

{
  int iVar1;
  dword *pdVar2;
  int iVar3;
  
  iVar3 = 0;
  if (0 < (int)this->mbr_0x0) {
    pdVar2 = &this->mbr_0x4;
    do {
      if (((*pdVar2 != 0) && (iVar1 = *pdVar2 + (int)pdVar2, iVar1 != 0)) &&
         (iVar1 = FUN_0059a530_stricmp(iVar1), iVar1 == 0)) {
        if ((&this->mbr_0x404)[iVar3] == 0) {
          return 0;
        }
        return (int)&this->mbr_0x404 + (&this->mbr_0x404)[iVar3] + iVar3 * 4;
      }
      iVar3 = iVar3 + 1;
      pdVar2 = pdVar2 + 1;
    } while (iVar3 < (int)this->mbr_0x0);
  }
  return 0;
}



// Function at 0046d710

int __thiscall OOAnalyzer::cls_0x46d6b0::meth_0x46d710(cls_0x46d6b0 *this)

{
  int iVar1;
  dword *pdVar2;
  int iVar3;
  undefined local_50 [80];
  
  iVar3 = 0;
  if (0 < (int)this->mbr_0x0) {
    pdVar2 = &this->mbr_0x4;
    do {
      if (((*pdVar2 != 0) && (iVar1 = *pdVar2 + (int)pdVar2, iVar1 != 0)) &&
         (iVar1 = FUN_0059a530_stricmp(iVar1), iVar1 == 0)) {
        if ((&this->mbr_0x404)[iVar3] != 0) {
          iVar3 = (int)&this->mbr_0x404 + (&this->mbr_0x404)[iVar3] + iVar3 * 4;
          if (iVar3 != 0) {
            return iVar3;
          }
          goto LAB_0046d769;
        }
        break;
      }
      iVar3 = iVar3 + 1;
      pdVar2 = pdVar2 + 1;
    } while (iVar3 < (int)this->mbr_0x0);
  }
  iVar3 = 0;
LAB_0046d769:
  FUN_0058b100(local_50,s_Unable_to_find___s__in_multireso_005d4718);
  FUN_00481c10(local_50,0);
  return iVar3;
}



