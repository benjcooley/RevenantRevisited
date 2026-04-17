// Decompiled methods and structure for class: cls_0x4c58f0

/*
/OOAnalyzer/cls_0x4c58f0
pack(disabled)
Structure cls_0x4c58f0 {
   448   dword   4   mbr_0x1c0   "Unsigned Double-Word (ddw, 4-bytes)"
   452   dword   4   mbr_0x1c4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 456 Alignment: 1

*/

// Function at 004c58f0

undefined4 __thiscall OOAnalyzer::cls_0x4c58f0::meth_0x4c58f0(cls_0x4c58f0 *this)

{
  int iVar1;
  dword *pdVar2;
  int iVar3;
  dword in_stack_00000004;
  
  iVar3 = 0;
  pdVar2 = &this->mbr_0x1c4;
  do {
    if (pdVar2[-1] == in_stack_00000004) {
      iVar1 = cls_0x5a5320::meth_0x47e920((cls_0x5a5320 *)&DAT_0065caf0);
      if ((int)(iVar1 - *pdVar2) < 0x438) {
        return 1;
      }
    }
    iVar3 = iVar3 + 1;
    pdVar2 = pdVar2 + 3;
  } while (iVar3 < 8);
  return 0;
}



