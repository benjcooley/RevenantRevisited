// FUN_00433640_DefWidget_TDefDroplist_ctor @ 00433640 size=644

undefined4 * __thiscall FUN_00433640_DefWidget_TDefDroplist_ctor(undefined4 *param_1,int *param_2,int param_3)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  void *local_c;
  undefined1 *puStack_8;
  int local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059cb4c;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0042a350_DefWidget_TWidgetBase_ParseAttrs();
  local_4 = 0;
  *param_1 = &PTR_FUN_005a3e1c;
  param_1[0x24] = 0;
  param_1[0x25] = 0;
  param_1[0x2f] = 0;
  param_1[0x30] = 0xffffffff;
  if ((param_1[5] & 1) == 0) {
    param_1[5] = param_1[5] | 1;
    iVar8 = FUN_00433170_DefWidget_DROPLIST_attr_parser();
    if (iVar8 != 0) {
      puVar1 = (undefined4 *)param_1[0x36];
      if (puVar1 != (undefined4 *)0x0) {
        if (param_1[0x18] == -10000) {
          param_1[0x18] = -puVar1[2];
          param_1[0x19] = -puVar1[3];
        }
        if (param_1[0x1a] == -10000) {
          uVar2 = puVar1[1];
          param_1[0x1a] = *puVar1;
          param_1[0x1b] = uVar2;
        }
      }
      if ((param_1[5] & 0x40000) == 0) {
        param_1[0x27] = 0;
        param_1[0x28] = 0;
      }
      else {
        if (*(int *)(param_3 + 0x10) != 9) {
          (**(code **)(*param_2 + 0x98))();
          ExceptionList = local_c;
          return param_1;
        }
        FUN_004795a0();
        iVar8 = FUN_00482fb0();
        local_4._0_1_ = 1;
        if (iVar8 == 0) {
          iVar8 = 0;
        }
        else {
          FUN_0041c7f0();
        }
        local_4 = (uint)local_4._1_3_ << 8;
        param_1[0x27] = iVar8;
        param_1[0x28] = 1;
        FUN_00419dd0(param_1 + 0x66);
        iVar8 = FUN_0042f700(param_1[0x27],param_2,param_3,param_1[100]);
        if (iVar8 == 0) {
          ExceptionList = local_c;
          return param_1;
        }
      }
      iVar8 = param_1[0x4c];
      iVar3 = param_1[0x18];
      uVar4 = param_1[5];
      if ((uVar4 & 0x80000) == 0) {
        iVar10 = param_1[0x4f] + param_1[0x1b] + param_1[0x19];
      }
      else {
        iVar10 = (param_1[0x19] - param_1[0x50]) - param_1[0x4e];
      }
      iVar5 = param_1[0x1a];
      iVar6 = param_1[0x4d];
      iVar7 = param_1[0x27];
      uVar2 = param_1[0x50];
      iVar9 = FUN_00482fb0();
      local_4._0_1_ = 2;
      if (iVar9 == 0) {
        iVar8 = 0;
      }
      else {
        iVar8 = FUN_0042fe80(param_2,iVar3 + iVar8,iVar10,(iVar5 - iVar6) - iVar8,uVar2,0,
                             param_1 + 0xe,param_1[0x16],
                             (uVar4 & 0x10000) << 2 | -(uint)(iVar7 != 0) & 0x80000 | 0x10002);
      }
      param_1[0x31] = iVar8;
      *(undefined4 **)(iVar8 + 0x78) = param_1;
      local_4 = (uint)local_4._1_3_ << 8;
      param_1[5] = param_1[5] & 0xfffffffe;
      FUN_00433ef0();
    }
  }
  ExceptionList = local_c;
  return param_1;
}


