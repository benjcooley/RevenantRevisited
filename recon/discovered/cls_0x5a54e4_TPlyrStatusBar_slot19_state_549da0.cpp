// FUN_00549da0 @ 00549da0 size=178

void __fastcall FUN_00549da0(int param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  if (DAT_00667fcc == 0) {
    *(undefined4 *)(param_1 + 0xd8) = 0;
    *(undefined4 *)(param_1 + 0xd4) = 0;
    *(undefined4 *)(param_1 + 0xe0) = 0;
    *(undefined4 *)(param_1 + 0xdc) = 0;
    return;
  }
  piVar1 = *(int **)(DAT_00667fcc + 0xe0);
  if ((piVar1 == (int *)0x0) || ((*piVar1 != 3 && ((piVar1 == (int *)0x0 || (*piVar1 != 0x19)))))) {
    iVar2 = 0;
  }
  else {
    iVar2 = piVar1[0x11];
  }
  iVar3 = *(int *)(param_1 + 0xd4);
  *(undefined4 *)(param_1 + 0xd8) = 6;
  if (iVar3 < 6) {
    iVar3 = iVar3 + 1;
  }
  else {
    if (iVar3 < 7) goto LAB_00549e18;
    iVar3 = iVar3 + -1;
  }
  *(int *)(param_1 + 0xd4) = iVar3;
LAB_00549e18:
  if (iVar2 == 0) {
    *(undefined4 *)(param_1 + 0xe0) = 0;
  }
  else {
    *(undefined4 *)(param_1 + 0xe0) = 6;
  }
  iVar2 = *(int *)(param_1 + 0xdc);
  if (iVar2 < *(int *)(param_1 + 0xe0)) {
    *(int *)(param_1 + 0xdc) = iVar2 + 1;
    return;
  }
  if (*(int *)(param_1 + 0xe0) < iVar2) {
    *(int *)(param_1 + 0xdc) = iVar2 + -1;
  }
  return;
}


