// FUN_004c4860_CalcDamage @ 004c4860 size=228

int __thiscall FUN_004c4860_CalcDamage(int *param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int unaff_retaddr;
  
  if (param_3 == -1) {
    return 0;
  }
  iVar1 = (**(code **)(*param_1 + 0x2c8))(param_3);
  iVar2 = (**(code **)(*param_1 + 700))();
  iVar2 = ((100 - iVar1) * (((param_3 + 100) * unaff_retaddr) / 100)) / 100 - iVar2;
  if (iVar2 < 1) {
    iVar2 = 1;
  }
  if (((((param_3 == 6) || (param_3 == 7)) || (param_3 == 8)) || (param_3 == 9)) && (param_3 != 0))
  {
    uVar3 = param_1[0x44];
    if ((uVar3 & 0x200) != 0) {
      iVar2 = iVar2 / 2;
    }
    if (param_1[0xa0] == 1) {
      iVar2 = 0;
    }
  }
  else {
    uVar3 = param_1[0x44];
    if ((uVar3 & 0x100) != 0) {
      iVar2 = iVar2 / 2;
    }
  }
  if (((uVar3 & 0x10) != 0) && (param_3 != 8)) {
    iVar2 = iVar2 / 7;
  }
  return iVar2;
}


