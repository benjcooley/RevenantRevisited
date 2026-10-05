// FUN_00535b90 @ 00535b90 size=466

void __thiscall FUN_00535b90(undefined4 param_1,int param_2,int param_3,undefined4 *param_4)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  undefined4 uStack_40;
  undefined4 uStack_3c;
  undefined4 uStack_38;
  undefined4 *puStack_34;
  undefined4 uStack_30;
  undefined1 *puStack_2c;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  iVar1 = param_2;
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1a85;
  local_c = ExceptionList;
  if (((param_2 != 0) && (param_3 != 0)) && (0 < (int)param_4)) {
    if (*(short *)(param_2 + 4) == 0xb) {
      param_2 = 0x3cafff;
      uVar5 = 2;
      ExceptionList = &local_c;
    }
    else {
      uVar5 = 1;
      uVar4 = 0;
      piVar2 = &DAT_0066f6f8;
      do {
        if (*piVar2 == param_2) {
          ExceptionList = &local_c;
          if ((int)uVar4 < 0x10) goto LAB_00535c3a;
          break;
        }
        piVar2 = piVar2 + 1;
        uVar4 = uVar4 + 1;
      } while ((int)piVar2 < 0x66f738);
      uVar4 = DAT_0066f73c + 1 & 0xf;
      DAT_0066f73c = uVar4;
      ExceptionList = &local_c;
      (&DAT_0066f6f8)[uVar4] = param_2;
LAB_00535c3a:
      switch(uVar4 & 3) {
      case 0:
        param_2 = 0xff0000;
        break;
      case 1:
        param_2 = 0xff00;
        break;
      case 2:
        param_2 = 0xffff00;
        break;
      case 3:
        param_2 = 0xffff;
      }
    }
    puStack_2c = (undefined1 *)0x160;
    uStack_30 = 0x535cbb;
    iVar3 = FUN_00482fb0();
    local_4 = 0;
    if (iVar3 == 0) {
      puStack_2c = (undefined1 *)0x0;
    }
    else {
      puStack_34 = &param_3;
      puStack_2c = (undefined1 *)param_4;
      uStack_30 = 0;
      uStack_38 = 1;
      uStack_3c = 0xff;
      uStack_40 = 0xff;
      uStack_40 = FUN_00429950(0xff);
      param_4 = &uStack_3c;
      FUN_00419dd0();
      param_4 = &uStack_40;
      FUN_00419dd0(&param_2);
      puStack_2c = (undefined1 *)
                   FUN_00533f10(param_1,iVar1,uVar5,0xffffd8f0,0xffffd8f0,0xffffd8f0,0xffffd8f0);
    }
    local_4 = 0xffffffff;
    uStack_30 = 0x535d4d;
    FUN_0041c840();
  }
  ExceptionList = local_c;
  return;
}


