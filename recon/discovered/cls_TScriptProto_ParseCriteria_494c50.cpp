// FUN_00494c50_TScriptProto_ParseCriteria @ 00494c50 size=464

undefined4 __thiscall FUN_00494c50_TScriptProto_ParseCriteria(int *param_1,int param_2)

{
  char cVar1;
  undefined4 uVar2;
  int iVar3;
  char *pcVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  char *pcVar8;
  char *pcVar9;
  undefined1 local_64 [100];
  
  iVar3 = FUN_00479700(s_CONTEXT_005da214,0);
  if (iVar3 == 0) {
    iVar3 = FUN_00479700(s_OBJTYPE_005da21c,0);
    if (iVar3 == 0) {
      iVar3 = FUN_00479700(s_OBJECT_005da224,0);
      if (iVar3 == 0) {
        FUN_004795c0();
        return 0;
      }
    }
  }
  FUN_00479580();
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
    FUN_0058b100(local_64,s_Script_error_at_line__d___s_005da0d0,*(undefined4 *)(param_2 + 0x30),
                 s_Expected_object_context_identifi_005da22c);
    if (DAT_00668154 == 0) {
      FUN_0054d170(&DAT_0065c5d0,local_64);
    }
    else {
      FUN_0041ee50(local_64);
    }
  }
  if (*param_1 != 0) {
    FUN_00482f80(*param_1);
    *param_1 = 0;
  }
  pcVar8 = *(char **)(param_2 + 0x28);
  uVar6 = 0xffffffff;
  pcVar4 = pcVar8;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    cVar1 = *pcVar4;
    pcVar4 = pcVar4 + 1;
  } while (cVar1 != '\0');
  pcVar4 = (char *)FUN_00482ef0(~uVar6);
  uVar6 = 0xffffffff;
  do {
    pcVar9 = pcVar8;
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    pcVar9 = pcVar8 + 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar9;
  } while (cVar1 != '\0');
  uVar6 = ~uVar6;
  pcVar8 = pcVar9 + -uVar6;
  pcVar9 = pcVar4;
  for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
    *(undefined4 *)pcVar9 = *(undefined4 *)pcVar8;
    pcVar8 = pcVar8 + 4;
    pcVar9 = pcVar9 + 4;
  }
  for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
    *pcVar9 = *pcVar8;
    pcVar8 = pcVar8 + 1;
    pcVar9 = pcVar9 + 1;
  }
  *param_1 = (int)pcVar4;
  FUN_00479580();
  iVar3 = FUN_00479700(s_PARENT_005da250,0);
  if (iVar3 != 0) {
    FUN_00479580();
    if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
      FUN_0058b100(local_64,s_Script_error_at_line__d___s_005da0d0,*(undefined4 *)(param_2 + 0x30),
                   s_Expected_object_parent_identifie_005da258);
      if (DAT_00668154 == 0) {
        FUN_0054d170(&DAT_0065c5d0,local_64);
      }
      else {
        FUN_0041ee50(local_64);
      }
    }
    uVar2 = *(undefined4 *)(param_2 + 0x28);
    iVar3 = 0;
    if (0 < DAT_00665ef4) {
      do {
        iVar5 = FUN_0059a530_stricmp(**(undefined4 **)(DAT_00665f04 + iVar3 * 4),uVar2);
        if (iVar5 == 0) {
          iVar3 = *(int *)(DAT_00665f04 + iVar3 * 4);
          goto LAB_00494df6;
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 < DAT_00665ef4);
    }
    iVar3 = 0;
LAB_00494df6:
    param_1[2] = iVar3;
    FUN_00479580();
  }
  FUN_004795a0();
  return 1;
}


