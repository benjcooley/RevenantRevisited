// FUN_0054d390 @ 0054d390 size=262

void __fastcall FUN_0054d390(int param_1)

{
  char cVar1;
  undefined4 *puVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  char *pcVar6;
  char *pcVar7;
  undefined4 local_4;
  
  iVar3 = DAT_00667fcc;
  if (*(int *)(param_1 + 0xa0) != 0) {
    puVar2 = *(undefined4 **)(param_1 + 0x6c);
    *(undefined4 *)(param_1 + 0xa0) = 0;
    cVar1 = *(char *)(iVar3 + 0x494);
    *puVar2 = 0x40;
    local_4 = DAT_00670660;
    if (cVar1 != '\0') {
      if (cVar1 == '\0') {
        local_4 = 0xffffff;
      }
      else {
        iVar5 = *(int *)(iVar3 + 0x4d8);
        if (0xf < iVar5) {
          iVar5 = 0x10;
        }
        local_4 = *(undefined4 *)(&DAT_005e20b8 + iVar5 * 4);
      }
    }
    puVar2[1] = local_4;
    _strncpy((char *)(puVar2 + 3),*(char **)(iVar3 + 0x38),0x4f);
    iVar3 = *(int *)(param_1 + 0x6c);
    *(undefined1 *)((int)puVar2 + 0x5b) = 0;
    pcVar6 = (char *)(iVar3 + 0xc);
    uVar4 = 0xffffffff;
    pcVar7 = pcVar6;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar1 = *pcVar7;
      pcVar7 = pcVar7 + 1;
    } while (cVar1 != '\0');
    iVar5 = -(~uVar4 - 1);
    pcVar6 = pcVar6 + (~uVar4 - 1);
    _strncpy(pcVar6,&DAT_005e5874,iVar5 + 0x4f);
    iVar3 = *(int *)(param_1 + 0x6c);
    pcVar6[iVar5 + 0x4f] = '\0';
    pcVar6 = (char *)(iVar3 + 0xc);
    uVar4 = 0xffffffff;
    pcVar7 = pcVar6;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar1 = *pcVar7;
      pcVar7 = pcVar7 + 1;
    } while (cVar1 != '\0');
    iVar3 = -(~uVar4 - 1);
    pcVar6 = pcVar6 + (~uVar4 - 1);
    _strncpy(pcVar6,(char *)(param_1 + 0xd0),iVar3 + 0x4f);
    pcVar6[iVar3 + 0x4f] = '\0';
    FUN_0054cd40(0);
    FUN_0054d700((char *)(param_1 + 0xd0));
  }
  return;
}


