// FUN_005339b0_TDeathPane_Initialize @ 005339b0 size=590

undefined4 __fastcall FUN_005339b0_TDeathPane_Initialize(int param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  bool bVar4;
  char *local_20 [4];
  char *local_10;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a19c1;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_00434e40_TButtonPane_Initialize();
  local_20[0] = s_goluc01_005e3ef0;
  local_20[1] = s_goand01_005e3ef8;
  local_20[2] = s_gojha01_005e3f00;
  local_20[3] = s_gooli01_005e3f08;
  local_10 = s_gosar00_005e3f10;
  iVar1 = FUN_00483300_RandomRange(0,4);
  iVar1 = FUN_0049c430(local_20[iVar1]);
  if (-1 < iVar1) {
    iVar2 = FUN_0049b650(iVar1);
    if (iVar2 != 0) {
      FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
    }
  }
  bVar4 = DAT_0066f6f4 != 0;
  *(uint *)(param_1 + 0x60) = *(uint *)(param_1 + 0x60) | 0xe;
  if (bVar4) {
    iVar1 = FUN_00482fb0(0x148);
    local_4 = 0;
    if (iVar1 == 0) {
      uVar3 = 0;
    }
    else {
      uVar3 = FUN_0042c400(DAT_0066f6f4,s_Restart_005e3f18,0,0,0,0,0xffffffff,0x10,0xffffffff,0);
    }
    local_4 = 0xffffffff;
    FUN_00436790(uVar3);
    iVar1 = FUN_00482fb0(0x148);
    local_4 = 1;
    if (iVar1 == 0) {
      uVar3 = 0;
    }
    else {
      uVar3 = FUN_0042c400(DAT_0066f6f4,&DAT_005e3f20,0,0,0,0,0xffffffff,0x10,0xffffffff,0);
    }
    local_4 = 0xffffffff;
    FUN_00436790(uVar3);
    iVar1 = FUN_00482fb0(0x148);
    local_4 = 2;
    if (iVar1 == 0) {
      uVar3 = 0;
    }
    else {
      uVar3 = FUN_0042c400(DAT_0066f6f4,&DAT_005e3f28,0,0,0,0,0xffffffff,0x10,0xffffffff,0);
    }
    local_4 = 0xffffffff;
    FUN_00436790(uVar3);
    iVar1 = FUN_00436900(0);
    *(uint *)(iVar1 + 0x14) = *(uint *)(iVar1 + 0x14) | 0x40;
    *(undefined1 **)(iVar1 + 0x80) = &LAB_00533950;
    iVar1 = FUN_00436900(1);
    *(undefined1 **)(iVar1 + 0x80) = &LAB_00533970;
    *(uint *)(iVar1 + 0x14) = *(uint *)(iVar1 + 0x14) | 0x40;
    iVar1 = FUN_00436900(2);
    *(undefined1 **)(iVar1 + 0x80) = &LAB_00533990;
    *(uint *)(iVar1 + 0x14) = *(uint *)(iVar1 + 0x14) | 0x40;
    ExceptionList = local_c;
    return 1;
  }
  ExceptionList = local_c;
  return 0;
}


