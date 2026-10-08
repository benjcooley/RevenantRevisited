// FUN_0044e930 @ 0044e930 size=547

void __thiscall FUN_0044e930(int param_1,int param_2,int param_3,int param_4,uint *param_5)

{
  int iVar1;
  int iVar2;
  bool bVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint local_30;
  uint local_2c;
  uint local_28;
  uint local_24;
  uint local_20;
  uint local_1c;
  uint local_18;
  uint local_14;
  uint local_10;
  uint local_c;
  uint local_8;
  
  iVar5 = *(int *)(param_1 + 0x7c);
  iVar2 = *(int *)(param_1 + 0x10);
  FUN_0046dad0(*(int *)(param_1 + 0x78) + param_2,iVar5 + param_3,&local_24,0);
  FUN_0046dad0(*(int *)(param_1 + 0x78) + param_2,iVar2 + 0xdc + iVar5,&local_30,0);
  local_1c = local_1c & 0xfffffff0;
  local_20 = local_20 & 0xfffffff0;
  local_24 = local_24 & 0xfffffff0;
  local_28 = local_28 & 0xfffffff0;
  local_2c = local_2c & 0xfffffff0;
  local_30 = local_30 & 0xfffffff0;
  bVar3 = false;
  if ((int)local_20 <= (int)local_2c) {
    do {
      if (bVar3) goto LAB_0044eb3d;
      iVar5 = 0;
      do {
        local_14 = local_2c;
        local_18 = local_30;
        if (iVar5 == 1) {
          local_2c = local_2c - 0x10;
        }
        else if (iVar5 == 2) {
          local_30 = local_30 - 0x10;
        }
        iVar2 = local_2c + 8;
        iVar1 = local_30 + 8;
        local_10 = local_28;
        iVar4 = FUN_00499e10(DAT_00666970,iVar1 >> 10,iVar2 >> 10);
        if (iVar4 == 0) {
          uVar6 = 0;
        }
        else {
          uVar6 = FUN_00499720(iVar1 >> 4 & 0x3f,iVar2 >> 4 & 0x3f);
        }
        FUN_0046dad0(*(int *)(param_1 + 0x78) + param_2,
                     (int)(uVar6 * 0x362) / 1000 + param_3 + *(int *)(param_1 + 0x7c),&local_c,0);
        local_c = local_c + 8;
        local_8 = local_8 + 8;
        if ((((local_c ^ local_30) & 0xfffffff0) == 0) && (((local_8 ^ local_2c) & 0xfffffff0) == 0)
           ) {
          bVar3 = true;
          local_28 = uVar6;
          break;
        }
        local_30 = local_18;
        local_2c = local_14;
        local_28 = local_10;
        iVar5 = iVar5 + 1;
      } while (iVar5 < 3);
      local_2c = local_2c - 0x10;
      local_30 = local_30 - 0x10;
    } while ((int)local_20 <= (int)local_2c);
    if (bVar3) goto LAB_0044eb3d;
  }
  FUN_0046dad0(*(int *)(param_1 + 0x78) + param_2,param_3 + *(int *)(param_1 + 0x7c),&local_30,
               *(undefined4 *)(param_4 + 8));
LAB_0044eb3d:
  param_5[2] = local_28;
  *param_5 = local_30;
  param_5[1] = local_2c;
  return;
}


