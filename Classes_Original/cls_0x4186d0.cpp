// Decompiled methods and structure for class: cls_0x4186d0

/*
/OOAnalyzer/cls_0x4186d0
pack(disabled)
Structure cls_0x4186d0 {
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 004186d0

int __thiscall OOAnalyzer::cls_0x4186d0::meth_0x4186d0(cls_0x4186d0 *this)

{
  int *piVar1;
  int iVar2;
  int in_stack_00000004;
  
  if (in_stack_00000004 < (int)this->mbr_0xc) {
    piVar1 = (int *)((int)this + in_stack_00000004 * 0x28 + 0x38);
    iVar2 = *piVar1;
    if (iVar2 != 0) {
      return (int)piVar1 + iVar2;
    }
  }
  return 0;
}



