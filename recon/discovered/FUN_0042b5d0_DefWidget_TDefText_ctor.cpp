// FUN_0042b5d0_DefWidget_TDefText_ctor @ 0042b5d0 size=295

undefined4 * __thiscall
FUN_0042b5d0_DefWidget_TDefText_ctor(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined *param_4)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  int iVar4;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ca4c;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0042a350_DefWidget_TWidgetBase_ParseAttrs(param_2,3,param_3);
  local_4 = 0;
  *param_1 = &PTR_FUN_005a3bfc;
  param_1[5] = param_1[5] | 1;
  if (param_4 == (undefined *)0x0) {
    param_4 = &DAT_006554b8;
  }
  iVar4 = FUN_0042b340_DefWidget_TEXT_attr_parser(param_4,param_2,param_3);
  if (iVar4 != 0) {
    uVar1 = param_1[5];
    if ((uVar1 & 0x40000) == 0) {
      if ((uVar1 & 0x80000) != 0) {
        param_1[0x34] = param_1[0x34] & 0xfffffffc | 4;
      }
    }
    else {
      param_1[0x34] = param_1[0x34] & 0xfffffffa | 2;
    }
    if ((uVar1 & 0x100000) == 0) {
      if ((uVar1 & 0x200000) != 0) {
        param_1[0x34] = param_1[0x34] & 0xffffffaf | 0x20;
      }
    }
    else {
      param_1[0x34] = param_1[0x34] & 0xffffff9f | 0x10;
    }
    puVar2 = (undefined4 *)param_1[0x24];
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
    param_1[5] = uVar1 & 0xfffffffe;
  }
  ExceptionList = local_c;
  return param_1;
}


