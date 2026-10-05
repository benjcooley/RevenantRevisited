// FUN_004367d0 @ 004367d0 size=190

void __thiscall FUN_004367d0(int *param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  
  if ((-1 < param_2) && (param_2 < param_1[0x22])) {
    if (*(int *)(param_1[0x26] + param_2 * 4) == param_1[0x27]) {
      param_1[0x27] = 0;
    }
    if (*(int *)(param_1[0x26] + param_2 * 4) == param_1[0x29]) {
      param_1[0x29] = 0;
    }
    if (*(int *)(param_1[0x26] + param_2 * 4) == param_1[0x28]) {
      param_1[0x28] = 0;
    }
    puVar1 = *(undefined4 **)(param_1[0x26] + param_2 * 4);
    if (puVar1 != (undefined4 *)0x0) {
      (**(code **)*puVar1)(1);
    }
    FUN_0041cb80(param_2);
    iVar2 = param_1[0x22];
    iVar4 = 0;
    if (0 < iVar2) {
      piVar5 = (int *)param_1[0x26];
      do {
        iVar3 = *piVar5;
        piVar5 = piVar5 + 1;
        *(int *)(iVar3 + 0xc) = iVar4;
        iVar4 = iVar4 + 1;
      } while (iVar4 < iVar2);
    }
    (**(code **)(*param_1 + 0x2c))(1);
  }
  return;
}


