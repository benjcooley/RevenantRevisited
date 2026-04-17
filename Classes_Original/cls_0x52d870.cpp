// Decompiled methods and structure for class: cls_0x52d870

/*
/OOAnalyzer/cls_0x52d870
pack(disabled)
Structure cls_0x52d870 {
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 24 Alignment: 1

*/

// Function at 0052d870

int __thiscall OOAnalyzer::cls_0x52d870::meth_0x52d870(cls_0x52d870 *this)

{
  dword dVar1;
  dword dVar2;
  int in_stack_00000004;
  
  dVar2 = *(dword *)(this->mbr_0x10 + in_stack_00000004 * 4);
  dVar1 = dVar2;
  if (dVar2 == 0) {
    dVar1 = this->mbr_0x14;
    dVar2 = this->mbr_0x14;
  }
  return *(int *)(dVar2 + 0x54) + *(int *)(dVar1 + 0x50);
}



