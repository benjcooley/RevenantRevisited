// Decompiled methods and structure for class: cls_0x495c20

/*
/OOAnalyzer/cls_0x495c20
pack(disabled)
Structure cls_0x495c20 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 64 Alignment: 1

*/

// Function at 00495c20

undefined4 __thiscall OOAnalyzer::cls_0x495c20::meth_0x495c20(cls_0x495c20 *this)

{
  byte bVar1;
  byte *pbVar2;
  int iVar3;
  bool bVar4;
  byte *in_stack_00000004;
  
  if ((this->mbr_0x3c != 0) && (in_stack_00000004 != (byte *)0x0)) {
    pbVar2 = (byte *)this->mbr_0x0;
    do {
      bVar1 = *pbVar2;
      bVar4 = bVar1 < *in_stack_00000004;
      if (bVar1 != *in_stack_00000004) {
LAB_00495c5b:
        iVar3 = (1 - (uint)bVar4) - (uint)(bVar4 != 0);
        goto LAB_00495c60;
      }
      if (bVar1 == 0) break;
      bVar1 = pbVar2[1];
      bVar4 = bVar1 < in_stack_00000004[1];
      if (bVar1 != in_stack_00000004[1]) goto LAB_00495c5b;
      pbVar2 = pbVar2 + 2;
      in_stack_00000004 = in_stack_00000004 + 2;
    } while (bVar1 != 0);
    iVar3 = 0;
LAB_00495c60:
    if (iVar3 == 0) {
      return 1;
    }
  }
  return 0;
}



