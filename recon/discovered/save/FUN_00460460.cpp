// FUN_00460460 @ 00460460 size=318

int __thiscall FUN_00460460(int param_1,int param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  undefined4 *puVar6;
  char *pcVar7;
  undefined4 *puVar8;
  char local_104 [258];
  char acStack_2 [2];
  
  FUN_00483120(&DAT_0065d6a4,local_104,0x104);
  uVar3 = 0xffffffff;
  pcVar7 = local_104;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  iVar2 = -(~uVar3 - 1);
  _strncpy(local_104 + (~uVar3 - 1),(char *)(param_1 + 0x58),iVar2 + 0x103);
  uVar4 = 0xffffffff;
  (local_104 + (~uVar3 - 1))[iVar2 + 0x103] = '\0';
  pcVar7 = local_104;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  iVar2 = -(~uVar4 - 1);
  _strncpy(local_104 + (~uVar4 - 1),&DAT_005d0d1c,iVar2 + 0x103);
  (local_104 + (~uVar4 - 1))[iVar2 + 0x103] = '\0';
  uVar3 = 0xffffffff;
  pcVar7 = local_104;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  iVar2 = -(~uVar3 - 1);
  _strncpy(local_104 + (~uVar3 - 1),(char *)**(undefined4 **)(param_2 + 0x4c),iVar2 + 0x103);
  (local_104 + (~uVar3 - 1))[iVar2 + 0x103] = '\0';
  uVar3 = 0xffffffff;
  pcVar7 = local_104;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  iVar2 = -(~uVar3 - 1);
  _strncpy(local_104 + (~uVar3 - 1),&DAT_005d0d20,iVar2 + 0x103);
  (local_104 + (~uVar3 - 1))[iVar2 + 0x103] = '\0';
  iVar2 = FUN_0051e370(local_104,0);
  if (iVar2 == 0) {
    return 0;
  }
  FUN_0046e6f0(*(undefined4 *)(param_2 + 0x38));
  FUN_0051e4d0(param_2 + 0x490);
  puVar6 = (undefined4 *)(param_2 + 0x570);
  puVar8 = (undefined4 *)(iVar2 + 0x570);
  for (iVar5 = 0x38; iVar5 != 0; iVar5 = iVar5 + -1) {
    *puVar8 = *puVar6;
    puVar6 = puVar6 + 1;
    puVar8 = puVar8 + 1;
  }
  return iVar2;
}


