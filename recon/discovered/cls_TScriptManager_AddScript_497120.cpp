// FUN_00497120_TScriptManager_AddScript @ 00497120 size=579

int __thiscall
FUN_00497120_TScriptManager_AddScript(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5
            )

{
  char cVar1;
  void **ppvVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  char *pcVar8;
  undefined4 local_e4;
  int local_e0;
  int local_dc;
  undefined ***local_d8;
  undefined4 local_d4;
  undefined4 local_d0;
  undefined4 local_cc;
  undefined4 local_c4;
  undefined4 local_c0;
  undefined1 *local_bc;
  undefined1 local_b8;
  undefined4 local_b4;
  undefined **local_a4;
  char *local_a0;
  char *local_9c;
  char *local_98;
  char *local_94;
  undefined4 local_90;
  char local_8c [128];
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059dab0;
  local_c = ExceptionList;
  iVar6 = 0;
  iVar7 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < *(int *)(param_1 + 0x8004)) {
    do {
      ExceptionList = ppvVar2;
      iVar3 = FUN_0059a530_stricmp(**(undefined4 **)(*(int *)(param_1 + 0x8014) + iVar7 * 4),param_3);
      if (iVar3 == 0) {
        iVar7 = *(int *)(*(int *)(param_1 + 0x8014) + iVar7 * 4);
        if (iVar7 != 0) goto LAB_00497314;
        break;
      }
      iVar7 = iVar7 + 1;
      ppvVar2 = ExceptionList;
    } while (iVar7 < *(int *)(param_1 + 0x8004));
  }
  FUN_0058b100(local_8c,s_OBJECT__s_BEGIN_END_005da6b4,param_3);
  uVar5 = 0xffffffff;
  pcVar8 = local_8c;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar8 + 1;
  } while (cVar1 != '\0');
  local_9c = local_8c;
  local_94 = local_8c;
  local_a4 = &PTR_LAB_005a36f8;
  local_98 = local_8c + (~uVar5 - 1);
  local_a0 = s_String_005da6d0;
  FUN_00478720();
  local_d8 = &local_a4;
  local_4 = 0;
  local_e4 = 0;
  local_e0 = 0;
  local_dc = 0;
  local_d4 = 0;
  local_d0 = 0;
  local_cc = 0;
  local_c4 = 0;
  local_c0 = 0;
  local_b8 = 0;
  local_b4 = 1;
  local_bc = (undefined1 *)FUN_00482fb0(0x2000);
  *local_bc = 0;
  local_4 = 1;
  FUN_00478a10();
  local_90 = FUN_00482fb0(0x4c);
  local_4._0_1_ = 2;
  if (local_90 == 0) {
    iVar7 = 0;
  }
  else {
    iVar7 = FUN_004946f0_TScriptProto_Ctor(0,param_4,param_5,0);
  }
  local_4 = CONCAT31(local_4._1_3_,1);
  FUN_00494e20_TScriptProto_ParseScript(&local_e4);
  FUN_0041c840(iVar7);
  local_4 = 3;
  FUN_004830f0(local_bc);
  if (local_e0 == 0) {
    if (local_dc != 0) {
      FUN_004830f0(local_d8);
      FUN_004a1540(local_dc);
    }
  }
  else {
    FUN_004830f0(local_d8);
    FUN_004830f0(local_e0);
  }
  local_4 = 0xffffffff;
  FUN_00478730();
LAB_00497314:
  iVar3 = FUN_00482fb0(0xe8);
  if (iVar3 != 0) {
    iVar6 = FUN_00492170_TScript_Ctor(param_2,iVar7);
  }
  uVar4 = FUN_0041c840(iVar6);
  *(undefined4 *)(iVar6 + 0x14) = uVar4;
  ExceptionList = local_c;
  return iVar6;
}


