// Decompiled methods and structure for class: cls_0x51fec0

/*
/OOAnalyzer/cls_0x51fec0
pack(disabled)
Structure cls_0x51fec0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 20 Alignment: 1

*/

// Function at 0051fec0

void __thiscall OOAnalyzer::cls_0x51fec0::meth_0x51fec0(cls_0x51fec0 *this,undefined4 param_1)

{
  int *piVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)this->mbr_0x0) {
    do {
      piVar1 = *(int **)(this->mbr_0x10 + iVar2 * 4);
      if (piVar1 != (int *)0x0) {
        (**(code **)(*piVar1 + 0x10))(param_1);
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x0);
  }
  return;
}



