// Decompiled methods and structure for class: cls_0x53ed70

/*
/OOAnalyzer/cls_0x53ed70
pack(disabled)
Structure cls_0x53ed70 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 24 Alignment: 1

*/

// Function at 0053ed70

undefined4 __thiscall OOAnalyzer::cls_0x53ed70::meth_0x53ed70(cls_0x53ed70 *this)

{
  int *piVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  byte *in_stack_00000004;
  
  iVar5 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      iVar4 = 0;
      piVar1 = *(int **)(this->mbr_0x14 + iVar5 * 4);
      if (0 < *piVar1) {
        do {
          uVar3 = *(uint *)(piVar1[4] + iVar4 * 4);
          uVar2 = uVar3;
          if (uVar3 == 0) {
            uVar2 = piVar1[5];
          }
          uVar3 = FUN_0059a530_stricmp(uVar3,(byte *)(uVar2 + 0x24),in_stack_00000004);
          if (uVar3 == 0) {
            return *(undefined4 *)(this->mbr_0x14 + iVar5 * 4);
          }
          iVar4 = iVar4 + 1;
          piVar1 = *(int **)(this->mbr_0x14 + iVar5 * 4);
        } while (iVar4 < *piVar1);
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)this->mbr_0x4);
  }
  return 0;
}



// Function at 0053ef90

int __thiscall OOAnalyzer::cls_0x53ed70::meth_0x53ef90(cls_0x53ed70 *this)

{
  int *piVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  byte *in_stack_00000004;
  
  iVar5 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      iVar4 = 0;
      piVar1 = *(int **)(this->mbr_0x14 + iVar5 * 4);
      if (0 < *piVar1) {
        do {
          uVar3 = *(uint *)(piVar1[4] + iVar4 * 4);
          uVar2 = uVar3;
          if (uVar3 == 0) {
            uVar2 = piVar1[5];
          }
          uVar3 = FUN_0059a530_stricmp(uVar3,(byte *)(uVar2 + 0x24),in_stack_00000004);
          if (uVar3 == 0) {
            iVar5 = *(int *)(this->mbr_0x14 + iVar5 * 4);
            iVar4 = *(int *)(*(int *)(iVar5 + 0x10) + iVar4 * 4);
            if (iVar4 != 0) {
              return iVar4;
            }
            return *(int *)(iVar5 + 0x14);
          }
          iVar4 = iVar4 + 1;
          piVar1 = *(int **)(this->mbr_0x14 + iVar5 * 4);
        } while (iVar4 < *piVar1);
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)this->mbr_0x4);
  }
  return 0;
}



// Function at 0053f010

int __thiscall OOAnalyzer::cls_0x53ed70::meth_0x53f010(cls_0x53ed70 *this)

{
  int *piVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  byte *in_stack_00000004;
  
  iVar5 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      iVar4 = 0;
      piVar1 = *(int **)(this->mbr_0x14 + iVar5 * 4);
      if (0 < *piVar1) {
        do {
          uVar3 = *(uint *)(piVar1[4] + iVar4 * 4);
          uVar2 = uVar3;
          if (uVar3 == 0) {
            uVar2 = piVar1[5];
          }
          uVar3 = FUN_0059a530_stricmp(uVar3,(byte *)(uVar2 + 4),in_stack_00000004);
          if (uVar3 == 0) {
            iVar5 = *(int *)(this->mbr_0x14 + iVar5 * 4);
            iVar4 = *(int *)(*(int *)(iVar5 + 0x10) + iVar4 * 4);
            if (iVar4 != 0) {
              return iVar4;
            }
            return *(int *)(iVar5 + 0x14);
          }
          iVar4 = iVar4 + 1;
          piVar1 = *(int **)(this->mbr_0x14 + iVar5 * 4);
        } while (iVar4 < *piVar1);
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)this->mbr_0x4);
  }
  return 0;
}



