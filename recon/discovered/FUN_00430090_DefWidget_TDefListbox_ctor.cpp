// FUN_00430090_DefWidget_TDefListbox_ctor @ 00430090 size=591

undefined4 * __thiscall FUN_00430090_DefWidget_TDefListbox_ctor(undefined4 *param_1,int *param_2,int **param_3,int *param_4)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  int *piVar4;
  int *piVar5;
  int iVar6;
  int *piStack_38;
  int *piStack_34;
  int *piStack_30;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 *local_14;
  int local_10;
  void *local_c;
  undefined1 *puStack_8;
  int local_4;
  
  piVar5 = (int *)param_3;
  piVar4 = param_2;
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059caeb;
  local_c = ExceptionList;
  piStack_30 = (int *)param_3;
  piStack_34 = (int *)0x6;
  piStack_38 = param_2;
  ExceptionList = &local_c;
  local_14 = param_1;
  FUN_0042a350_DefWidget_TWidgetBase_ParseAttrs();
  uVar1 = param_1[5];
  local_4 = 0;
  *param_1 = &PTR_FUN_005a3d44;
  param_1[0x26] = 0;
  param_1[0x27] = 0;
  param_1[0x28] = 0xffffffff;
  if ((uVar1 & 1) == 0) {
    param_1[5] = uVar1 | 1;
    piStack_38 = param_4;
    if (param_4 == (int *)0x0) {
      if (((uVar1 & 0x10000) == 0) || ((uVar1 & 0x40000) != 0)) {
        if (((uVar1 & 0x20000) == 0) || (piStack_38 = &DAT_00655c40, (uVar1 & 0x40000) != 0)) {
          piStack_38 = &DAT_006559c0;
        }
      }
      else {
        piStack_38 = &DAT_00655b48;
      }
    }
    piStack_30 = piVar5;
    piStack_34 = piVar4;
    iVar6 = FUN_0042f9f0_DefWidget_LISTBOX_attr_parser();
    if (iVar6 != 0) {
      puVar2 = (undefined4 *)param_1[0x36];
      if (puVar2 != (undefined4 *)0x0) {
        if (param_1[0x18] == -10000) {
          param_1[0x18] = -puVar2[2];
          param_1[0x19] = -puVar2[3];
        }
        if (param_1[0x1a] == -10000) {
          uVar3 = puVar2[1];
          param_1[0x1a] = *puVar2;
          param_1[0x1b] = uVar3;
        }
      }
      if ((param_1[5] & 0x80000) == 0) {
        param_1[0x24] = 0;
        param_1[0x25] = 0;
      }
      else {
        if (piVar5[4] != 9) {
          piStack_30 = (int *)0x0;
          piStack_34 = (int *)s_Unrecognized_tag_in_LIST_control_005cd60c;
          piStack_38 = (int *)0x43019d;
          (**(code **)(*piVar4 + 0x98))();
          ExceptionList = local_c;
          return param_1;
        }
        piStack_30 = (int *)0x4301a9;
        FUN_004795a0();
        piStack_30 = (int *)0x14;
        piStack_34 = (int *)0x4301b0;
        iVar6 = FUN_00482fb0();
        local_4._0_1_ = 1;
        if (iVar6 == 0) {
          iVar6 = 0;
        }
        else {
          piStack_30 = (int *)0x40;
          piStack_34 = (int *)0x40;
          piStack_38 = (int *)0x4301cd;
          param_3 = (int **)iVar6;
          FUN_0041c7f0();
        }
        piStack_38 = (int *)param_1[0x4c];
        piStack_34 = (int *)param_1[0x4a];
        param_3 = &piStack_38;
        local_4 = (uint)local_4._1_3_ << 8;
        param_1[0x24] = iVar6;
        param_1[0x25] = 1;
        piStack_30 = piStack_38;
        FUN_00419dd0_BlitEffect_SetField(param_1 + 0x4b);
        iVar6 = FUN_0042f700(param_1[0x24],piVar4,piVar5,param_1[0x49]);
        if (iVar6 == 0) {
          ExceptionList = local_c;
          return param_1;
        }
      }
      if (param_1[0x4d] != 0) {
        piStack_30 = (int *)&param_3;
        piStack_34 = (int *)&param_2;
        piStack_38 = (int *)&param_4;
        FUN_004304a0(&local_18,&local_1c);
        piStack_30 = (int *)0x168;
        piStack_34 = (int *)0x430269;
        local_10 = FUN_00482fb0();
        local_4._0_1_ = 2;
        if (local_10 == 0) {
          iVar6 = 0;
        }
        else {
          piStack_30 = param_1 + 0x53;
          piStack_34 = (int *)param_3;
          piStack_38 = (int *)0xffffffff;
          iVar6 = FUN_0042de00_DefWidget_TDefScrollbar_ctor_programmatic(piVar4,local_18,local_1c,param_4,param_2,0,0);
        }
        local_4 = (uint)local_4._1_3_ << 8;
        param_1[0x35] = iVar6;
        *(undefined4 **)(iVar6 + 0x78) = param_1;
      }
      param_1[5] = param_1[5] & 0xfffffffe;
      piStack_30 = (int *)0x4302c8;
      FUN_004308b0();
    }
  }
  ExceptionList = local_c;
  return param_1;
}


