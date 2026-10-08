// FUN_00427d80 @ 00427d80 size=244

undefined4 FUN_00427d80(undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  char *pcVar5;
  char *pcVar6;
  char *pcVar7;
  char acStack_a0 [38];
  char acStack_7a [2];
  undefined4 auStack_78 [30];
  
  if (DAT_00668154 == 0) {
    iVar2 = FUN_0047a410(param_2,&DAT_005cca5c,acStack_a0);
    if (iVar2 != 0) {
      uVar3 = 0xffffffff;
      pcVar5 = &DAT_00665f34;
      do {
        pcVar7 = pcVar5;
        if (uVar3 == 0) break;
        uVar3 = uVar3 - 1;
        pcVar7 = pcVar5 + 1;
        cVar1 = *pcVar5;
        pcVar5 = pcVar7;
      } while (cVar1 != '\0');
      uVar3 = ~uVar3;
      pcVar5 = pcVar7 + -uVar3;
      pcVar7 = (char *)auStack_78;
      for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
        *(undefined4 *)pcVar7 = *(undefined4 *)pcVar5;
        pcVar5 = pcVar5 + 4;
        pcVar7 = pcVar7 + 4;
      }
      for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
        *pcVar7 = *pcVar5;
        pcVar5 = pcVar5 + 1;
        pcVar7 = pcVar7 + 1;
      }
      uVar3 = 0xffffffff;
      pcVar5 = (char *)auStack_78;
      do {
        if (uVar3 == 0) break;
        uVar3 = uVar3 - 1;
        cVar1 = *pcVar5;
        pcVar5 = pcVar5 + 1;
      } while (cVar1 != '\0');
      if (acStack_7a[~uVar3] != '\\') {
        iVar2 = -1;
        pcVar5 = (char *)auStack_78;
        do {
          pcVar7 = pcVar5;
          if (iVar2 == 0) break;
          iVar2 = iVar2 + -1;
          pcVar7 = pcVar5 + 1;
          cVar1 = *pcVar5;
          pcVar5 = pcVar7;
        } while (cVar1 != '\0');
        *(undefined2 *)(pcVar7 + -1) = DAT_005cca60;
      }
      uVar3 = 0xffffffff;
      pcVar5 = acStack_a0;
      do {
        pcVar7 = pcVar5;
        if (uVar3 == 0) break;
        uVar3 = uVar3 - 1;
        pcVar7 = pcVar5 + 1;
        cVar1 = *pcVar5;
        pcVar5 = pcVar7;
      } while (cVar1 != '\0');
      uVar3 = ~uVar3;
      iVar2 = -1;
      pcVar5 = (char *)auStack_78;
      do {
        pcVar6 = pcVar5;
        if (iVar2 == 0) break;
        iVar2 = iVar2 + -1;
        pcVar6 = pcVar5 + 1;
        cVar1 = *pcVar5;
        pcVar5 = pcVar6;
      } while (cVar1 != '\0');
      pcVar5 = pcVar7 + -uVar3;
      pcVar7 = pcVar6 + -1;
      for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
        *(undefined4 *)pcVar7 = *(undefined4 *)pcVar5;
        pcVar5 = pcVar5 + 4;
        pcVar7 = pcVar7 + 4;
      }
      for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
        *pcVar7 = *pcVar5;
        pcVar5 = pcVar5 + 1;
        pcVar7 = pcVar7 + 1;
      }
      FUN_0049a560();
      FUN_004bc470(auStack_78);
      return 0;
    }
  }
  else {
    FUN_0041ee50(s_This_command_is_only_available_i_005cca64);
  }
  return 4;
}


