// FUN_0044ee00 @ 0044ee00 size=557

void __fastcall FUN_0044ee00(int param_1)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  char *pcVar6;
  char *pcVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  char local_20 [32];
  
  if (DAT_00667fcc != 0) {
    local_2c = *(int *)(DAT_00667fcc + 0x10);
    local_28 = *(int *)(DAT_00667fcc + 0x14);
    local_24 = *(int *)(DAT_00667fcc + 0x18);
    FUN_0046dad0((*(int *)(param_1 + 0x78) - *(int *)(param_1 + 4)) + DAT_00668510,
                 (*(int *)(param_1 + 0x7c) - *(int *)(param_1 + 8)) + DAT_00668514,&local_38,
                 *(int *)(DAT_00667fcc + 0x18) + 0x32);
    iVar2 = FUN_0046dc60(&local_2c,&local_38);
    local_30 = local_30 - local_24;
    local_34 = local_34 - local_28;
    local_38 = local_38 - local_2c;
    iVar4 = local_38;
    if (local_38 < 1) {
      iVar4 = -local_38;
    }
    if (iVar4 < 0x10) {
      iVar4 = local_34;
      if (local_34 < 1) {
        iVar4 = -local_34;
      }
      if (iVar4 < 0x10) {
        uVar3 = FUN_0046d710(s_cursor_005d02e8);
        FUN_0043a020(uVar3);
        *(undefined4 *)(param_1 + 0x11c) = 1;
        return;
      }
    }
    uVar5 = iVar2 + 0x10U & 0xe0;
    FUN_0058b100(local_20,s_wedge__s_005d02f0,(&PTR_DAT_005d0228)[(int)uVar5 >> 5]);
    uVar3 = FUN_0046d710(local_20);
    FUN_0043a020(uVar3);
    iVar4 = -1;
    pcVar6 = local_20;
    do {
      pcVar7 = pcVar6;
      if (iVar4 == 0) break;
      iVar4 = iVar4 + -1;
      pcVar7 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar7;
    } while (cVar1 != '\0');
    uVar9 = 0x2b;
    uVar8 = 0;
    *(undefined4 *)(pcVar7 + -1) = s_shadow_005d02fc._0_4_;
    *(undefined2 *)(pcVar7 + 3) = s_shadow_005d02fc._4_2_;
    pcVar7[5] = s_shadow_005d02fc[6];
    uVar3 = FUN_0046d710(local_20);
    FUN_0043a090(uVar3,uVar8,uVar9);
    FUN_0043a0b0(0,1);
    if (*(int *)(param_1 + 0x128) != -1) {
      (**(code **)(*DAT_00667fd0 + 0x30))(*(int *)(param_1 + 0x128),0);
    }
    *(undefined4 *)(param_1 + 0x128) = 0xffffffff;
    switch(uVar5) {
    case 0:
      *(undefined4 *)(param_1 + 0x128) = 0x21;
      break;
    case 0x20:
      *(undefined4 *)(param_1 + 0x128) = 0x27;
      break;
    case 0x40:
      *(undefined4 *)(param_1 + 0x128) = 0x22;
      break;
    case 0x60:
      *(undefined4 *)(param_1 + 0x128) = 0x28;
      break;
    case 0x80:
      *(undefined4 *)(param_1 + 0x128) = 0x23;
      break;
    case 0xa0:
      *(undefined4 *)(param_1 + 0x128) = 0x25;
      break;
    case 0xc0:
      *(undefined4 *)(param_1 + 0x128) = 0x24;
      break;
    case 0xe0:
      *(undefined4 *)(param_1 + 0x128) = 0x26;
    }
    if (*(int *)(param_1 + 0x128) != -1) {
      (**(code **)(*DAT_00667fd0 + 0x30))(*(int *)(param_1 + 0x128),1);
    }
    *(undefined4 *)(param_1 + 0x11c) = 1;
  }
  return;
}


