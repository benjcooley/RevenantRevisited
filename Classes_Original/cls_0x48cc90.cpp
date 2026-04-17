// Decompiled methods and structure for class: cls_0x48cc90

/*
/OOAnalyzer/cls_0x48cc90
pack(disabled)
Structure cls_0x48cc90 {
   360   dword   4   mbr_0x168   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 364 Alignment: 1

*/

// Function at 0048cc90

dword __thiscall OOAnalyzer::cls_0x48cc90::meth_0x48cc90(cls_0x48cc90 *this)

{
  int iVar1;
  dword in_stack_00000004;
  
  if (in_stack_00000004 == 0) {
    return in_stack_00000004;
  }
  iVar1 = in_stack_00000004 - 1;
  if (0x1c < iVar1) {
    iVar1 = 0x1d;
  }
  return (&this->mbr_0x168)[iVar1];
}



// Function at 0048ccb0

int __thiscall OOAnalyzer::cls_0x48cc90::meth_0x48ccb0(cls_0x48cc90 *this)

{
  int iVar1;
  dword *pdVar2;
  int in_stack_00000004;
  
  iVar1 = 0;
  pdVar2 = &this->mbr_0x168;
  do {
    if (in_stack_00000004 < (int)*pdVar2) {
      return iVar1;
    }
    iVar1 = iVar1 + 1;
    pdVar2 = pdVar2 + 1;
  } while (iVar1 < 0x1f);
  return 0x1e;
}



