// FUN_00533dd0 @ 00533dd0 size=319

char * FUN_00533dd0(char *param_1,char *param_2,int param_3)

{
  char cVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  char *pcVar5;
  uint uVar6;
  uint uVar7;
  char *pcVar8;
  char *pcVar9;
  char local_214 [20];
  char local_200;
  char local_1ff [255];
  char local_100 [256];
  
  cVar2 = *param_1;
  pcVar8 = local_100;
  do {
    if (cVar2 == '\0') {
      *pcVar8 = '\0';
      _strncpy(param_2,local_100,param_3 - 1);
      param_2[param_3 + -1] = '\0';
      return param_2;
    }
    cVar2 = *param_1;
    if (cVar2 < '\0') {
      if (cVar2 != '[') {
LAB_00533ebb:
        *pcVar8 = cVar2;
        goto LAB_00533ecb;
      }
      cVar2 = param_1[1];
      param_1 = param_1 + 1;
      if ((cVar2 == '[') || (cVar2 == ']')) goto LAB_00533ebb;
      pcVar5 = local_214;
      while ((cVar2 != '\0' && (cVar2 != ']'))) {
        *pcVar5 = cVar2;
        pcVar9 = param_1 + 1;
        pcVar5 = pcVar5 + 1;
        param_1 = param_1 + 1;
        cVar2 = *pcVar9;
      }
      if (*param_1 == ']') {
        param_1 = param_1 + 1;
      }
      *pcVar5 = '\0';
      local_200 = '\0';
      iVar3 = FUN_0059a530(local_214,&DAT_005e3f3c);
      iVar4 = DAT_00667fcc;
      if ((iVar3 == 0) ||
         ((iVar4 = FUN_0059a530(local_214,&DAT_005e3f40), iVar4 == 0 &&
          (iVar4 = DAT_0066f738, DAT_0066f738 != 0)))) {
        uVar6 = 0xffffffff;
        pcVar5 = *(char **)(iVar4 + 0x38);
        do {
          pcVar9 = pcVar5;
          if (uVar6 == 0) break;
          uVar6 = uVar6 - 1;
          pcVar9 = pcVar5 + 1;
          cVar2 = *pcVar5;
          pcVar5 = pcVar9;
        } while (cVar2 != '\0');
        uVar6 = ~uVar6;
        pcVar5 = pcVar9 + -uVar6;
        pcVar9 = &local_200;
        for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
          *(undefined4 *)pcVar9 = *(undefined4 *)pcVar5;
          pcVar5 = pcVar5 + 4;
          pcVar9 = pcVar9 + 4;
        }
        for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
          *pcVar9 = *pcVar5;
          pcVar5 = pcVar5 + 1;
          pcVar9 = pcVar9 + 1;
        }
      }
      pcVar5 = &local_200;
      cVar2 = local_200;
      while (cVar2 != '\0') {
        *pcVar8 = cVar2;
        pcVar9 = pcVar5 + 1;
        pcVar8 = pcVar8 + 1;
        pcVar5 = pcVar5 + 1;
        cVar2 = *pcVar9;
      }
    }
    else {
      cVar1 = param_1[1];
      *pcVar8 = cVar2;
      pcVar8 = pcVar8 + 1;
      param_1 = param_1 + 1;
      *pcVar8 = cVar1;
LAB_00533ecb:
      pcVar8 = pcVar8 + 1;
      param_1 = param_1 + 1;
    }
    cVar2 = *param_1;
  } while( true );
}


