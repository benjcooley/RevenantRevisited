// FUN_00520d80 @ 00520d80 size=124

void __thiscall FUN_00520d80(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  short sVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  FUN_00472430(param_2,param_3,param_4);
  sVar1 = **(short **)(param_2 + 4);
  *(short **)(param_2 + 4) = *(short **)(param_2 + 4) + 1;
  if (sVar1 < 1) {
    *(undefined4 *)(param_1 + 0xd8) = 0;
    return;
  }
  iVar5 = (int)sVar1;
  iVar3 = FUN_00482fb0(iVar5 + 1);
  *(int *)(param_1 + 0xd8) = iVar3;
  if (iVar3 != 0) {
    iVar4 = 0;
    if (0 < iVar5) {
      do {
        puVar2 = *(undefined1 **)(param_2 + 4);
        *(undefined1 *)(iVar3 + iVar4) = *puVar2;
        iVar4 = iVar4 + 1;
        *(undefined1 **)(param_2 + 4) = puVar2 + 1;
      } while (iVar4 < iVar5);
    }
    *(undefined1 *)(iVar3 + iVar4) = 0;
  }
  return;
}


