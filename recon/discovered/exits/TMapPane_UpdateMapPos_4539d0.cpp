// TMapPane_UpdateMapPos @ 0x004539d0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// camera follow; centeron flag 8 = snap once
// FUN_004539d0 @ 004539d0 size=1985

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004539d0(int param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int local_34;
  int local_30;
  uint local_2c;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  int local_4;
  
  if (DAT_00668154 != 0) {
    if (((*(int *)(param_1 + 0x114) != 0) || (*(int *)(param_1 + 0x118) != 0)) &&
       (DAT_006671d0 == 0)) {
      FUN_0046dad0(*(int *)(param_1 + 0x114),*(undefined4 *)(param_1 + 0x118),&local_c,0);
      local_4 = local_4 + *(int *)(param_1 + 0xac);
      local_8 = local_8 + *(int *)(param_1 + 0xa8);
      local_c = local_c + *(int *)(param_1 + 0xa4);
      FUN_0046d7a0(&local_c,&local_24,&local_28);
      iVar2 = DAT_0065c5c4 / 2;
      *(int *)(param_1 + 0x34) = local_24 - DAT_00667c30 / 2;
      *(int *)(param_1 + 0x38) = local_28 - iVar2;
      if (((local_c != *(int *)(param_1 + 0xb0)) || (local_8 != *(int *)(param_1 + 0xb4))) ||
         (local_4 != *(int *)(param_1 + 0xb8))) {
        FUN_0049beb0(local_c,local_8,local_4);
      }
      FUN_0041c750(&local_c);
      _DAT_00656ec8 = 1;
    }
    piVar1 = (int *)(param_1 + 0xa4);
    FUN_0046d7a0(piVar1,&local_24,&local_28);
    iVar2 = DAT_0065c5c4 / 2;
    *(int *)(param_1 + 0x34) = local_24 - DAT_00667c30 / 2;
    *(int *)(param_1 + 0x38) = local_28 - iVar2;
    if (((*piVar1 != *(int *)(param_1 + 0xb0)) ||
        (*(int *)(param_1 + 0xa8) != *(int *)(param_1 + 0xb4))) ||
       (*(int *)(param_1 + 0xac) != *(int *)(param_1 + 0xb8))) {
      FUN_0049beb0(*piVar1,*(undefined4 *)(param_1 + 0xa8),*(undefined4 *)(param_1 + 0xac));
    }
    FUN_0041c750(piVar1);
    if (DAT_006671d0 == 0) {
      return;
    }
    FUN_004546a0();
    _DAT_00656ec8 = 1;
    return;
  }
  local_18 = *(int *)(param_1 + 0xb0);
  local_14 = *(int *)(param_1 + 0xb4);
  local_10 = *(int *)(param_1 + 0xb8);
  uVar5 = *(uint *)(param_1 + 0x98);
  local_2c = uVar5;
  if ((DAT_00658440 & 1) == 0) {
    DAT_00658440 = DAT_00658440 | 1;
    DAT_00658468 = 0;
    DAT_0065846c = 0;
    DAT_00658470 = 0;
    FUN_0058b66c(&DAT_004541b0);
  }
  if ((DAT_00658440 & 2) == 0) {
    DAT_00658440 = DAT_00658440 | 2;
    FUN_0058b66c(&DAT_004541a0);
  }
  if (((*(uint *)(param_1 + 0xd8) & 1) == 0) || (*(int *)(param_1 + 0x918) == 1)) {
    if (((*(uint *)(param_1 + 0xd8) & 2) == 0) || (*(int *)(param_1 + 0x918) == 1)) {
      uVar5 = *(uint *)(param_1 + 0x98);
      local_c = local_18;
      local_8 = local_14;
      local_4 = local_10;
      local_2c = uVar5;
    }
    else {
      local_c = *(int *)(param_1 + 0xe0);
      local_8 = *(int *)(param_1 + 0xe4);
      local_4 = *(int *)(param_1 + 0xe8);
      uVar5 = *(uint *)(param_1 + 0xec);
      local_2c = uVar5;
    }
  }
  else {
    iVar2 = *(int *)(param_1 + 0xdc);
    if (iVar2 != 0) {
      local_c = *(int *)(iVar2 + 0x10);
      local_8 = *(int *)(iVar2 + 0x14);
      local_4 = *(int *)(iVar2 + 0x18);
      uVar5 = (uint)*(ushort *)(iVar2 + 0xe);
      local_2c = uVar5;
    }
  }
  iVar2 = DAT_00658320 - local_4;
  if (iVar2 < 0) {
    iVar2 = local_4 - DAT_00658320;
  }
  if (iVar2 < 9) {
    local_4 = DAT_00658320;
  }
  if (((DAT_005d7a04 == 0) && (DAT_0065d0d0 != 0)) || (uVar5 != *(uint *)(param_1 + 0x98))) {
LAB_00453e5a:
    DAT_00658470 = 0;
    DAT_0065846c = 0;
    DAT_00658468 = 0;
    local_14 = local_8;
    local_18 = local_c;
    local_10 = local_4;
  }
  else {
    iVar2 = *(int *)(param_1 + 0xa4);
    iVar6 = iVar2 - local_c;
    if (iVar6 < 0) {
      iVar6 = local_c - iVar2;
    }
    iVar3 = *(int *)(param_1 + 0xa8) - local_8;
    if (iVar3 < 0) {
      iVar3 = local_8 - *(int *)(param_1 + 0xa8);
    }
    iVar4 = iVar6;
    if (iVar3 <= iVar6) {
      iVar4 = iVar3;
    }
    if (((0x3ff < (iVar3 - (iVar4 >> 1)) + iVar6) || ((*(uint *)(param_1 + 0xd8) & 4) == 0)) ||
       ((*(uint *)(param_1 + 0xd8) & 8) != 0)) goto LAB_00453e5a;
    iVar6 = 10;
    if (DAT_005d7a08 != 0) {
      iVar6 = 6;
    }
    iVar2 = (local_c - iVar2) / iVar6;
    iVar3 = (local_8 - *(int *)(param_1 + 0xa8)) / iVar6;
    iVar6 = (local_4 - *(int *)(param_1 + 0xac)) / iVar6;
    if (iVar2 + iVar3 < 0x100) {
      if (DAT_00658468 < iVar2) {
        DAT_00658468 = DAT_00658468 + 1;
      }
      else if (iVar2 < DAT_00658468) {
        DAT_00658468 = DAT_00658468 + -1;
      }
      if (DAT_0065846c < iVar3) {
        DAT_0065846c = DAT_0065846c + 1;
      }
      else if (iVar3 < DAT_0065846c) {
        DAT_0065846c = DAT_0065846c + -1;
      }
      if (DAT_00658470 < iVar6) {
        DAT_00658470 = DAT_00658470 + 1;
      }
      else if (iVar6 < DAT_00658470) {
        DAT_00658470 = DAT_00658470 + -1;
      }
      if (DAT_00658468 < 0xd) {
        if (DAT_00658468 < -0xc) {
          DAT_00658468 = -0xc;
        }
      }
      else {
        DAT_00658468 = 0xc;
      }
      if (DAT_0065846c < 0xd) {
        if (DAT_0065846c < -0xc) {
          DAT_0065846c = -0xc;
        }
      }
      else {
        DAT_0065846c = 0xc;
      }
      if (DAT_00658470 < 0xd) {
        if (DAT_00658470 < -0xc) {
          DAT_00658470 = -0xc;
        }
      }
      else {
        DAT_00658470 = 0xc;
      }
      local_10 = local_10 + DAT_00658470;
      local_14 = local_14 + DAT_0065846c;
      local_18 = local_18 + DAT_00658468;
    }
    else {
      local_18 = local_c;
      DAT_00658470 = 0;
      DAT_0065846c = 0;
      DAT_00658468 = 0;
      local_14 = local_8;
      local_10 = local_4;
    }
  }
  DAT_00658320 = local_4;
  if ((DAT_005d7a04 == 0) && (DAT_0065d0d0 != 0)) {
    FUN_0046d7a0(&local_c,&local_34,&local_30);
    iVar2 = local_34;
    if (local_34 < 1) {
      iVar2 = -local_34;
    }
    iVar6 = (DAT_00667c30 + -0x40) / 2;
    if (iVar2 < iVar6) {
      local_34 = 0;
    }
    else {
      if (local_34 < 0) {
        iVar6 = -iVar6;
      }
      local_34 = (local_34 + iVar6) / (DAT_00667c30 + -0x40);
    }
    local_30 = local_30 + -0x20;
    iVar2 = local_30;
    if (local_30 < 1) {
      iVar2 = -local_30;
    }
    iVar6 = (DAT_0065c5c4 + -0x40) / 2;
    if (iVar2 < iVar6) {
      local_30 = 0;
    }
    else {
      if (local_30 < 0) {
        iVar6 = -iVar6;
      }
      local_30 = (local_30 + iVar6) / (DAT_0065c5c4 + -0x40);
    }
    FUN_0046d7a0(param_1 + 0xa4,&local_28,&local_24);
    iVar2 = DAT_00667c30 + -0x40;
    iVar6 = local_28;
    if (local_28 < 1) {
      iVar6 = -local_28;
    }
    iVar3 = iVar2 / 2;
    if (iVar6 < iVar3) {
      local_28 = 0;
    }
    else {
      if (local_28 < 0) {
        iVar3 = -iVar3;
      }
      local_28 = (local_28 + iVar3) / iVar2;
    }
    iVar6 = DAT_0065c5c4 + -0x40;
    iVar3 = local_24;
    if (local_24 < 1) {
      iVar3 = -local_24;
    }
    iVar4 = iVar6 / 2;
    if (iVar3 < iVar4) {
      local_24 = 0;
    }
    else {
      if (local_24 < 0) {
        iVar4 = -iVar4;
      }
      local_24 = (local_24 + iVar4) / iVar6;
    }
    local_30 = iVar6 * local_30;
    local_34 = iVar2 * local_34;
    FUN_0046dad0(local_34,local_30,&local_18,0);
    iVar2 = local_18 - *(int *)(param_1 + 0xa4);
    if (iVar2 < 1) {
      iVar2 = *(int *)(param_1 + 0xa4) - local_18;
    }
    if (iVar2 < 5) {
      iVar2 = local_14 - *(int *)(param_1 + 0xa8);
      if (iVar2 < 1) {
        iVar2 = *(int *)(param_1 + 0xa8) - local_14;
      }
      if (iVar2 < 5) goto LAB_00454138;
    }
    FUN_0046d7a0(&local_18,&local_20,&local_1c);
    iVar2 = DAT_0065c5c4 / 2;
    *(int *)(param_1 + 0x34) = local_20 - DAT_00667c30 / 2;
    *(int *)(param_1 + 0x38) = local_1c - iVar2;
    if (((local_18 != *(int *)(param_1 + 0xb0)) || (local_14 != *(int *)(param_1 + 0xb4))) ||
       (local_10 != *(int *)(param_1 + 0xb8))) {
      FUN_0049beb0(local_18,local_14,local_10);
    }
    *(int *)(param_1 + 0xac) = local_10;
    *(int *)(param_1 + 0xa4) = local_18;
    *(int *)(param_1 + 0xa8) = local_14;
    FUN_004546a0();
  }
  else {
    FUN_0046d7a0(&local_18,&local_1c,&local_20);
    iVar2 = DAT_0065c5c4 / 2;
    *(int *)(param_1 + 0x34) = local_1c - DAT_00667c30 / 2;
    *(int *)(param_1 + 0x38) = local_20 - iVar2;
    if ((local_18 != *(int *)(param_1 + 0xb0)) ||
       ((local_14 != *(int *)(param_1 + 0xb4) || (local_10 != *(int *)(param_1 + 0xb8))))) {
      FUN_0049beb0(local_18,local_14,local_10);
    }
    *(int *)(param_1 + 0xa4) = local_18;
    *(int *)(param_1 + 0xa8) = local_14;
    *(int *)(param_1 + 0xac) = local_10;
  }
LAB_00454138:
  *(uint *)(param_1 + 0x98) = local_2c;
  if (local_2c != *(uint *)(param_1 + 0x9c)) {
    FUN_004546a0();
  }
  _DAT_00658318 = local_c;
  _DAT_006584ac = *(undefined4 *)(param_1 + 0x9c);
  _DAT_0065831c = local_8;
  DAT_00658320 = local_4;
  *(uint *)(param_1 + 0xd8) = *(uint *)(param_1 + 0xd8) & 0xfffffff7;
  return;
}


