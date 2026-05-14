// FUN_00499500_TSector_InitLights @ 00499500 size=91

void __fastcall FUN_00499500_TSector_InitLights(int param_1)

{
  int *piVar1;
  int iVar2;
  
  if ((*(int *)(param_1 + 0x98) == 0) && (*(int *)(param_1 + 0xac) == 0)) {
    for (iVar2 = 0; iVar2 < *(int *)(param_1 + 0xfc); iVar2 = iVar2 + 1) {
      piVar1 = *(int **)(*(int *)(param_1 + 0x10c) + iVar2 * 4);
      if (piVar1 == (int *)0x0) {
        piVar1 = *(int **)(param_1 + 0x110);
      }
      if (*(int *)(*(int *)(param_1 + 200) + *piVar1 * 4) != 0) {
        FUN_00471b40(0);
      }
    }
    *(undefined4 *)(param_1 + 0xac) = 1;
  }
  return;
}


