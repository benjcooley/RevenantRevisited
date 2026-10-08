// FUN_004909d0 @ 004909d0 size=260

int FUN_004909d0(int *param_1,undefined4 param_2)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  bool bVar6;
  
  if (param_1 == (int *)0x0) {
    return 0;
  }
  bVar6 = param_1 == DAT_0065bb14;
  DAT_00667fd0 = param_1;
  param_1[0x12] = 0;
  if (bVar6) {
    DAT_0065bb14 = (int *)0x0;
    iVar4 = 1;
  }
  else {
    iVar4 = FUN_0048e8f0();
    if (iVar4 == 0) goto LAB_00490ab5;
  }
  FUN_004911b0(param_2);
  piVar1 = DAT_00667fd0;
  (**(code **)(*DAT_00667fd0 + 8))();
  FUN_00412490();
  if ((undefined4 *)piVar1[5] != (undefined4 *)0x0) {
    puVar5 = (undefined4 *)piVar1[5];
    for (uVar2 = piVar1[3] & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
      *(undefined1 *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  piVar1[2] = 0;
  piVar1[1] = 0;
  piVar1[0x12] = 0;
  piVar1[8] = 0;
  piVar1[9] = 0;
  piVar1[10] = 0;
  piVar1[0xb] = 0;
  piVar1[7] = 0;
  if (piVar1[0x18] != 0) {
    FUN_004aa490(piVar1[0x17]);
    piVar1[0x17] = -1;
    if (piVar1[0x19] != 0) {
      if ((undefined4 *)piVar1[0x18] != (undefined4 *)0x0) {
        (*(code *)**(undefined4 **)piVar1[0x18])(1);
      }
      piVar1[0x18] = 0;
    }
    piVar1[0x19] = 0;
  }
  FUN_004aa0a0(0);
  FUN_00482120();
LAB_00490ab5:
  piVar1 = DAT_00667fd0 + 6;
  if ((iVar4 != 0) && (DAT_006682b8 == 0)) {
    DAT_00667fd0 = (int *)0x0;
    return *piVar1;
  }
  DAT_00667fd0 = (int *)0x0;
  return 0;
}


