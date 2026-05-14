// FUN_00496110_TGameState_LoadStream @ 00496110 size=304

undefined4 __thiscall FUN_00496110_TGameState_LoadStream(uint *param_1,int param_2)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  byte *pbVar6;
  uint *puVar7;
  byte *pbVar8;
  int local_38;
  byte local_34 [52];
  
  local_38 = **(int **)(param_2 + 4);
  *(int **)(param_2 + 4) = *(int **)(param_2 + 4) + 1;
  if (local_38 < 1) {
    return 1;
  }
  do {
    FUN_0049ce00(local_34);
    iVar5 = 0;
    uVar2 = **(uint **)(param_2 + 4);
    *(uint **)(param_2 + 4) = *(uint **)(param_2 + 4) + 1;
    uVar3 = 0xffffffff;
    pbVar6 = local_34;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      bVar1 = *pbVar6;
      pbVar6 = pbVar6 + 1;
    } while (bVar1 != 0);
    if (0 < (int)(~uVar3 - 1)) {
      do {
        local_34[iVar5] = local_34[iVar5] ^ 0x80;
        uVar3 = 0xffffffff;
        iVar5 = iVar5 + 1;
        pbVar6 = local_34;
        do {
          if (uVar3 == 0) break;
          uVar3 = uVar3 - 1;
          bVar1 = *pbVar6;
          pbVar6 = pbVar6 + 1;
        } while (bVar1 != 0);
      } while (iVar5 < (int)(~uVar3 - 1));
    }
    uVar3 = 0;
    if (0 < (int)*param_1) {
      puVar7 = param_1 + 0x1001;
      do {
        iVar5 = FUN_0059a530_stricmp(local_34,*puVar7);
        if (iVar5 == 0) {
          if (-1 < (int)uVar3) {
            if (uVar3 < *param_1) {
              param_1[uVar3 + 1] = uVar2;
            }
            goto LAB_00496203;
          }
          break;
        }
        uVar3 = uVar3 + 1;
        puVar7 = puVar7 + 1;
      } while ((int)uVar3 < (int)*param_1);
    }
    uVar3 = 0xffffffff;
    pbVar6 = local_34;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      bVar1 = *pbVar6;
      pbVar6 = pbVar6 + 1;
    } while (bVar1 != 0);
    uVar3 = FUN_00482fb0(~uVar3);
    uVar4 = 0xffffffff;
    param_1[*param_1 + 0x1001] = uVar3;
    pbVar6 = local_34;
    do {
      pbVar8 = pbVar6;
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      pbVar8 = pbVar6 + 1;
      bVar1 = *pbVar6;
      pbVar6 = pbVar8;
    } while (bVar1 != 0);
    uVar4 = ~uVar4;
    pbVar6 = pbVar8 + -uVar4;
    pbVar8 = (byte *)param_1[*param_1 + 0x1001];
    for (uVar3 = uVar4 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *(undefined4 *)pbVar8 = *(undefined4 *)pbVar6;
      pbVar6 = pbVar6 + 4;
      pbVar8 = pbVar8 + 4;
    }
    for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *pbVar8 = *pbVar6;
      pbVar6 = pbVar6 + 1;
      pbVar8 = pbVar8 + 1;
    }
    param_1[*param_1 + 1] = uVar2;
    *param_1 = *param_1 + 1;
LAB_00496203:
    local_38 = local_38 + -1;
    if (local_38 == 0) {
      return 1;
    }
  } while( true );
}


