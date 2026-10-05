// FUN_00535e90 @ 00535e90 size=337

undefined4 __fastcall FUN_00535e90(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1a9a;
  local_c = ExceptionList;
  if ((0 < *(int *)(param_1 + 0x1d8)) && (DAT_00667fcc != 0)) {
    ExceptionList = &local_c;
    *(undefined4 *)(param_1 + 0x1ec) = DAT_0065d0d0;
    FUN_0047c580();
    iVar1 = FUN_00482fb0();
    local_4 = 0;
    if (iVar1 == 0) {
      uVar2 = 0;
    }
    else {
      FUN_00429950(0xff);
      FUN_00419dd0();
      uVar2 = FUN_00429950(0x3c,0xaf);
      FUN_00419dd0(uVar2);
      uVar2 = FUN_00533f10(param_1,DAT_00667fcc,3,0xffffd8f0,0xffffd8f0,0xffffd8f0,0xffffd8f0);
    }
    local_4 = 0xffffffff;
    *(undefined4 *)(param_1 + 400) = uVar2;
    FUN_0041c840();
    ExceptionList = local_c;
    return 1;
  }
  return 0;
}


