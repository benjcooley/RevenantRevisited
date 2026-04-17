// Decompiled methods and structure for class: cls_0x415a00

/*
/OOAnalyzer/cls_0x415a00
pack(disabled)
Structure cls_0x415a00 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 00415a00

undefined4 __thiscall OOAnalyzer::cls_0x415a00::meth_0x415a00(cls_0x415a00 *this,int param_1)

{
  int iVar1;
  undefined4 in_stack_00000008;
  
  if ((((this->mbr_0x0 != 0) && (-1 < param_1)) && (param_1 < DAT_005e8860)) &&
     (iVar1 = *(int *)(DAT_005e8870 + param_1 * 4), iVar1 != 0)) {
    *(undefined4 *)(iVar1 + 0x5c) = in_stack_00000008;
    return 1;
  }
  return 0;
}



