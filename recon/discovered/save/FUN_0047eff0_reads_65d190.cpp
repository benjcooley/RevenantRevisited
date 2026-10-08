// FUN_0047eff0 @ 0047eff0 size=235

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0047eff0(int param_1)

{
  int *piVar1;
  
  FUN_0053cab0(1);
  *(undefined4 *)(param_1 + 0x6c8) = 1;
  if (*(int *)(param_1 + 0x6a0) == 0) {
    if ((((DAT_0065d190 != 1) && (DAT_0065d194 == 0)) && (DAT_0065d198 == 0)) && (DAT_0065d190 == 0)
       ) {
      DAT_0065d194 = 1;
    }
  }
  else {
    FUN_0048ed90(&DAT_0065b140,0xffffffff);
    DAT_0065b188 = 0;
    _DAT_0065b18c = 0;
    (**(code **)(DAT_0065b140 + 0x28))();
  }
  piVar1 = DAT_0065bfe4;
  (**(code **)(*DAT_0065bfe4 + 0x1c))(DAT_0065bfe4[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bfe8;
  (**(code **)(*DAT_0065bfe8 + 0x1c))(DAT_0065bfe8[5] | 0x10000);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bfec;
  (**(code **)(*DAT_0065bfec + 0x1c))(DAT_0065bfec[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  return;
}


