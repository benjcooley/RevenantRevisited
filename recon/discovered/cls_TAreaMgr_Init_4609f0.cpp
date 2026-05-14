// FUN_004609f0 @ 004609f0 size=534

undefined4 __thiscall FUN_004609f0(int *param_1,int param_2)

{
  char cVar1;
  byte bVar2;
  byte *pbVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  char *pcVar8;
  undefined4 local_108;
  char local_104 [258];
  char acStack_2 [2];
  
  if (((param_2 < 0) || (param_1[1] <= param_2)) || (*param_1 == 0)) {
    return 0;
  }
  if (param_2 == param_1[7]) {
    return 1;
  }
  pbVar3 = *(byte **)(param_1[5] + param_2 * 4);
  if (pbVar3 == (byte *)0x0) {
    return 0;
  }
  local_108 = param_1;
  if (-1 < param_1[7]) {
    if (DAT_0065b488 != 0) {
      FUN_0049eff0();
    }
    DAT_0065b8f4 = 0;
    FUN_0049d650();
    FUN_0049b400();
    param_1[7] = -1;
  }
  bVar2 = *pbVar3;
  param_1[7] = param_2;
  if ((bVar2 & 1) != 0) {
    return 0;
  }
  FUN_00483120(&DAT_0065d6a4,local_104,0x104);
  uVar6 = 0xffffffff;
  pcVar8 = local_104;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar8 + 1;
  } while (cVar1 != '\0');
  iVar5 = -(~uVar6 - 1);
  _strncpy(local_104 + (~uVar6 - 1),(char *)(pbVar3 + 0x58),iVar5 + 0x103);
  (local_104 + (~uVar6 - 1))[iVar5 + 0x103] = '\0';
  uVar6 = 0xffffffff;
  pcVar8 = local_104;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar8 + 1;
  } while (cVar1 != '\0');
  uVar7 = 0xffffffff;
  local_104[~uVar6 - 1] = '\0';
  pcVar8 = local_104;
  do {
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar8 + 1;
  } while (cVar1 != '\0');
  iVar5 = -(~uVar7 - 1);
  _strncpy(local_104 + (~uVar7 - 1),&DAT_005d0dcc,iVar5 + 0x103);
  (local_104 + (~uVar7 - 1))[iVar5 + 0x103] = '\0';
  FUN_0049ee20(local_104,1);
  if (DAT_0065b2fc == 2) {
    iVar5 = 0;
LAB_00460b33:
    if ((DAT_0065c3c8 & 8) != 0) goto LAB_00460b4c;
    if (iVar5 == 0) goto LAB_00460b5e;
    pcVar8 = s_Error_in_module_file__d_005d0e00;
  }
  else {
    iVar5 = DAT_0065b2fc;
    if (DAT_0065b2fc != 10) goto LAB_00460b33;
LAB_00460b4c:
    pcVar8 = s_Compressed_files_found_in_module_005d0dd4;
  }
  FUN_00481c10(pcVar8,local_104);
LAB_00460b5e:
  uVar7 = 0xffffffff;
  local_104[~uVar6 - 1] = '\0';
  pcVar8 = local_104;
  do {
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar8 + 1;
  } while (cVar1 != '\0');
  iVar5 = -(~uVar7 - 1);
  _strncpy(local_104 + (~uVar7 - 1),&DAT_005d0e18,iVar5 + 0x103);
  (local_104 + (~uVar7 - 1))[iVar5 + 0x103] = '\0';
  _strncpy(&DAT_0065b8f4,local_104,0x103);
  piVar4 = local_108;
  DAT_0065b9f7 = 0;
  _strncpy((char *)(local_108 + 8),local_104,0x103);
  *(undefined1 *)((int)piVar4 + 0x123) = 0;
  FUN_0049d2a0();
  FUN_0049b220();
  FUN_0041c000_TAreaMgr_Load();
  return 1;
}


