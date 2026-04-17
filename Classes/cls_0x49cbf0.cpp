// Decompiled methods and structure for class: cls_0x49cbf0

/*
/OOAnalyzer/cls_0x49cbf0
pack(disabled)
Structure cls_0x49cbf0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 0049cbf0

undefined4 __thiscall OOAnalyzer::cls_0x49cbf0::meth_0x49cbf0(cls_0x49cbf0 *this,int param_1)

{
  int in_stack_00000008;
  
  if (((this->mbr_0x0 != 0) && (param_1 < 6)) && (in_stack_00000008 < 0x1f)) {
    return *(undefined4 *)(*(int *)(this->mbr_0x0 + param_1 * 4) + in_stack_00000008 * 4);
  }
  return 0;
}



