#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x412330



// Function at 00412330

undefined4 cls_0x412330::meth_0x412330()

{
  int iVar1;
  int **ppiVar2;
  int *piVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  
  if (this->mbr_0x0 != 0) {
    if (DAT_005d7a28 != 0) {
      FUN_0056cdb0();
    }
    if ((this->mbr_0x0 != 0) &&
       (uVar5 = 0, iVar6 = DAT_005e88a0, uVar4 = DAT_005e8890, 0 < (int)DAT_005e8890)) {
      do {
        if (((iVar6 != 0) &&
            (((uVar5 < uVar4 && (iVar1 = *(int *)(iVar6 + uVar5 * 4), iVar1 != 0)) &&
             (this->mbr_0x0 != 0)))) && (iVar1 != 0)) {
          cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)&DAT_005e8890);
          iVar6 = DAT_005e88a0;
          uVar4 = DAT_005e8890;
        }
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)uVar4);
    }
    iVar6 = 0;
    if (0 < DAT_005e8860) {
      do {
        if (((-1 < iVar6) &&
            (ppiVar2 = *(int ***)(DAT_005e8870 + iVar6 * 4), ppiVar2 != (int **)0x0)) &&
           (ppiVar2 != (int **)0x0)) {
          if (ppiVar2[0x15] != (int *)0x0) {
            if (DAT_005d7a28 == 0) {
              (**(code **)(*DAT_006699d0 + 0x38))(DAT_006699d0,*ppiVar2);
            }
            else {
              FUN_0056d120((int)(ppiVar2 + 1));
            }
            ppiVar2[0x15] = (int *)0x0;
          }
          piVar3 = *ppiVar2;
          if (piVar3 != (int *)0x0) {
            (**(code **)(*piVar3 + 8))(piVar3);
            *ppiVar2 = (int *)0x0;
          }
          FUN_004830f0(ppiVar2);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)&DAT_005e8860);
        iVar6 = iVar6 + 1;
      } while (iVar6 < DAT_005e8860);
    }
    DAT_005e8860 = 0;
    DAT_005e8864._0_4_ = 0;
    if (DAT_005e8918 != (int *)0x0) {
      (**(code **)(*DAT_005e8918 + 8))(DAT_005e8918);
      DAT_005e8918 = (int *)0x0;
    }
    iVar6 = (**(code **)(*DAT_00668f14 + 0x18))(DAT_00668f14,DAT_006699d0);
    if (iVar6 != 0) {
      FUN_004a90d0(iVar6);
    }
    if (DAT_006699d0 != (int *)0x0) {
      (**(code **)(*DAT_006699d0 + 8))(DAT_006699d0);
    }
    DAT_006699d0 = (int *)0x0;
    this->mbr_0x0 = 0;
    return 1;
  }
  return 0;
}



