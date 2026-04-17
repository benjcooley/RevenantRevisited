// Decompiled methods and structure for class: cls_0x499710

/*
/OOAnalyzer/cls_0x499710
pack(disabled)
Structure cls_0x499710 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 00499710

void __thiscall OOAnalyzer::cls_0x499710::meth_0x499710(cls_0x499710 *this)

{
  int iVar1;
  undefined4 *puVar2;
  
  puVar2 = (undefined4 *)this->mbr_0x0;
  for (iVar1 = 0x800; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  return;
}



