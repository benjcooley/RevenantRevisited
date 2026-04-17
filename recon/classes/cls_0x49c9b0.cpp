// Decompiled methods and structure for class: cls_0x49c9b0

/*
/OOAnalyzer/cls_0x49c9b0
pack(disabled)
Structure cls_0x49c9b0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 0049c9b0

void __thiscall OOAnalyzer::cls_0x49c9b0::meth_0x49c9b0(cls_0x49c9b0 *this)

{
  dword dVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar4;
  
  dVar1 = FUN_00482fb0(0x18);
  this->mbr_0x0 = dVar1;
  iVar4 = 0;
  do {
    puVar2 = (undefined4 *)FUN_00482fb0(0x7c);
    *(undefined4 **)(iVar4 + this->mbr_0x0) = puVar2;
    iVar4 = iVar4 + 4;
    for (iVar3 = 0x1f; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar2 = 0xffe17b80;
      puVar2 = puVar2 + 1;
    }
  } while (iVar4 < 0x18);
  return;
}



