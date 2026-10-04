// FUN_00420980 @ 00420980 size=210

undefined4 FUN_00420980(int param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  
  uVar3 = 0;
  if (*(int *)(param_2 + 0x10) == 4) {
    iVar2 = FUN_00479700(&PTR_DAT_005caf14,0);
    if (((iVar2 != 0) && (piVar1 = *(int **)(param_1 + 0xe0), piVar1 != (int *)0x0)) &&
       ((*piVar1 == 3 || ((piVar1 != (int *)0x0 && (*piVar1 == 0x19)))))) {
      FUN_004d3fd0();
      FUN_00479580();
      return 0x4000;
    }
    iVar2 = FUN_00479700(&DAT_005caf18,0);
    if (iVar2 == 0) {
      FUN_00451fe0(*(undefined4 *)(param_2 + 0x28),param_1,0,0);
      return 4;
    }
    if ((*(int **)(param_1 + 0xe0) == (int *)0x0) || (**(int **)(param_1 + 0xe0) != 3)) {
      FUN_004d3b90(0,3);
      uVar3 = 0x4000;
    }
    FUN_00479580();
  }
  return uVar3;
}


