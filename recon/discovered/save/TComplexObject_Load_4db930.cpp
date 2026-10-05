// FUN_004db930 @ 004db930 size=592

void __thiscall FUN_004db930(int *param_1,int param_2,int param_3,int param_4)

{
  char cVar1;
  byte bVar2;
  undefined2 uVar3;
  uint *puVar4;
  char *pcVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  uint *puVar10;
  char *pcVar11;
  uint *puVar12;
  undefined1 uVar13;
  char local_2c [16];
  void *pvStack_1c;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ec62;
  pvStack_c = ExceptionList;
  if (param_4 < 1) {
    uVar13 = 0;
    ExceptionList = &pvStack_c;
  }
  else {
    uVar13 = **(undefined1 **)(param_2 + 4);
    ExceptionList = &pvStack_c;
    *(undefined1 **)(param_2 + 4) = *(undefined1 **)(param_2 + 4) + 1;
  }
  FUN_00472430(param_2,param_3,uVar13);
  if (param_3 < 7) {
    puVar4 = (uint *)FUN_00482fb0(100);
    local_4 = 0;
    if (puVar4 == (uint *)0x0) {
      local_4 = 0xffffffff;
      puVar4 = (uint *)0x0;
    }
    else {
      pcVar5 = (char *)(**(code **)(*param_1 + 0x220))();
      uVar8 = 0xffffffff;
      puVar4[9] = 0xffffffff;
      do {
        pcVar11 = pcVar5;
        if (uVar8 == 0) break;
        uVar8 = uVar8 - 1;
        pcVar11 = pcVar5 + 1;
        cVar1 = *pcVar5;
        pcVar5 = pcVar11;
      } while (cVar1 != '\0');
      uVar8 = ~uVar8;
      *(char *)(puVar4 + 1) = '\0';
      *puVar4 = 1;
      puVar4[10] = 0;
      puVar4[0xc] = 0;
      puVar4[0xb] = 0;
      puVar4[0xd] = 0x10;
      puVar4[0x10] = 0;
      puVar4[0xf] = 0;
      puVar4[0xe] = 0;
      puVar4[0x11] = 0;
      puVar4[0x17] = 0;
      puVar4[0x14] = 0;
      puVar4[0x18] = 1;
      puVar4[0x12] = 0;
      puVar4[0x13] = 0;
      puVar10 = (uint *)(pcVar11 + -uVar8);
      puVar12 = puVar4 + 1;
      for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
        *puVar12 = *puVar10;
        puVar10 = puVar10 + 1;
        puVar12 = puVar12 + 1;
      }
      local_4 = 0xffffffff;
      for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
        *(char *)puVar12 = (char)*puVar10;
        puVar10 = (uint *)((int)puVar10 + 1);
        puVar12 = (uint *)((int)puVar12 + 1);
      }
      *puVar4 = 1;
    }
  }
  else {
    bVar2 = **(byte **)(param_2 + 4);
    *(byte **)(param_2 + 4) = *(byte **)(param_2 + 4) + 1;
    FUN_0049ce00(local_2c);
    puVar4 = (uint *)FUN_00482fb0(100);
    if (puVar4 == (uint *)0x0) {
      puVar4 = (uint *)0x0;
    }
    else {
      uVar8 = 0xffffffff;
      *(char *)(puVar4 + 1) = '\0';
      *puVar4 = 1;
      puVar4[9] = 0xffffffff;
      puVar4[10] = 0;
      puVar4[0xc] = 0;
      puVar4[0xb] = 0;
      puVar4[0xd] = 0x10;
      puVar4[0x10] = 0;
      puVar4[0xf] = 0;
      puVar4[0xe] = 0;
      puVar4[0x11] = 0;
      puVar4[0x17] = 0;
      puVar4[0x14] = 0;
      puVar4[0x18] = 1;
      puVar4[0x12] = 0;
      puVar4[0x13] = 0;
      pcVar5 = local_2c;
      do {
        pcVar11 = pcVar5;
        if (uVar8 == 0) break;
        uVar8 = uVar8 - 1;
        pcVar11 = pcVar5 + 1;
        cVar1 = *pcVar5;
        pcVar5 = pcVar11;
      } while (cVar1 != '\0');
      uVar8 = ~uVar8;
      puVar10 = (uint *)(pcVar11 + -uVar8);
      puVar12 = puVar4 + 1;
      for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
        *puVar12 = *puVar10;
        puVar10 = puVar10 + 1;
        puVar12 = puVar12 + 1;
      }
      for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
        *(char *)puVar12 = (char)*puVar10;
        puVar10 = (uint *)((int)puVar10 + 1);
        puVar12 = (uint *)((int)puVar12 + 1);
      }
      *puVar4 = (uint)bVar2;
    }
  }
  (**(code **)(*param_1 + 0x1f8))(puVar4);
  (**(code **)(*param_1 + 0x200))(puVar4);
  (**(code **)(*param_1 + 0x208))(puVar4,0);
  if (((int *)param_1[0x15] == (int *)0x0) ||
     (iVar6 = (**(code **)(*(int *)param_1[0x15] + 0x88))((short)param_1[3]), iVar6 == 0)) {
    uVar3 = (**(code **)(*param_1 + 0x138))(puVar4 + 1,0xffffffff);
  }
  else {
    iVar7 = FUN_0058ade0(iVar6,0x3a);
    if (iVar7 != 0) {
      iVar6 = iVar7 + 1;
    }
    iVar6 = FUN_0059a530(puVar4 + 1,iVar6);
    if (iVar6 == 0) {
      ExceptionList = pvStack_1c;
      return;
    }
    uVar3 = (**(code **)(*param_1 + 0x138))(puVar4 + 1,0xffffffff);
  }
  *(undefined2 *)(param_1 + 3) = uVar3;
  ExceptionList = pvStack_1c;
  return;
}


