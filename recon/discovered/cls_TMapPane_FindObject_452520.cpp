// FUN_00452520 @ 00452520 size=367

int * __thiscall FUN_00452520(int param_1,int param_2,int param_3)

{
  int *piVar1;
  int iVar2;
  int unaff_EDI;
  int *piVar3;
  undefined4 uStack_4;
  
  if ((param_2 != DAT_005e91cc) || (piVar1 = DAT_005e91d4, param_3 != DAT_005e91d0)) {
    piVar1 = (int *)FUN_004142d0(param_2,param_3);
  }
  if (piVar1 == (int *)0x0) {
    piVar3 = (int *)(param_3 + *(int *)(param_1 + 0x7c));
    (**(code **)(**(int **)(param_1 + 0x864) + 0x44))(param_2 + *(int *)(param_1 + 0x78),piVar3,1,1)
    ;
    FUN_0044cf80_TMapIterator_Initialize(0,0x20,0,0,0xffffffff);
    while (piVar3 != (int *)0x0) {
      iVar2 = (**(code **)(*piVar3 + 0xf0))(&stack0xffffffa0);
      if (((iVar2 != 0) &&
          (((DAT_00668154 != 0 || ((short)piVar3[1] != 9)) &&
           (iVar2 = FUN_0046e8a0(), **(int **)(*(int *)(iVar2 + 4) + 0x54) != 1)))) &&
         ((((piVar1 == (int *)0x0 || (iVar2 = (**(code **)(*piVar3 + 0x10c))(), iVar2 != 0)) ||
           ((unaff_EDI == 0 &&
            (iVar2 = (**(code **)(*piVar3 + 0x104))(*(undefined4 *)(param_1 + 0x864)), iVar2 != 0)))
           ) && (((DAT_00668154 != 0 ||
                  ((iVar2 = FUN_0043a160(), iVar2 == 0 &&
                   (iVar2 = (**(code **)(*piVar3 + 0x134))(), iVar2 != 0)))) ||
                 (iVar2 = (**(code **)(*piVar3 + 0xc0))(uStack_4), iVar2 != -1)))))) {
        unaff_EDI = (**(code **)(*piVar3 + 0x10c))();
        piVar1 = piVar3;
      }
      FUN_0044d080_TMapIterator_NextItem();
    }
    piVar3 = *(int **)(param_1 + 0x864);
    (**(code **)(*piVar3 + 0x44))(0,0,piVar3[1],piVar3[2]);
  }
  return piVar1;
}


