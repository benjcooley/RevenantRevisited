// Decompiled methods and structure for class: cls_0x49c970

/*
/OOAnalyzer/cls_0x49c970
pack(disabled)
Structure cls_0x49c970 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 0049c970

void __thiscall OOAnalyzer::cls_0x49c970::meth_0x49c970(cls_0x49c970 *this)

{
  LPCVOID pvVar1;
  int iVar2;
  
  if (this->mbr_0x0 != 0) {
    iVar2 = 0;
    do {
      pvVar1 = *(LPCVOID *)(iVar2 + this->mbr_0x0);
      if (pvVar1 != (LPCVOID)0x0) {
        FUN_004830f0(pvVar1);
      }
      iVar2 = iVar2 + 4;
    } while (iVar2 < 0x18);
    FUN_004830f0((LPCVOID)this->mbr_0x0);
  }
  return;
}



