// Decompiled methods and structure for class: cls_0x4925b0

/*
/OOAnalyzer/cls_0x4925b0
pack(disabled)
Structure cls_0x4925b0 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 004925b0

undefined4 __thiscall
OOAnalyzer::cls_0x4925b0::meth_0x4925b0(cls_0x4925b0 *this,int param_1,int param_2)

{
  dword dVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int in_stack_0000000c;
  
  dVar1 = this->mbr_0x4;
  do {
    if (dVar1 == 0) {
      return 0;
    }
    iVar3 = 0;
    if (0 < *(int *)(dVar1 + 0x44)) {
      do {
        piVar4 = *(int **)(*(int *)(dVar1 + 0x1c) + iVar3 * 4);
        if (piVar4 == (int *)0x0) {
          piVar4 = *(int **)(dVar1 + 0x20);
        }
        if ((piVar4 != (int *)0x0) && (*piVar4 == param_1)) {
          if (param_2 == 0) {
            if (in_stack_0000000c == 0) {
              return 1;
            }
          }
          else {
            iVar2 = FUN_0059a530(piVar4 + 2,param_2);
            if (iVar2 == 0) {
              return 1;
            }
            if (in_stack_0000000c == 0) goto LAB_00492619;
          }
          iVar2 = FUN_0059a530(piVar4 + 2);
          if (iVar2 == 0) {
            return 1;
          }
        }
LAB_00492619:
        iVar3 = iVar3 + 1;
      } while (iVar3 < *(int *)(dVar1 + 0x44));
    }
    dVar1 = *(dword *)(dVar1 + 8);
  } while( true );
}



