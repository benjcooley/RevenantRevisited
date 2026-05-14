// FUN_00499250_TSector_RemoveObject @ 00499250 size=170

int __thiscall FUN_00499250_TSector_RemoveObject(int param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  int *piVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  iVar5 = 0;
  if (0 < *(int *)(param_1 + 0xb8)) {
    piVar4 = *(int **)(param_1 + 200);
    while (*piVar4 != param_2) {
      iVar5 = iVar5 + 1;
      piVar4 = piVar4 + 1;
      if (*(int *)(param_1 + 0xb8) <= iVar5) {
        return -1;
      }
    }
    if (-1 < iVar5) {
      FUN_0041cb40(iVar5);
      *(undefined4 *)(param_2 + 0x44) = 0;
      piVar4 = (int *)(param_1 + 0xcc);
      iVar6 = 6;
      do {
        iVar7 = 0;
        if (0 < *piVar4) {
          piVar3 = (int *)piVar4[4];
          do {
            piVar2 = (int *)*piVar3;
            if (piVar2 == (int *)0x0) {
              piVar2 = (int *)piVar4[5];
            }
            if (*piVar2 == iVar5) {
              iVar1 = *(int *)(piVar4[4] + iVar7 * 4);
              if (iVar1 != 0) {
                FUN_004830f0(iVar1);
              }
              FUN_0041cb80(iVar7);
              break;
            }
            iVar7 = iVar7 + 1;
            piVar3 = piVar3 + 1;
          } while (iVar7 < *piVar4);
        }
        piVar4 = piVar4 + 6;
        iVar6 = iVar6 + -1;
        if (iVar6 == 0) {
          return iVar5;
        }
      } while( true );
    }
  }
  return -1;
}


