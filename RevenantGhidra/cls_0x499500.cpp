// Decompiled methods and structure for class: cls_0x499500

/*
/OOAnalyzer/cls_0x499500
pack(disabled)
Structure cls_0x499500 {
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   200   dword   4   mbr_0xc8   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 276 Alignment: 1

*/

// Function at 00499500

void __thiscall OOAnalyzer::cls_0x499500::meth_0x499500(cls_0x499500 *this)

{
  int *piVar1;
  int iVar2;
  
  if ((this->mbr_0x98 == 0) && (this->mbr_0xac == 0)) {
    for (iVar2 = 0; iVar2 < (int)this->mbr_0xfc; iVar2 = iVar2 + 1) {
      piVar1 = *(int **)(this->mbr_0x10c + iVar2 * 4);
      if (piVar1 == (int *)0x0) {
        piVar1 = (int *)this->mbr_0x110;
      }
      if (*(int *)(this->mbr_0xc8 + *piVar1 * 4) != 0) {
        FUN_00471b40();
      }
    }
    this->mbr_0xac = 1;
  }
  return;
}



