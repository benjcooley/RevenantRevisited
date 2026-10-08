// FUN_0047cdb0 @ 0047cdb0 size=197

void __thiscall FUN_0047cdb0(int *param_1,undefined4 param_2,undefined4 param_3)

{
  int *piVar1;
  uint uVar2;
  undefined4 uVar3;
  int iVar4;
  
  if (param_1[0x175] != 0) {
    return;
  }
  FUN_00490860(param_2,param_3);
  if (DAT_00668154 != 0) {
    return;
  }
  if (param_1[7] < 1) {
    uVar2 = 0;
  }
  else {
    uVar2 = param_1[param_1[7] + 0xb];
  }
  if ((uVar2 & 2) != 0) {
    return;
  }
  if (DAT_00667fcc != 0) {
    piVar1 = *(int **)(DAT_00667fcc + 0xe0);
    if (piVar1 != (int *)0x0) {
      if (*piVar1 == 3) {
        uVar3 = 2;
        goto LAB_0047ce59;
      }
      if ((piVar1 != (int *)0x0) && (*piVar1 == 0x19)) {
        uVar3 = 4;
        goto LAB_0047ce59;
      }
    }
    if (*(int *)(DAT_00667fcc + 0xe0) != 0) {
      iVar4 = FUN_004dab80(s_sneak_005c618c);
      uVar3 = 8;
      if (iVar4 != 0) goto LAB_0047ce59;
    }
  }
  uVar3 = 1;
LAB_0047ce59:
  uVar3 = FUN_00439150(param_2,param_3,uVar3);
  (**(code **)(*param_1 + 0x4c))(uVar3);
  return;
}


