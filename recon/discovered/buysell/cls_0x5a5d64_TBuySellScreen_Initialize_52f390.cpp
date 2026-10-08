// FUN_0052f390 @ 0052f390 size=857

undefined4 __fastcall FUN_0052f390(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  undefined4 *puVar4;
  void *unaff_ESI;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a194f;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  *(undefined4 *)(param_1 + 0x1b0) = 1;
  *(undefined4 *)(param_1 + 0x180) = 0;
  *(undefined4 *)(param_1 + 0x188) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x18c) = 0xffffffff;
  if (*(int *)(param_1 + 0x1ac) == 0) {
    FUN_00434e40();
    if (*(int *)(param_1 + 400) == 0) {
      iVar1 = FUN_0047f670(s_buysell_dat_005e3d2c,0xffffffff,0);
      *(int *)(param_1 + 400) = iVar1;
      *(undefined4 *)(param_1 + 0x184) = 3;
      *(undefined4 *)(param_1 + 0x1a0) = 0;
      *(undefined4 *)(param_1 + 0x1a4) = 0;
      *(undefined4 *)(param_1 + 0x1a8) = 0;
      *(undefined4 *)(param_1 + 0x1b4) = 0;
      if (iVar1 == 0) {
        ExceptionList = local_c;
        return 0;
      }
      iVar1 = FUN_00482fb0(0x148);
      local_4 = 0;
      if (iVar1 == 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = FUN_0042c400(*(undefined4 *)(param_1 + 400),s_BuySellUp_005e3d38,0x26,FUN_0052fe90,0
                             ,0,0,0x10,0xffffffff,0);
      }
      local_4 = 0xffffffff;
      FUN_00436790(uVar2);
      iVar1 = FUN_00482fb0(0x148);
      local_4 = 1;
      if (iVar1 == 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = FUN_0042c400(*(undefined4 *)(param_1 + 400),s_BuySellDown_005e3d44,0x28,FUN_0052fee0
                             ,0,0,0,0x10,0xffffffff,0);
      }
      local_4 = 0xffffffff;
      FUN_00436790(uVar2);
      iVar1 = FUN_00482fb0(0x148);
      local_4 = 2;
      if (iVar1 == 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = FUN_0042c400(*(undefined4 *)(param_1 + 400),s_BuySellActivate_005e3d50,0x41,
                             FUN_0052ff40,0,0,0,0x10,0xffffffff,0);
      }
      local_4 = 0xffffffff;
      FUN_00436790(uVar2);
      iVar1 = FUN_00482fb0(0x148);
      local_4 = 3;
      if (iVar1 == 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = FUN_0042c400(*(undefined4 *)(param_1 + 400),s_BuySellExit_005e3d60,0x45,FUN_00530570
                             ,0,0,0,0x10,0xffffffff,0);
      }
      local_4 = 0xffffffff;
      FUN_00436790(uVar2);
      piVar3 = (int *)FUN_00436900(0);
      (**(code **)(*piVar3 + 0x1c))(piVar3[5] & 0xffffffdf);
      piVar3 = (int *)FUN_00436900(1);
      (**(code **)(*piVar3 + 0x1c))(piVar3[5] & 0xffffffdf);
      piVar3 = (int *)FUN_00436900(2);
      (**(code **)(*piVar3 + 0x1c))(piVar3[5] & 0xffffffdf);
      piVar3 = (int *)FUN_00436900(3);
      (**(code **)(*piVar3 + 0x1c))(piVar3[5] & 0xffffffdf);
      FUN_0046d710(s_BuySellMain_005e3d6c);
      puVar4 = (undefined4 *)FUN_00482fb0(0x78);
      if (puVar4 != (undefined4 *)0x0) {
        FUN_004bcb00();
        *puVar4 = &PTR_FUN_005a3980;
        puVar4[0x1a] = 0;
        FUN_004a5740(400,0x18,0x400,puVar4[4],0);
        puVar4[0x1c] = 1;
        *(undefined4 **)(param_1 + 0x19c) = puVar4;
        *(undefined4 *)(param_1 + 0x1ac) = 1;
        ExceptionList = unaff_ESI;
        return 1;
      }
      *(undefined4 *)(param_1 + 0x1ac) = 1;
      *(undefined4 *)(param_1 + 0x19c) = 0;
      ExceptionList = unaff_ESI;
      return 1;
    }
  }
  ExceptionList = local_c;
  return 1;
}


