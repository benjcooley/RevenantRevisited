// FUN_004377c0_DefWidget_LoadFile @ 004377c0 size=485

undefined4 __thiscall FUN_004377c0_DefWidget_LoadFile(int param_1,char *param_2)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 *puVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  char *pcVar11;
  undefined1 local_104 [260];
  
  if (*(int *)(param_1 + 200) != 0) {
    FUN_004830f0(*(int *)(param_1 + 200));
    *(undefined4 *)(param_1 + 200) = 0;
    *(undefined4 *)(param_1 + 0xcc) = 0;
  }
  *(undefined4 *)(param_1 + 0x178) = 0;
  *(undefined4 *)(param_1 + 0x150) = 1;
  FUN_0058b100(local_104,&DAT_005cdbd8,&DAT_0065dde8,s_widgets_def_005cdbcc);
  iVar3 = FUN_004a13f0(local_104,&DAT_005cdbe0,1);
  if (iVar3 == 0) {
    FUN_00481c10(s_Unable_to_load_widgets_def_005cdbe4,0);
  }
  iVar4 = FUN_004a17b0(iVar3);
  FUN_0058b100(local_104,&DAT_005cdc00,&DAT_0065dde8,param_2);
  iVar5 = FUN_004a13f0(local_104,&DAT_005cdc08,1);
  if (iVar5 == 0) {
    FUN_00481c10(s_Unable_to_load__s_005cdc0c,param_2);
  }
  iVar6 = FUN_004a17b0(iVar5);
  iVar8 = iVar6 + 1 + iVar4;
  *(int *)(param_1 + 0xcc) = iVar8;
  puVar7 = (undefined4 *)FUN_00482fb0(iVar8 + 1);
  uVar9 = *(int *)(param_1 + 0xcc) + 1;
  *(undefined4 **)(param_1 + 200) = puVar7;
  for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
    *puVar7 = 0;
    puVar7 = puVar7 + 1;
  }
  for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
    *(undefined1 *)puVar7 = 0;
    puVar7 = (undefined4 *)((int)puVar7 + 1);
  }
  FUN_004a15a0(*(undefined4 *)(param_1 + 200),iVar4,1,iVar3);
  iVar8 = FUN_004834e0(*(undefined4 *)(param_1 + 200),iVar4);
  if (iVar8 != 0) {
    FUN_00483540(*(undefined4 *)(param_1 + 200),iVar4);
  }
  pcVar2 = *(char **)(param_1 + 200);
  uVar10 = 0xffffffff;
  pcVar11 = pcVar2;
  do {
    if (uVar10 == 0) break;
    uVar10 = uVar10 - 1;
    cVar1 = *pcVar11;
    pcVar11 = pcVar11 + 1;
  } while (cVar1 != '\0');
  uVar10 = ~uVar10;
  pcVar2[uVar10 - 1] = '\n';
  FUN_004a15a0(pcVar2 + uVar10,iVar6,1,iVar5);
  iVar4 = FUN_004834e0(uVar10 + *(int *)(param_1 + 200),iVar6);
  if (iVar4 != 0) {
    FUN_00483540(uVar10 + *(int *)(param_1 + 200),iVar6);
  }
  *(undefined1 *)(*(int *)(param_1 + 0xcc) + *(int *)(param_1 + 200)) = 0;
  FUN_004a1540(iVar3);
  FUN_004a1540(iVar5);
  _strncpy((char *)(param_1 + 0xd0),param_2,0x7f);
  *(undefined1 *)(param_1 + 0x14f) = 0;
  return 1;
}


