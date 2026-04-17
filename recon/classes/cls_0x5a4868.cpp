// Decompiled methods and structure for class: cls_0x5a4868

/*
/OOAnalyzer/cls_0x5a4868
pack(disabled)
Structure cls_0x5a4868 {
   0   cls_0x5a4868::vftable_5a4868 *   4   vftptr_0x0   "pointer to cls_0x5a4868::vftable_5a4868"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 004461e0

cls_0x5a4868 * __thiscall OOAnalyzer::cls_0x5a4868::cls_0x5a4868(cls_0x5a4868 *this)

{
  int iVar1;
  dword in_stack_00000004;
  
  iVar1 = DAT_006582c4;
  this->vftptr_0x0 = &cls_0x5a4868__vftable_5a4868_005a4868;
  if (iVar1 <= (int)in_stack_00000004) {
    DAT_006582c4 = in_stack_00000004 + 1;
  }
  *(cls_0x5a4868 **)(&DAT_006581fc.field_0x0 + in_stack_00000004 * 4) = this;
  this->mbr_0x4 = in_stack_00000004;
  return this;
}



