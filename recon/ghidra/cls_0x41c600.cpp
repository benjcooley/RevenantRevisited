// Decompiled methods and structure for class: cls_0x41c600

/*
/OOAnalyzer/cls_0x41c600
pack(disabled)
Structure cls_0x41c600 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 24 Alignment: 1

*/

// Function at 0041c600

void __thiscall OOAnalyzer::cls_0x41c600::meth_0x41c600(cls_0x41c600 *this)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  
  iVar4 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      iVar1 = *(int *)(this->mbr_0x14 + iVar4 * 4);
      if ((*(uint *)(iVar1 + 0x44) & 2) != 0) {
        *(uint *)(iVar1 + 0x44) = *(uint *)(iVar1 + 0x44) & 0xfffffffd;
        if (*(int *)(iVar1 + 0x25c) != 0) {
          SoftwareRenderer::meth_0x4defe0(*(SoftwareRenderer **)(*(int *)(iVar1 + 0x25c) + 0x1c));
          iVar2 = *(int *)(iVar1 + 0x25c);
          *(undefined4 *)(iVar2 + 0x1c) = 0;
          *(undefined4 *)(iVar2 + 0x20) = 0;
        }
        if ((*(uint *)(iVar1 + 0x44) & 0x20) != 0) {
          if (DAT_0065abd8 == 0) {
            DAT_0065abd8 = -8;
          }
          else {
            DAT_0065abdc = 0xfffffff8;
          }
        }
        if ((*(uint *)(iVar1 + 0x44) & 0x40) != 0) {
          cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
          cls_0x41c7d0::meth_0x49bd90((cls_0x41c7d0 *)&DAT_00667548);
          uVar3 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
          cls_0x41c7d0::meth_0x49b8e0((cls_0x41c7d0 *)&DAT_00667548,uVar3);
        }
        if ((DAT_0066829c == 0) && ((*(byte *)(iVar1 + 0x44) & 0x10) != 0)) {
          cls_0x495cf0::meth_0x4967a0((cls_0x495cf0 *)&DAT_0065def0);
        }
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)this->mbr_0x4);
  }
  return;
}



