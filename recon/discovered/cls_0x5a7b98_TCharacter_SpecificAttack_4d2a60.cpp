// FUN_004d2a60 @ 004d2a60 size=377

undefined4 __thiscall FUN_004d2a60(int *param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  uint uStack_4;
  
  iVar2 = (**(code **)(*param_1 + 0x1c0))();
  if (iVar2 < 1) {
    return 0;
  }
  if ((param_1[0x44] & 0x80000U) == 0) {
    iVar2 = *(int *)(param_1[0x36] + 0x48);
    if ((iVar2 != 0) && ((*(uint *)(iVar2 + 0x24) & 0x2000000) != 0)) {
      return 0;
    }
    iVar2 = *(int *)(param_1[0x36] + 0x4c);
    if ((iVar2 != 0) && ((*(byte *)(iVar2 + 0x24) & 0x80) != 0)) {
      return 0;
    }
  }
  piVar1 = (int *)param_1[0x38];
  if ((piVar1 == (int *)0x0) ||
     (((iVar2 = *piVar1, iVar2 != 3 && ((piVar1 == (int *)0x0 || (iVar2 != 0x19)))) ||
      (uVar5 = piVar1[0x11], uVar5 == 0)))) {
    iVar2 = FUN_004cd690(&uStack_4,1,0xffffffff,*(undefined1 *)((int)param_1 + 0x36),0x20,7);
    uVar5 = (iVar2 < 1) - 1 & uStack_4;
LAB_004d2bb7:
    if (uVar5 != 0) {
      uVar4 = (**(code **)(*param_1 + 4))(uVar5);
      goto LAB_004d2b05;
    }
  }
  else {
    if ((iVar2 == 3) || (iVar2 == 0x19)) goto LAB_004d2bb7;
    uVar5 = 0;
  }
  uVar4 = 10000;
LAB_004d2b05:
  iVar2 = FUN_00483300(1,0x32);
  iVar3 = FUN_00483300(1,0x32);
  uStack_8 = 0xffffffff;
  uStack_c = 0xffffffff;
  uStack_10 = 0xffffffff;
  uStack_14 = 0xffffffff;
  param_1[0x48] = 0;
  iVar2 = FUN_004d1120(param_2,&uStack_8,&uStack_c,&uStack_10,&uStack_14,uVar4,0xffffffff,0xffffffff
                       ,iVar2 + iVar3,0,0,uVar5);
  if (iVar2 == 0) {
    return 0;
  }
  uVar4 = FUN_004d2120(param_2,uStack_8,uStack_c,uStack_10,uStack_14,uVar5);
  return uVar4;
}


