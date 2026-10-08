// FUN_0042d4b0 @ 0042d4b0 size=519

void __thiscall FUN_0042d4b0(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined4 unaff_retaddr;
  
  if (param_2 == 1) {
    if ((param_1[5] & 0x40000U) == 0) {
      (**(code **)(*param_1 + 0x1c))(param_1[5] | 0x10000);
      (**(code **)(*param_1 + 0x1c))(param_1[5] | 0x20);
      param_1[0x26] = 1;
      *(int **)(param_1[2] + 0xa4) = param_1;
      return;
    }
    iVar1 = FUN_0049c430(s_click1_005cd2f0);
    if ((-1 < iVar1) && (iVar2 = FUN_0049b650(iVar1), iVar2 != 0)) {
      FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
    }
    uVar3 = param_1[5];
    if ((uVar3 & 0x40000) == 0) {
      if ((uVar3 & 0x80000) == 0) goto LAB_0042d6a6;
    }
    else if ((uVar3 & 0x80000) == 0) {
      FUN_00438a50(~uVar3 >> 0x10 & 1);
      return;
    }
    if ((uVar3 & 0x10000) == 0) {
      FUN_00438a50(1);
      return;
    }
  }
  else {
    if (param_2 != 4) {
      return;
    }
    if ((param_1[5] & 0x40000U) != 0) {
      return;
    }
    if (param_1[0x26] == 0) {
      return;
    }
    (**(code **)(*param_1 + 0x1c))(param_1[5] & 0xfffeffff);
    (**(code **)(*param_1 + 0x1c))(param_1[5] | 0x20);
    iVar1 = *param_1;
    *(undefined4 *)(param_1[2] + 0xa4) = 0;
    param_1[0x26] = 0;
    iVar1 = (**(code **)(iVar1 + 0x54))(unaff_retaddr,4);
    if (iVar1 == 0) {
      return;
    }
    iVar1 = FUN_0049c430(s_click1_005cd2f0);
    if ((-1 < iVar1) && (iVar2 = FUN_0049b650(iVar1), iVar2 != 0)) {
      FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
    }
    uVar3 = param_1[5];
    if ((uVar3 & 0x40000) == 0) {
      if ((uVar3 & 0x80000) == 0) goto LAB_0042d6a6;
    }
    else if ((uVar3 & 0x80000) == 0) {
      if ((~uVar3 & 0x10000) == 0) {
        uVar3 = uVar3 & 0xfffeffff;
      }
      else {
        uVar3 = uVar3 | 0x10000;
      }
      (**(code **)(*param_1 + 0x1c))(uVar3);
      (**(code **)(*param_1 + 0x1c))(param_1[5] | 0x20);
      return;
    }
    if ((uVar3 & 0x10000) == 0) {
      (**(code **)(*param_1 + 0x1c))(uVar3 | 0x10000);
      (**(code **)(*param_1 + 0x1c))(param_1[5] | 0x20);
      return;
    }
  }
LAB_0042d6a6:
  FUN_0042a820(3000);
  return;
}


