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
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      iVar3 = 0;
      piVar1 = *(int **)(this->mbr_0x14 + iVar4 * 4);
      if (0 < *piVar1) {
        do {
          iVar2 = *(int *)(piVar1[4] + iVar3 * 4);
          if (iVar2 == 0) {
            iVar2 = piVar1[5];
          }
          iVar2 = FUN_0059a530(iVar2 + 0x24);
          if (iVar2 == 0) {
            return *(undefined4 *)(this->mbr_0x14 + iVar4 * 4);
          }
          iVar3 = iVar3 + 1;
          piVar1 = *(int **)(this->mbr_0x14 + iVar4 * 4);
        } while (iVar3 < *piVar1);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)this->mbr_0x4);
  }
  return 0;
}



// Function at 0053ef90

int __thiscall OOAnalyzer::cls_0x53ed70::meth_0x53ef90(cls_0x53ed70 *this)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      iVar3 = 0;
      piVar1 = *(int **)(this->mbr_0x14 + iVar4 * 4);
      if (0 < *piVar1) {
        do {
          iVar2 = *(int *)(piVar1[4] + iVar3 * 4);
          if (iVar2 == 0) {
            iVar2 = piVar1[5];
          }
          iVar2 = FUN_0059a530(iVar2 + 0x24);
          if (iVar2 == 0) {
            iVar4 = *(int *)(this->mbr_0x14 + iVar4 * 4);
            iVar3 = *(int *)(*(int *)(iVar4 + 0x10) + iVar3 * 4);
            if (iVar3 != 0) {
              return iVar3;
            }
            return *(int *)(iVar4 + 0x14);
          }
          iVar3 = iVar3 + 1;
          piVar1 = *(int **)(this->mbr_0x14 + iVar4 * 4);
        } while (iVar3 < *piVar1);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)this->mbr_0x4);
  }
  return 0;
}



// Function at 0053f010

int __thiscall OOAnalyzer::cls_0x53ed70::meth_0x53f010(cls_0x53ed70 *this)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      iVar3 = 0;
      piVar1 = *(int **)(this->mbr_0x14 + iVar4 * 4);
      if (0 < *piVar1) {
        do {
          iVar2 = *(int *)(piVar1[4] + iVar3 * 4);
          if (iVar2 == 0) {
            iVar2 = piVar1[5];
          }
          iVar2 = FUN_0059a530(iVar2 + 4);
          if (iVar2 == 0) {
            iVar4 = *(int *)(this->mbr_0x14 + iVar4 * 4);
            iVar3 = *(int *)(*(int *)(iVar4 + 0x10) + iVar3 * 4);
            if (iVar3 != 0) {
              return iVar3;
            }
            return *(int *)(iVar4 + 0x14);
          }
          iVar3 = iVar3 + 1;
          piVar1 = *(int **)(this->mbr_0x14 + iVar4 * 4);
        } while (iVar3 < *piVar1);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)this->mbr_0x4);
  }
  return 0;
}



