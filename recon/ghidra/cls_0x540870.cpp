// Decompiled methods and structure for class: cls_0x540870

/*
/OOAnalyzer/cls_0x540870
pack(disabled)
Structure cls_0x540870 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 20 Alignment: 1

*/

// Function at 00540870

int __thiscall OOAnalyzer::cls_0x540870::meth_0x540870(cls_0x540870 *this)

{
  dword dVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint in_stack_00000004;
  
  iVar4 = 0;
  uVar5 = 0;
  dVar1 = this->mbr_0x0;
  if (0 < (int)dVar1) {
    do {
      if ((this->mbr_0x10 != 0) && (uVar5 < this->mbr_0x0)) {
        iVar2 = *(int *)(this->mbr_0x10 + uVar5 * 4);
        if (iVar2 != 0) {
          uVar3 = FUN_0059a530(in_stack_00000004,(byte *)(*(int *)(iVar2 + 0x120) + 4),
                               (byte *)in_stack_00000004);
          if (uVar3 == 0) {
            iVar4 = iVar4 + 1;
          }
        }
      }
      uVar5 = uVar5 + 1;
    } while ((int)uVar5 < (int)dVar1);
  }
  return iVar4;
}



