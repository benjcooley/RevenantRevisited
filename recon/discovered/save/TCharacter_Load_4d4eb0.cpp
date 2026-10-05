// FUN_004d4eb0 @ 004d4eb0 size=532

void __thiscall FUN_004d4eb0(int *param_1,int param_2,undefined4 param_3,int param_4)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  undefined1 uVar4;
  
  if (param_4 < 3) {
    uVar4 = 0;
  }
  else {
    uVar4 = **(undefined1 **)(param_2 + 4);
    *(undefined1 **)(param_2 + 4) = *(undefined1 **)(param_2 + 4) + 1;
  }
  FUN_004db930(param_2,param_3,uVar4);
  if (0 < param_4) {
    piVar1 = *(int **)(param_2 + 4);
    iVar3 = piVar1[1];
    param_1[0x4e] = *piVar1;
    iVar2 = piVar1[2];
    param_1[0x4f] = iVar3;
    param_1[0x50] = iVar2;
    *(int **)(param_2 + 4) = piVar1 + 3;
    if (param_4 < 4) {
      param_1[0x57] = -1;
    }
    else {
      param_1[0x57] = piVar1[3];
      *(int **)(param_2 + 4) = piVar1 + 4;
    }
    if (param_4 < 2) {
      param_1[0x6d] = -1;
      param_1[0x6c] = -1;
      param_1[0x6b] = -1;
      param_1[0x6a] = -1;
    }
    else {
      piVar1 = *(int **)(param_2 + 4);
      iVar3 = piVar1[1];
      param_1[0x6b] = *piVar1;
      iVar2 = piVar1[2];
      param_1[0x6c] = iVar3;
      iVar3 = piVar1[3];
      param_1[0x6d] = iVar2;
      param_1[0x6a] = iVar3;
      *(int **)(param_2 + 4) = piVar1 + 4;
    }
    if ((short)param_1[1] == 0xc) {
      iVar3 = *(int *)(param_1[0x3f] + 0x1e8);
      iVar2 = (**(code **)(*param_1 + 0x1c0))();
      if (iVar3 < iVar2) {
        (**(code **)(*param_1 + 0x1c4))(iVar3);
      }
      iVar3 = *(int *)(param_1[0x3f] + 0x1e8);
      iVar2 = (**(code **)(*param_1 + 0x1d8))();
      if (iVar3 < iVar2) {
        (**(code **)(*param_1 + 0x1dc))(iVar3);
      }
      iVar3 = *(int *)(param_1[0x3f] + 0x1e4);
      iVar2 = (**(code **)(*param_1 + 0x1c8))();
      if (iVar3 < iVar2) {
        (**(code **)(*param_1 + 0x1cc))(iVar3);
      }
      iVar3 = *(int *)(param_1[0x3f] + 0x1e4);
      iVar2 = (**(code **)(*param_1 + 0x1e0))();
      if (iVar3 < iVar2) {
        (**(code **)(*param_1 + 0x1e4))(iVar3);
      }
      iVar3 = *(int *)(param_1[0x3f] + 0x1e0);
      iVar2 = (**(code **)(*param_1 + 0x1d0))();
      if (iVar3 < iVar2) {
        (**(code **)(*param_1 + 0x1d4))(iVar3);
      }
      iVar3 = *(int *)(param_1[0x3f] + 0x1e0);
      iVar2 = (**(code **)(*param_1 + 0x1e8))();
      if (iVar3 < iVar2) {
        (**(code **)(*param_1 + 0x1ec))(iVar3);
      }
    }
    iVar3 = (**(code **)(*param_1 + 0x1c0))();
    if (iVar3 < 1) {
      (**(code **)(*param_1 + 0x40))(param_1[2] | 0x1000);
    }
    (**(code **)(*param_1 + 0x1c0))();
    param_1[0x66] = 0;
    param_1[0x65] = 100;
    param_1[0x67] = 100;
    param_1[0x68] = 0;
  }
  return;
}


