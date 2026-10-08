// FUN_005496a0 @ 005496a0 size=149

void __thiscall FUN_005496a0(int *param_1,int *param_2)

{
  char cVar1;
  char *pcVar2;
  uint uVar3;
  uint uVar4;
  char *pcVar5;
  char *pcVar6;
  
  if ((int *)param_1[0x66] != (int *)0x0) {
    (**(code **)(*(int *)param_1[0x66] + 0xa0))();
  }
  param_1[0x66] = (int)param_2;
  if (param_1[0x67] != 0) {
    FUN_00482f80(param_1[0x67]);
  }
  uVar3 = 0xffffffff;
  pcVar5 = &DAT_005e5714;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  pcVar2 = (char *)FUN_00482ef0(~uVar3);
  uVar3 = 0xffffffff;
  pcVar5 = &DAT_005e570c;
  do {
    pcVar6 = pcVar5;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar6 = pcVar5 + 1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar6;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  pcVar5 = pcVar6 + -uVar3;
  pcVar6 = pcVar2;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined4 *)pcVar6 = *(undefined4 *)pcVar5;
    pcVar5 = pcVar5 + 4;
    pcVar6 = pcVar6 + 4;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pcVar6 = *pcVar5;
    pcVar5 = pcVar5 + 1;
    pcVar6 = pcVar6 + 1;
  }
  param_1[0x67] = (int)pcVar2;
  (**(code **)(*param_1 + 0x2c))(1);
  if (param_2 != (int *)0x0) {
    (**(code **)(*param_2 + 0x9c))();
  }
  return;
}


