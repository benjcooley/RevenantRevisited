// FUN_00431ca0_DefWidget_TDefEdit_ctor @ 00431ca0 size=487

undefined4 * __thiscall
FUN_00431ca0_DefWidget_TDefEdit_ctor(undefined4 *param_1,int *param_2,undefined4 param_3,undefined *param_4)

{
  uint uVar1;
  size_t _Count;
  undefined4 uVar2;
  undefined4 *puVar3;
  int iVar4;
  char *_Dest;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059cb24;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0042a350_DefWidget_TWidgetBase_ParseAttrs(param_2,7,param_3);
  uVar1 = param_1[5];
  local_4 = 0;
  *param_1 = &PTR_FUN_005a3db0;
  param_1[0x25] = 0;
  param_1[0x24] = 0;
  if ((uVar1 & 1) == 0) {
    param_1[5] = uVar1 | 1;
    if ((param_4 == (undefined *)0x0) && (param_4 = &DAT_00655ab8, (uVar1 & 0x40000) == 0)) {
      param_4 = &DAT_00656180;
    }
    iVar4 = FUN_00431750_DefWidget_EDIT_attr_parser(param_4,param_2,param_3);
    if (iVar4 != 0) {
      iVar4 = FUN_00479700(s_MAXLEN_005cd76c,0);
      if (iVar4 == 0) {
        param_1[0x26] = 0x14;
      }
      else {
        FUN_00479580();
        iVar4 = FUN_0047a410(param_3,&DAT_005cd774,param_1 + 0x26);
        if (iVar4 == 0) {
          (**(code **)(*param_2 + 0x9c))(s_MAXLEN_005cd778);
          ExceptionList = local_c;
          return param_1;
        }
      }
      param_1[0x28] = param_1[0x44];
      if (param_1[0x4b] == 0) {
        iVar4 = param_1[0x49];
      }
      else {
        iVar4 = param_1[0x1a] - param_1[0x49];
      }
      param_1[0x2c] = iVar4 + param_1[0x18];
      param_1[0x2d] = param_1[0x4a] + param_1[0x19];
      if (param_1[0x50] == 0) {
        iVar4 = param_1[0x4e];
      }
      else {
        iVar4 = param_1[0x1a] - param_1[0x4e];
      }
      param_1[0x2a] = iVar4 + param_1[0x18];
      param_1[0x2b] = param_1[0x4f] + param_1[0x19];
      _Dest = (char *)FUN_00482ef0(param_1[0x26] + 1);
      if ((char *)param_1[0x17] == (char *)0x0) {
        *_Dest = '\0';
      }
      else {
        _Count = param_1[0x26];
        _strncpy(_Dest,(char *)param_1[0x17],_Count);
        uVar2 = param_1[0x17];
        _Dest[_Count] = '\0';
        FUN_00482f80(uVar2);
      }
      puVar3 = (undefined4 *)param_1[0x2e];
      param_1[0x17] = _Dest;
      if (puVar3 != (undefined4 *)0x0) {
        if (param_1[0x18] == -10000) {
          param_1[0x18] = -puVar3[2];
          param_1[0x19] = -puVar3[3];
        }
        if (param_1[0x1a] == -10000) {
          uVar2 = puVar3[1];
          param_1[0x1a] = *puVar3;
          param_1[0x1b] = uVar2;
        }
      }
      param_1[5] = param_1[5] & 0xfffffffe;
    }
  }
  ExceptionList = local_c;
  return param_1;
}


