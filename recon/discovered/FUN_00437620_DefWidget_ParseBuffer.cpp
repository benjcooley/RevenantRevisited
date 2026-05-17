// FUN_00437620_DefWidget_ParseBuffer @ 00437620 size=413

int __fastcall FUN_00437620_DefWidget_ParseBuffer(int *param_1)

{
  int iVar1;
  undefined **local_60;
  char *local_5c;
  int local_58;
  int local_54;
  int local_50;
  int local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined ***local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_2c;
  undefined4 local_28;
  undefined1 *local_24;
  undefined1 local_20;
  undefined4 local_1c;
  void *local_14;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059cc38;
  local_c = ExceptionList;
  local_58 = param_1[0x32];
  if ((local_58 != 0) && (0 < param_1[0x33])) {
    local_54 = param_1[0x33] + local_58;
    ExceptionList = &local_c;
    param_1[0x54] = 0;
    param_1[0x27] = 0;
    param_1[0x29] = 0;
    param_1[0x28] = 0;
    param_1[0x2a] = 0;
    local_60 = &PTR_LAB_005a36f8;
    local_5c = s_String_005cdbc4;
    local_50 = local_58;
    FUN_00478720();
    local_40 = &local_60;
    local_4 = 0;
    local_4c = 0;
    local_48 = 0;
    local_44 = 0;
    local_3c = 0;
    local_38 = 0;
    local_34 = 0;
    local_2c = 0;
    local_28 = 0;
    local_20 = 0;
    local_1c = 1;
    local_24 = (undefined1 *)FUN_00482fb0(0x2000);
    *local_24 = 0;
    local_4 = 1;
    FUN_00479450();
    param_1[0x55] = (int)&local_4c;
    iVar1 = (**(code **)(*param_1 + 0xb4))(&local_4c,param_1 + 0x56);
    param_1[0x55] = 0;
    if (iVar1 != 0) {
      (**(code **)(*param_1 + 0x94))(0,1);
    }
    local_c = (void *)0x2;
    FUN_004830f0(local_2c);
    if (local_50 == 0) {
      if (local_4c != 0) {
        FUN_004830f0(local_48);
        FUN_004a1540(local_4c);
      }
    }
    else {
      FUN_004830f0(local_48);
      FUN_004830f0(local_50);
    }
    local_c = (void *)0xffffffff;
    FUN_00478730();
    ExceptionList = local_14;
    return iVar1;
  }
  return 0;
}


