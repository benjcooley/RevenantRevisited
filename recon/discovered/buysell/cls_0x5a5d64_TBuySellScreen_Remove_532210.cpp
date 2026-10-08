// FUN_00532210 @ 00532210 size=289

void __thiscall FUN_00532210(int param_1,byte *param_2)

{
  byte bVar1;
  short sVar2;
  byte *pbVar3;
  int iVar4;
  int iVar5;
  byte *pbVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  bool bVar10;
  uint local_8;
  int local_4;
  
  local_8 = 0;
  if (0 < *(short *)(param_1 + 0x194)) {
    local_4 = 0;
    do {
      pbVar3 = *(byte **)(local_4 + *(int *)(param_1 + 0x198) + 4);
      pbVar6 = param_2;
      do {
        bVar1 = *pbVar3;
        bVar10 = bVar1 < *pbVar6;
        if (bVar1 != *pbVar6) {
LAB_00532267:
          iVar4 = (1 - (uint)bVar10) - (uint)(bVar10 != 0);
          goto LAB_0053226c;
        }
        if (bVar1 == 0) break;
        bVar1 = pbVar3[1];
        bVar10 = bVar1 < pbVar6[1];
        if (bVar1 != pbVar6[1]) goto LAB_00532267;
        pbVar3 = pbVar3 + 2;
        pbVar6 = pbVar6 + 2;
      } while (bVar1 != 0);
      iVar4 = 0;
LAB_0053226c:
      if (iVar4 == 0) {
        FUN_0052f310();
        sVar2 = *(short *)(param_1 + 0x194);
        if (local_8 < (uint)(int)sVar2) {
          iVar4 = *(short *)(param_1 + 0x196) + -1;
          if ((int)local_8 < iVar4) {
            iVar4 = iVar4 - local_8;
            puVar8 = (undefined4 *)(*(int *)(param_1 + 0x198) + local_4);
            do {
              iVar4 = iVar4 + -1;
              puVar7 = puVar8 + 0x12;
              puVar9 = puVar8;
              for (iVar5 = 0x12; iVar5 != 0; iVar5 = iVar5 + -1) {
                *puVar9 = *puVar7;
                puVar7 = puVar7 + 1;
                puVar9 = puVar9 + 1;
              }
              puVar8 = puVar8 + 0x12;
            } while (iVar4 != 0);
          }
          sVar2 = sVar2 + -1;
          *(short *)(param_1 + 0x194) = sVar2;
          if (sVar2 < 1) {
            if (*(int *)(param_1 + 0x198) != 0) {
              FUN_004830f0(*(int *)(param_1 + 0x198));
            }
            *(undefined4 *)(param_1 + 0x198) = 0;
            *(undefined2 *)(param_1 + 0x194) = 0;
            *(undefined2 *)(param_1 + 0x196) = 0;
          }
        }
        local_8 = local_8 - 1;
        local_4 = local_4 + -0x48;
      }
      local_8 = local_8 + 1;
      local_4 = local_4 + 0x48;
    } while ((int)local_8 < (int)*(short *)(param_1 + 0x194));
  }
  return;
}


