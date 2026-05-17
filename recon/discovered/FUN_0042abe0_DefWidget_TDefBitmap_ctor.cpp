// FUN_0042abe0_DefWidget_TDefBitmap_ctor @ 0042abe0 size=204

undefined4 * __thiscall
FUN_0042abe0_DefWidget_TDefBitmap_ctor(undefined4 *param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  int iVar3;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ca28;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0042a350_DefWidget_TWidgetBase_ParseAttrs(param_2,1,param_3);
  local_4 = 0;
  *param_1 = &PTR_FUN_005a3b24;
  param_1[5] = param_1[5] | 1;
  if (param_4 == 0) {
    param_1[0x24] = 0;
    param_1[0x25] = 0;
    param_1[0x25] = 0x80000000;
  }
  iVar3 = FUN_0042aaf0(param_4,param_2,param_3);
  if (iVar3 != 0) {
    puVar1 = (undefined4 *)param_1[0x24];
    if (puVar1 != (undefined4 *)0x0) {
      if (param_1[0x18] == -10000) {
        iVar3 = puVar1[3];
        param_1[0x18] = -puVar1[2];
        param_1[0x19] = -iVar3;
      }
      if (param_1[0x1a] == -10000) {
        uVar2 = puVar1[1];
        param_1[0x1a] = *puVar1;
        param_1[0x1b] = uVar2;
      }
    }
    param_1[5] = param_1[5] & 0xfffffffe;
  }
  ExceptionList = local_c;
  return param_1;
}


