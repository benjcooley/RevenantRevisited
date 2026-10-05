// FUN_0052c5c0 @ 0052c5c0 size=341

void __fastcall FUN_0052c5c0(int param_1)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  uint *puVar5;
  uint uVar6;
  undefined4 *puVar7;
  undefined4 *local_28;
  undefined1 local_20 [20];
  void *local_c;
  undefined1 *puStack_8;
  int local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a18e4;
  local_c = ExceptionList;
  if (*(int *)(param_1 + 100) != 0) {
    ExceptionList = &local_c;
    local_28 = (undefined4 *)FUN_00482fb0(0xc);
    local_4 = 0;
    if (local_28 == (undefined4 *)0x0) {
      local_28 = (undefined4 *)0x0;
    }
    else {
      puVar4 = *(undefined4 **)(param_1 + 100);
      *local_28 = *puVar4;
      FUN_0049cc20(0x2080,0x40);
      piVar1 = local_28 + 1;
      local_4._0_1_ = 1;
      FUN_005295e0(puVar4 + 1,0x1040,piVar1,local_20);
      uVar3 = FUN_00482ef0(*piVar1 << 1);
      iVar2 = *piVar1;
      local_28[2] = uVar3;
      puVar4 = (undefined4 *)FUN_0049cdd0();
      local_4 = (uint)local_4._1_3_ << 8;
      puVar7 = (undefined4 *)local_28[2];
      for (uVar6 = (uint)(iVar2 << 1) >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
        *puVar7 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar7 = puVar7 + 1;
      }
      for (uVar6 = iVar2 << 1 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
        *(undefined1 *)puVar7 = *(undefined1 *)puVar4;
        puVar4 = (undefined4 *)((int)puVar4 + 1);
        puVar7 = (undefined4 *)((int)puVar7 + 1);
      }
      FUN_0049cc50();
    }
    local_4 = 0xffffffff;
    puVar5 = (uint *)(*(int *)(param_1 + 0x60) + 0x24);
    uVar6 = 0;
    puVar4 = *(undefined4 **)(*(int *)(param_1 + 0x60) + 0x34);
    while( true ) {
      if ((puVar5 == (uint *)0x0) || (*puVar5 <= uVar6)) goto LAB_0052c6f2;
      if (*(int *)*puVar4 == **(int **)(param_1 + 100)) break;
      puVar4 = puVar4 + 1;
      uVar6 = uVar6 + 1;
    }
    puVar4 = (undefined4 *)*puVar4;
    if (puVar4 != (undefined4 *)0x0) {
      *puVar4 = 0xffffffff;
      if (puVar4[2] != 0) {
        FUN_00482f80(puVar4[2]);
      }
      puVar4[2] = 0;
      puVar4[1] = 0;
      FUN_004830f0(puVar4);
    }
    FUN_0041cb40(uVar6);
LAB_0052c6f2:
    FUN_0041c840(local_28);
  }
  ExceptionList = local_c;
  return;
}


