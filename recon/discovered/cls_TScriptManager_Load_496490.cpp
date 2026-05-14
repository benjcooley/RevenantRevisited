// FUN_00496490_TScriptManager_Load @ 00496490 size=501

undefined4 __thiscall FUN_00496490_TScriptManager_Load(int param_1,char *param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  char *_Dest;
  int iVar4;
  undefined4 local_20c;
  undefined1 local_208 [260];
  undefined1 local_104 [260];
  
  if (DAT_0065a784 < 0) {
    iVar4 = 0;
  }
  else {
    iVar4 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
  }
  FUN_0058b100(local_104,&DAT_005da54c,&DAT_0065bd48,param_2);
  if (iVar4 != 0) {
    if (DAT_0065a784 < 0) {
      iVar4 = 0;
    }
    else {
      iVar4 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    FUN_0058b100(local_208,s__s_s__s_005da554,&DAT_0065d6a4,iVar4 + 0x58,param_2);
  }
  iVar4 = FUN_004a13f0(local_208,&DAT_005da55c,0);
  if (iVar4 == 0) {
    iVar4 = FUN_004a13f0(local_104,&DAT_005da560,0);
    if (iVar4 == 0) {
      FUN_00481c10(s_Unable_to_find_game_script_file___005da564,param_2);
    }
  }
  local_20c = 1;
  iVar1 = FUN_004a17b0(iVar4);
  iVar2 = FUN_00482fb0(iVar1 + 1);
  iVar3 = FUN_004a15a0(iVar2,1,iVar1,iVar4);
  if (iVar3 < iVar1) {
    local_20c = 0;
  }
  else {
    *(undefined1 *)(iVar2 + iVar1) = 0;
    iVar3 = FUN_004834e0(iVar2,iVar1);
    if (iVar3 != 0) {
      FUN_00483540(iVar2,iVar1);
    }
    FUN_00496860_TScriptManager_ParseScripts(iVar2,param_2,param_3);
  }
  FUN_004830f0(iVar2);
  FUN_004a1540(iVar4);
  iVar4 = 0;
  if (0 < *(int *)(param_1 + 0x802c)) {
    do {
      iVar1 = FUN_0059a530_stricmp(*(undefined4 *)(*(int *)(param_1 + 0x803c) + iVar4 * 4),param_2);
      if (iVar1 == 0) {
        if ((-1 < iVar4) && (iVar4 = *(int *)(*(int *)(param_1 + 0x803c) + iVar4 * 4), iVar4 != 0))
        {
          *(undefined4 *)(iVar4 + 0x80) = param_3;
          goto LAB_00496638;
        }
        break;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)(param_1 + 0x802c));
  }
  _Dest = (char *)FUN_00482fb0(0x84);
  _strncpy(_Dest,param_2,0x7f);
  _Dest[0x7f] = '\0';
  *(undefined4 *)(_Dest + 0x80) = param_3;
  FUN_0041c840(_Dest);
LAB_00496638:
  if (DAT_00666918 != 0) {
    FUN_0045a680(8,0);
  }
  *(undefined4 *)(param_1 + 0x8040) = 0;
  return local_20c;
}


