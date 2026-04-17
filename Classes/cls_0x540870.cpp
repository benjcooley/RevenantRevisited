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
  int iVar3;
  uint uVar4;
  undefined4 in_stack_00000004;
  
  iVar3 = 0;
  uVar4 = 0;
  dVar1 = this->mbr_0x0;
  if (0 < (int)dVar1) {
    do {
      if ((this->mbr_0x10 != 0) && (uVar4 < this->mbr_0x0)) {
        iVar2 = *(int *)(this->mbr_0x10 + uVar4 * 4);
        if (iVar2 != 0) {
          iVar2 = FUN_0059a530(*(int *)(iVar2 + 0x120) + 4,in_stack_00000004);
          if (iVar2 == 0) {
            iVar3 = iVar3 + 1;
          }
        }
      }
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < (int)dVar1);
  }
  return iVar3;
}



