// Decompiled methods and structure for class: cls_0x572850

/*
/OOAnalyzer/cls_0x572850
pack(disabled)
Structure cls_0x572850 {
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 60 Alignment: 1

*/

// Function at 00572850

void __thiscall OOAnalyzer::cls_0x572850::meth_0x572850(cls_0x572850 *this)

{
  byte bVar1;
  byte bVar2;
  byte *pbVar3;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) && (in_stack_00000004 < (int)this->mbr_0x1c)) {
    pbVar3 = (byte *)(this->mbr_0x38 +
                     ((int)(in_stack_00000004 + (in_stack_00000004 >> 0x1f & 7U)) >> 3));
    bVar2 = '\x01' << ((byte)in_stack_00000004 & 7);
    bVar1 = *pbVar3;
    if ((bVar2 & bVar1) != 0) {
      this->mbr_0x20 = this->mbr_0x20 - 1;
    }
    *pbVar3 = ~bVar2 & bVar1;
  }
  return;
}



