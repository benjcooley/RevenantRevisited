// FUN_0041b0b0_TArea_In @ 0041b0b0 size=102

undefined4 __thiscall FUN_0041b0b0_TArea_In(int param_1,undefined4 *param_2,int param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  
  if ((*(byte *)(param_1 + 0x44) & 1) != 0) {
    return 1;
  }
  if ((*(int *)(param_1 + 0x40) == -1) || (param_3 == *(int *)(param_1 + 0x40))) {
    iVar4 = 0;
    uVar1 = *param_2;
    uVar2 = param_2[1];
    if (0 < *(short *)(param_1 + 0x48)) {
      do {
        uVar5 = uVar1;
        uVar6 = uVar2;
        FUN_0041c780(iVar4);
        iVar3 = FUN_0041c720(uVar5,uVar6);
        if (iVar3 != 0) {
          return 1;
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 < *(short *)(param_1 + 0x48));
    }
  }
  return 0;
}


