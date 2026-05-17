// FUN_0042df00_DefWidget_TDefScrollbar_ctor @ 0042df00 size=220

undefined4 * __thiscall
FUN_0042df00_DefWidget_TDefScrollbar_ctor(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 *param_4)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059caa6;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0042a350_DefWidget_TWidgetBase_ParseAttrs(param_2,5,param_3);
  uVar1 = param_1[5];
  local_4 = 0;
  *param_1 = &PTR_FUN_005a3cd8;
  param_1[0x24] = 0;
  param_1[0x25] = 0;
  param_1[0x26] = 0;
  param_1[0x27] = 1000;
  param_1[0x28] = 100;
  if ((uVar1 & 1) == 0) {
    if (param_4 == (undefined4 *)0x0) {
      puVar4 = param_1 + 0x29;
      iVar2 = 0x21;
      puVar3 = &DAT_00655d38;
      if ((uVar1 & 0x20000) == 0) {
        puVar3 = &DAT_006560f8;
      }
    }
    else {
      iVar2 = 0x21;
      puVar3 = param_4;
      puVar4 = param_1 + 0x29;
    }
    for (; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *puVar3;
      puVar3 = puVar3 + 1;
      puVar4 = puVar4 + 1;
    }
    param_1[5] = uVar1 | 1;
    iVar2 = FUN_0042dfe0(param_2,param_3,param_4);
    if (iVar2 != 0) {
      param_1[5] = param_1[5] & 0xfffffffe;
      FUN_0042e240();
    }
  }
  ExceptionList = local_c;
  return param_1;
}


