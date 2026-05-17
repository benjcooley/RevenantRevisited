// FUN_0042c6a0_DefWidget_TDefButton_ctor @ 0042c6a0 size=269

undefined4 * __thiscall
FUN_0042c6a0_DefWidget_TDefButton_ctor(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined *param_4)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  int iVar4;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ca82;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0042a350_DefWidget_TWidgetBase_ParseAttrs(param_2,4,param_3);
  uVar1 = param_1[5];
  local_4 = 0;
  *param_1 = &PTR_FUN_005a3c68;
  param_1[0x26] = 0;
  param_1[0x24] = 0;
  if ((uVar1 & 1) == 0) {
    param_1[5] = uVar1 | 1;
    if (param_4 == (undefined *)0x0) {
      if ((uVar1 & 0x200000) == 0) {
        param_4 = &DAT_00656220;
        if ((uVar1 & 0x400000) == 0) {
          param_4 = &DAT_00655910;
        }
      }
      else {
        param_4 = &DAT_006562d8;
      }
    }
    iVar4 = FUN_0042bd90_DefWidget_BUTTON_attr_parser(param_4,param_2,param_3);
    if (iVar4 != 0) {
      puVar2 = (undefined4 *)param_1[0x27];
      if (puVar2 != (undefined4 *)0x0) {
        if (param_1[0x18] == -10000) {
          iVar4 = puVar2[3];
          param_1[0x18] = -puVar2[2];
          param_1[0x19] = -iVar4;
        }
        if (param_1[0x1a] == -10000) {
          uVar3 = puVar2[1];
          param_1[0x1a] = *puVar2;
          param_1[0x1b] = uVar3;
        }
      }
      uVar1 = param_1[5];
      param_1[5] = uVar1 & 0xfffffffe;
      if (-1 < (int)param_1[0x1d]) {
        param_1[5] = uVar1 & 0xfffffffe | 0x80000;
      }
      if ((param_1[5] & 0x80000) != 0) {
        param_1[5] = param_1[5] | 0x40000;
      }
    }
  }
  ExceptionList = local_c;
  return param_1;
}


