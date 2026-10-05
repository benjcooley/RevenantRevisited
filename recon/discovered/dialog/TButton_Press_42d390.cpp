// FUN_0042d390 @ 0042d390 size=285

void __thiscall FUN_0042d390(int *param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  
  uVar1 = param_1[5];
  if (param_2 == 0) {
    if ((uVar1 & 0x40000) == 0) {
      (**(code **)(*param_1 + 0x1c))(uVar1 & 0xfffeffff);
      (**(code **)(*param_1 + 0x1c))(param_1[5] | 0x20);
    }
    return;
  }
  if ((uVar1 & 0x40000) == 0) {
    (**(code **)(*param_1 + 0x1c))(uVar1 | 0x10000);
    (**(code **)(*param_1 + 0x1c))(param_1[5] | 0x20);
  }
  iVar2 = FUN_0049c430(s_click1_005cd2f0);
  if (-1 < iVar2) {
    iVar3 = FUN_0049b650(iVar2);
    if (iVar3 != 0) {
      FUN_0049b990(iVar2,0x7f,1,0,0x50,700);
    }
  }
  uVar1 = param_1[5];
  if ((uVar1 & 0x40000) == 0) {
    if ((uVar1 & 0x80000) == 0) goto LAB_0042d477;
  }
  else if ((uVar1 & 0x80000) == 0) {
    if ((~uVar1 & 0x10000) != 0) {
      (**(code **)(*param_1 + 0x1c))(uVar1 | 0x10000);
      FUN_004387f0(1);
      return;
    }
    (**(code **)(*param_1 + 0x1c))(uVar1 & 0xfffeffff);
    FUN_004387f0(1);
    return;
  }
  if ((uVar1 & 0x10000) == 0) {
    (**(code **)(*param_1 + 0x1c))(uVar1 | 0x10000);
    FUN_004387f0(1);
    return;
  }
LAB_0042d477:
  FUN_0042a820(3000);
  return;
}


