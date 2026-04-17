// Decompiled methods and structure for class: cls_0x587f80

/*
/OOAnalyzer/cls_0x587f80
pack(disabled)
Structure cls_0x587f80 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 00587f80

void __thiscall OOAnalyzer::cls_0x587f80::meth_0x587f80(cls_0x587f80 *this)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  
  piVar1 = (int *)this->mbr_0x0;
  iVar3 = *piVar1;
  while (iVar3 != 0) {
    iVar3 = *piVar1;
    if (iVar3 != 0) {
      piVar2 = (int *)piVar1[1];
      *piVar1 = 0;
      *(int **)(iVar3 + 4) = piVar2;
      *piVar2 = iVar3;
    }
    iVar3 = *piVar1;
  }
  return;
}



