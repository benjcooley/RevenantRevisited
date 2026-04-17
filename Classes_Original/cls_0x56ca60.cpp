// Decompiled methods and structure for class: cls_0x56ca60

/*
/OOAnalyzer/cls_0x56ca60
pack(disabled)
Structure cls_0x56ca60 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 0056ca60

void __thiscall OOAnalyzer::cls_0x56ca60::meth_0x56ca60(cls_0x56ca60 *this)

{
  short *psVar1;
  int iVar2;
  int in_stack_00000004;
  
  if (this->mbr_0x0 != 0) {
    FUN_004830f0(this->mbr_0x0);
  }
  psVar1 = (short *)FUN_00482fb0(0x20000);
  this->mbr_0x0 = (dword)psVar1;
  if (in_stack_00000004 == 0x10000) {
    iVar2 = 0;
    do {
      *psVar1 = ((((ushort)(iVar2 >> 7) & 0x1e) * 0x20 + ((ushort)(iVar2 >> 3) & 0x1e)) * 0x20 +
                ((ushort)iVar2 & 0xf)) * 2;
      iVar2 = iVar2 + 1;
      psVar1 = psVar1 + 1;
    } while (iVar2 < 0x10000);
    return;
  }
  iVar2 = 0;
  do {
    *psVar1 = ((((ushort)(iVar2 >> 7) & 0x1e) * 0x20 + ((ushort)(iVar2 >> 3) & 0x1e)) * 0x10 +
              ((ushort)iVar2 & 0xf)) * 2;
    iVar2 = iVar2 + 1;
    psVar1 = psVar1 + 1;
  } while (iVar2 < 0x10000);
  return;
}



