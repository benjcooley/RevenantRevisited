// FUN_0052c970_TBarInvPane_Initialize @ 0052c970 size=129

undefined4 __fastcall FUN_0052c970_TBarInvPane_Initialize(int *param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  
  FUN_00491900_TPane_Initialize();
  param_1[0x1a] = -1;
  param_1[0x19] = -1;
  param_1[0x1b] = 0;
  iVar3 = FUN_0046d710(s_BarInvBox_005e37b8);
  uVar2 = DAT_0065b648;
  iVar1 = DAT_0065b644;
  param_1[0x21] = iVar3;
  (**(code **)(*param_1 + 0x20))(iVar1,uVar2);
  iVar1 = DAT_0065b644;
  param_1[0x22] = 0;
  param_1[0x23] = (iVar1 + -0xdc) / 0x2d;
  (**(code **)(*param_1 + 0x2c))(1);
  return 1;
}


