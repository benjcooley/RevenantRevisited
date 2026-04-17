// Decompiled methods and structure for class: cls_0x534a40

/*
/OOAnalyzer/cls_0x534a40
pack(disabled)
Structure cls_0x534a40 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 260 Alignment: 1

*/

// Function at 00534a40

void __thiscall OOAnalyzer::cls_0x534a40::meth_0x534a40(cls_0x534a40 *this)

{
  int iVar1;
  dword *pdVar2;
  
  iVar1 = 0;
  this->mbr_0x50 = 1;
  this->mbr_0x58 = 0;
  if (0 < (int)this->mbr_0x5c) {
    pdVar2 = &this->mbr_0x100;
    do {
      if (*pdVar2 != 0) {
        cls_0x4367d0::meth_0x4367d0((cls_0x4367d0 *)this->mbr_0x0);
        *pdVar2 = 0;
      }
      iVar1 = iVar1 + 1;
      pdVar2 = pdVar2 + 1;
    } while (iVar1 < (int)this->mbr_0x5c);
  }
  return;
}



