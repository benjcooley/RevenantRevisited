// Decompiled methods and structure for class: cls_0x499720

/*
/OOAnalyzer/cls_0x499720
pack(disabled)
Structure cls_0x499720 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 00499720

ushort __thiscall OOAnalyzer::cls_0x499720::meth_0x499720_TSector_ReturnWalkmap(cls_0x499720 *this,int param_1)

{
  int in_stack_00000008;
  
  if (this->mbr_0x0 != 0) {
    return *(ushort *)(this->mbr_0x0 + (in_stack_00000008 * 0x40 + param_1) * 2) & 0x3ff;
  }
  return 0;
}



// Function at 00499750

void __thiscall OOAnalyzer::cls_0x499720::meth_0x499750_TSector_SetWalkmap(cls_0x499720 *this,int param_1,int param_2)

{
  ushort uVar1;
  dword dVar2;
  int iVar3;
  ushort in_stack_0000000c;
  
  dVar2 = this->mbr_0x0;
  if (dVar2 != 0) {
    iVar3 = param_2 * 0x40 + param_1;
    uVar1 = *(ushort *)(dVar2 + iVar3 * 2);
    *(ushort *)(dVar2 + iVar3 * 2) = (in_stack_0000000c ^ uVar1) & 0x3ff ^ uVar1;
  }
  return;
}



