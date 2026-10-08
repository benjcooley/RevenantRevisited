// FUN_00420900 @ 00420900 size=113

undefined4 FUN_00420900(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  
  iVar2 = param_2;
  iVar1 = FUN_0041e690(*(undefined4 *)(param_2 + 0x28),param_1,param_4);
  if (iVar1 == 0) {
    return 4;
  }
  FUN_00479580();
  iVar2 = FUN_0047a410(iVar2,&DAT_005caf10,&param_2);
  if (iVar2 == 0) {
    param_2 = 0;
  }
  if (param_1 != 0) {
    iVar2 = FUN_0046ea90(iVar1);
    FUN_004cfba0(iVar2 + param_2);
  }
  return 1;
}


