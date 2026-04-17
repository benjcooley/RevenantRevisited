// Decompiled methods and structure for class: cls_0x4605c0

/*
/OOAnalyzer/cls_0x4605c0
pack(disabled)
Structure cls_0x4605c0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 24 Alignment: 1

*/

// Function at 004605c0

void __thiscall OOAnalyzer::cls_0x4605c0::meth_0x4605c0(cls_0x4605c0 *this)

{
  dword *this_00;
  LPCVOID pvVar1;
  int iVar2;
  
  this_00 = &this->mbr_0x4;
  iVar2 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      if ((-1 < iVar2) &&
         (pvVar1 = *(LPCVOID *)(this->mbr_0x14 + iVar2 * 4), pvVar1 != (LPCVOID)0x0)) {
        FUN_0045f8e0((int)pvVar1);
        FUN_004830f0(pvVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)*this_00);
  }
  *this_00 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x0 = 0;
  return;
}



