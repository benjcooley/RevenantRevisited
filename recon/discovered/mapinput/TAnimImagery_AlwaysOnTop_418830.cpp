// FUN_00418830 @ 00418830 size=88

uint __thiscall FUN_00418830(int *param_1,int *param_2)

{
  ushort uVar1;
  int iVar2;
  uint uVar3;
  
  uVar1 = *(ushort *)(param_2 + 3);
  iVar2 = (**(code **)(*param_1 + 0x3c))();
  if (iVar2 <= (int)(uint)uVar1) {
    return 0;
  }
  iVar2 = (**(code **)(*param_2 + 300))(0xffffffff);
  if (iVar2 == 0) {
    return 0;
  }
  if ((*(byte *)(iVar2 + 0x10) & 0x20) != 0) {
    return 0;
  }
  uVar3 = (**(code **)(*param_1 + 0xec))((uint)uVar1);
  return uVar3 & 1;
}


