// FUN_0054d2f0 @ 0054d2f0 size=157

void __fastcall FUN_0054d2f0(int param_1)

{
  char cVar1;
  char *pcVar2;
  uint uVar3;
  int iVar4;
  char *pcVar5;
  
  if (*(int *)(param_1 + 0xa0) == 0) {
    DAT_0065a9c8 = DAT_0065a9c8 | DAT_0065a9c4;
    DAT_0065a9c4 = 0;
    if (DAT_00667fcc != 0) {
      FUN_004cee70(0);
      FUN_004cf000();
    }
    pcVar2 = (char *)FUN_0049d800(s_msgprefix_005e5868);
    uVar3 = 0xffffffff;
    pcVar5 = pcVar2;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar5;
      pcVar5 = pcVar5 + 1;
    } while (cVar1 != '\0');
    iVar4 = ~uVar3 - 1;
    *(int *)(param_1 + 0xa4) = 0x50 - iVar4;
    *(undefined1 *)(param_1 + 0xd0) = 0;
    FUN_00444e20(0);
    FUN_0054d0c0(0x20,iVar4,pcVar2);
    *(undefined4 *)(param_1 + 0xa0) = 1;
  }
  return;
}


