// FUN_00529220 @ 00529220 size=220

undefined4 * __thiscall FUN_00529220(undefined4 *param_1,undefined4 *param_2)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined1 local_70 [4];
  int local_6c;
  int local_68;
  undefined1 local_5c [80];
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a17b8;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  *param_1 = *param_2;
  FUN_0049cc20(0x2080,0x20);
  local_4 = 0;
  puVar1 = (undefined4 *)FUN_00529150(param_2[2],param_2[1],local_70);
  if ((local_68 - local_6c) / 2 != 0x1040) {
    FUN_0058b100(local_5c,s_error_decompressing_level_visgri_005e32f8,*param_1);
    FUN_00481c10(local_5c,&DAT_0066f33c);
  }
  iVar4 = 0x41;
  puVar2 = param_1 + 1;
  do {
    iVar4 = iVar4 + -1;
    puVar5 = puVar1;
    puVar6 = puVar2;
    for (iVar3 = 0x20; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar6 = *puVar5;
      puVar5 = puVar5 + 1;
      puVar6 = puVar6 + 1;
    }
    puVar2 = puVar2 + 0x20;
    puVar1 = puVar1 + 0x20;
  } while (iVar4 != 0);
  local_4 = 0xffffffff;
  FUN_0049cc50();
  ExceptionList = pvStack_c;
  return param_1;
}


