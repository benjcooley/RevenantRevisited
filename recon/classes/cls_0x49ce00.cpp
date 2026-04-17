// Decompiled methods and structure for class: cls_0x49ce00

/*
/OOAnalyzer/cls_0x49ce00
pack(disabled)
Structure cls_0x49ce00 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 0049ce00

cls_0x49ce00 * __thiscall OOAnalyzer::cls_0x49ce00::cls_0x49ce00(cls_0x49ce00 *this)

{
  byte *pbVar1;
  uint uVar2;
  char *in_stack_00000004;
  
  _strncpy(in_stack_00000004,(char *)((byte *)this->mbr_0x4 + 1),(uint)*(byte *)this->mbr_0x4);
  pbVar1 = (byte *)this->mbr_0x4;
  uVar2 = (uint)*pbVar1;
  in_stack_00000004[uVar2] = '\0';
  this->mbr_0x4 = (dword)(pbVar1 + uVar2 + 1);
  return this;
}



