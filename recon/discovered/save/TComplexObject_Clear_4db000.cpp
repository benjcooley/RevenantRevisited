// FUN_004db000 @ 004db000 size=390

void __fastcall FUN_004db000(int *param_1)

{
  char cVar1;
  int iVar2;
  undefined2 uVar3;
  undefined4 *puVar4;
  char *pcVar5;
  uint uVar6;
  uint uVar7;
  void *unaff_EBX;
  char *pcVar8;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ec4d;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  param_1[2] = param_1[2] | 0x28000;
  FUN_00471b60(3);
  iVar2 = param_1[0x38];
  if ((((iVar2 != 0) && (iVar2 != param_1[0x37])) && (iVar2 != param_1[0x36])) && (iVar2 != 0)) {
    if (*(int *)(iVar2 + 0x5c) != 0) {
      FUN_00482f80(*(int *)(iVar2 + 0x5c));
    }
    FUN_004830f0(iVar2);
  }
  iVar2 = param_1[0x36];
  if (((iVar2 != 0) && (iVar2 != param_1[0x37])) && (iVar2 != 0)) {
    if (*(int *)(iVar2 + 0x5c) != 0) {
      FUN_00482f80(*(int *)(iVar2 + 0x5c));
    }
    FUN_004830f0(iVar2);
  }
  iVar2 = param_1[0x37];
  if (iVar2 != 0) {
    if (*(int *)(iVar2 + 0x5c) != 0) {
      FUN_00482f80(*(int *)(iVar2 + 0x5c));
    }
    FUN_004830f0(iVar2);
  }
  puVar4 = (undefined4 *)FUN_00482fb0(100);
  local_4 = 0;
  if (puVar4 == (undefined4 *)0x0) {
    puVar4 = (undefined4 *)0x0;
  }
  else {
    pcVar5 = (char *)(**(code **)(*param_1 + 0x220))();
    uVar6 = 0xffffffff;
    puVar4[9] = 0xffffffff;
    do {
      pcVar8 = pcVar5;
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      pcVar8 = pcVar5 + 1;
      cVar1 = *pcVar5;
      pcVar5 = pcVar8;
    } while (cVar1 != '\0');
    uVar6 = ~uVar6;
    puVar4[10] = 0;
    puVar4[0xc] = 0;
    puVar4[0xb] = 0;
    puVar4[0x10] = 0;
    puVar4[0xf] = 0;
    puVar4[0xe] = 0;
    puVar4[0x11] = 0;
    puVar4[0x17] = 0;
    puVar4[0x14] = 0;
    puVar4[0x12] = 0;
    puVar4[0x13] = 0;
    *(char *)(puVar4 + 1) = '\0';
    *puVar4 = 1;
    puVar4[0xd] = 0x10;
    puVar4[0x18] = 1;
    pcVar5 = pcVar8 + -uVar6;
    pcVar8 = (char *)(puVar4 + 1);
    for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined4 *)pcVar8 = *(undefined4 *)pcVar5;
      pcVar5 = pcVar5 + 4;
      pcVar8 = pcVar8 + 4;
    }
    for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
      *pcVar8 = *pcVar5;
      pcVar5 = pcVar5 + 1;
      pcVar8 = pcVar8 + 1;
    }
    *puVar4 = 1;
  }
  param_1[0x37] = (int)puVar4;
  param_1[0x36] = (int)puVar4;
  param_1[0x38] = (int)puVar4;
  local_4 = 0xffffffff;
  uVar3 = (**(code **)(*param_1 + 0x138))(puVar4 + 1,0xffffffff);
  *(undefined2 *)(param_1 + 3) = uVar3;
  ExceptionList = unaff_EBX;
  return;
}


