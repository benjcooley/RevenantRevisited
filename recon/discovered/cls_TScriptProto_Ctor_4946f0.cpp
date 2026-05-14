// FUN_004946f0_TScriptProto_Ctor @ 004946f0 size=488

undefined4 * __thiscall
FUN_004946f0_TScriptProto_Ctor(undefined4 *param_1,undefined4 param_2,undefined4 param_3,char *param_4,char *param_5)

{
  char cVar1;
  undefined4 *puVar2;
  char *pcVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  char *pcVar7;
  undefined **local_60;
  char *local_5c;
  char *local_58;
  char *local_54;
  char *local_50;
  undefined4 local_4c;
  int local_48;
  int local_44;
  undefined ***local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_2c;
  undefined4 local_28;
  undefined1 *local_24;
  undefined1 local_20;
  undefined4 local_1c;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d9cd;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0041c7f0(0,4);
  local_4 = 0;
  puVar2 = (undefined4 *)FUN_00482fb0(0x50);
  param_1[8] = puVar2;
  for (iVar4 = 0x14; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  local_4 = 1;
  FUN_0041c7f0(0,4);
  param_1[2] = param_2;
  param_1[0xe] = param_3;
  uVar5 = 0xffffffff;
  pcVar3 = param_4;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar1 = *pcVar3;
    pcVar3 = pcVar3 + 1;
  } while (cVar1 != '\0');
  local_4._0_1_ = 2;
  pcVar3 = (char *)FUN_00482fb0(~uVar5);
  uVar5 = 0xffffffff;
  param_1[0xf] = pcVar3;
  do {
    pcVar7 = param_4;
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    pcVar7 = param_4 + 1;
    cVar1 = *param_4;
    param_4 = pcVar7;
  } while (cVar1 != '\0');
  uVar5 = ~uVar5;
  pcVar7 = pcVar7 + -uVar5;
  for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    *(undefined4 *)pcVar3 = *(undefined4 *)pcVar7;
    pcVar7 = pcVar7 + 4;
    pcVar3 = pcVar3 + 4;
  }
  for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
    *pcVar3 = *pcVar7;
    pcVar7 = pcVar7 + 1;
    pcVar3 = pcVar3 + 1;
  }
  *param_1 = 0;
  param_1[1] = 0;
  param_1[0x10] = 0;
  param_1[0x12] = 0;
  if (param_5 != (char *)0x0) {
    uVar5 = 0xffffffff;
    pcVar3 = param_5;
    do {
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      cVar1 = *pcVar3;
      pcVar3 = pcVar3 + 1;
    } while (cVar1 != '\0');
    if (0 < (int)(~uVar5 - 1)) {
      local_54 = param_5 + (~uVar5 - 1);
      local_60 = &PTR_LAB_005a36f8;
      local_58 = param_5;
      local_50 = param_5;
      local_5c = s_String_005da20c;
      FUN_00478720();
      local_40 = &local_60;
      local_4._0_1_ = 3;
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
      local_4._0_1_ = 4;
      FUN_00478a10();
      FUN_00494e20_TScriptProto_ParseScript(&local_4c);
      local_4 = CONCAT31(local_4._1_3_,5);
      FUN_004830f0(local_24);
      if (local_48 == 0) {
        if (local_44 != 0) {
          FUN_004830f0(local_40);
          FUN_004a1540(local_44);
        }
      }
      else {
        FUN_004830f0(local_40);
        FUN_004830f0(local_48);
      }
      local_4 = CONCAT31(local_4._1_3_,2);
      FUN_00478730();
    }
  }
  ExceptionList = local_c;
  return param_1;
}


