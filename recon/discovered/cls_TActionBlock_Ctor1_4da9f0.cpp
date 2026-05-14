// FUN_004da9f0_TActionBlock_Ctor1 @ 004da9f0 size=113

undefined4 * __thiscall FUN_004da9f0_TActionBlock_Ctor1(undefined4 *param_1,char *param_2,undefined4 param_3)

{
  char cVar1;
  uint uVar2;
  uint uVar3;
  char *pcVar4;
  char *pcVar5;
  
  uVar2 = 0xffffffff;
  param_1[9] = 0xffffffff;
  do {
    pcVar4 = param_2;
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    pcVar4 = param_2 + 1;
    cVar1 = *param_2;
    param_2 = pcVar4;
  } while (cVar1 != '\0');
  param_1[10] = 0;
  uVar2 = ~uVar2;
  *(char *)(param_1 + 1) = '\0';
  param_1[0xc] = 0;
  param_1[0xb] = 0;
  param_1[0x10] = 0;
  param_1[0xf] = 0;
  param_1[0xe] = 0;
  param_1[0x11] = 0;
  param_1[0x17] = 0;
  param_1[0x14] = 0;
  param_1[0x12] = 0;
  param_1[0x13] = 0;
  *param_1 = 1;
  param_1[0xd] = 0x10;
  param_1[0x18] = 1;
  pcVar4 = pcVar4 + -uVar2;
  pcVar5 = (char *)(param_1 + 1);
  for (uVar3 = uVar2 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined4 *)pcVar5 = *(undefined4 *)pcVar4;
    pcVar4 = pcVar4 + 4;
    pcVar5 = pcVar5 + 4;
  }
  for (uVar2 = uVar2 & 3; uVar2 != 0; uVar2 = uVar2 - 1) {
    *pcVar5 = *pcVar4;
    pcVar4 = pcVar4 + 1;
    pcVar5 = pcVar5 + 1;
  }
  *param_1 = param_3;
  return param_1;
}


