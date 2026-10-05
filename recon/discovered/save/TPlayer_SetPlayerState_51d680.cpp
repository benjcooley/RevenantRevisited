// FUN_0051d680 @ 0051d680 size=376

void __thiscall FUN_0051d680(uint param_1,uint param_2)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  
  uVar1 = *(uint *)(param_1 + 0x36c);
  *(uint *)(param_1 + 0x36c) = param_2;
  if ((((param_2 & 2) != 0) && (*(int **)(param_1 + 0xd8) != (int *)0x0)) &&
     ((iVar2 = **(int **)(param_1 + 0xd8), iVar2 == 2 || ((iVar2 == 4 || (iVar2 == 0x1a)))))) {
    FUN_004cee70(0);
  }
  if (DAT_0066829c == 0) {
    return;
  }
  if (param_1 != DAT_00667fcc) goto LAB_0051d7c1;
  if (((param_2 & 4) == 0) || (DAT_0065d0d0 == 0)) {
    if (((param_2 & 4) == 0) && (DAT_0065d0d0 == 0)) {
      uVar3 = 1;
      goto LAB_0051d6f6;
    }
  }
  else {
    uVar3 = 0;
LAB_0051d6f6:
    FUN_0047c580(uVar3);
  }
  if (((((param_2 & 8) == 0) || (DAT_0065cb34 == (int *)0x0)) ||
      (iVar2 = FUN_0048eaf0(), iVar2 != 0)) || (iVar2 = FUN_0048eb00(), iVar2 != 0)) {
    if ((((param_2 & 8) == 0) && (DAT_0065cb30 != (int *)0x0)) &&
       ((iVar2 = FUN_0048ead0(), iVar2 == 0 &&
        ((iVar2 = FUN_0048eb00(), iVar2 == 0 && (DAT_0065cb30 != (int *)0x0)))))) {
      (**(code **)(*DAT_0065cb30 + 0x28))();
    }
  }
  else {
    (**(code **)(*DAT_0065cb34 + 0x2c))();
    FUN_00535d80(0);
    iVar2 = FUN_0047ed20();
    if (iVar2 == 3) {
      FUN_0047ecc0();
    }
  }
  if (((param_2 & 0x10) == 0) && ((-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) != param_1)) {
    FUN_004538d0(param_1,8);
  }
LAB_0051d7c1:
  if (uVar1 != param_2) {
    if (DAT_0067682c != 0) {
      FUN_005878a0(param_1,param_2);
      return;
    }
    FUN_00583fe0(param_1,0x45,param_2,0,0x31);
  }
  return;
}


