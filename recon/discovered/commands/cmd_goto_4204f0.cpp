// FUN_004204f0 @ 004204f0 size=195

undefined4 FUN_004204f0(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  iVar1 = *(int *)(param_2 + 0x10);
  if (iVar1 == 8) {
    uVar2 = *(undefined4 *)(param_2 + 0x14);
  }
  else {
    if ((iVar1 != 2) && (iVar1 != 4)) {
      return 4;
    }
    iVar1 = FUN_00451fe0(*(undefined4 *)(param_2 + 0x28),param_1,1,0);
    if (iVar1 != 0) {
      FUN_004cee50(iVar1);
      return 1;
    }
    uVar2 = FUN_00497800(*(undefined4 *)(param_2 + 0x28),param_1);
  }
  FUN_00478a10();
  FUN_00479580();
  iVar1 = *(int *)(param_2 + 0x10);
  if (iVar1 == 8) {
    uVar3 = *(undefined4 *)(param_2 + 0x14);
  }
  else {
    if ((iVar1 != 2) && (iVar1 != 4)) {
      return 4;
    }
    uVar3 = FUN_00497800(*(undefined4 *)(param_2 + 0x28),param_1);
  }
  FUN_00478a10();
  FUN_004cedb0(uVar2,uVar3,0);
  return 1;
}


