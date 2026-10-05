// TExit_ReadExitList @ 0x0050c8f0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// static; module exit.def if present, else ClassDefPath exit.def
// FUN_0050c8f0 @ 0050c8f0 size=944

undefined4 FUN_0050c8f0(int param_1)

{
  char cVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  char *pcVar8;
  char *pcVar9;
  undefined4 local_2d4;
  int local_2d0;
  int local_2cc;
  undefined4 local_2c8;
  int local_2c4;
  undefined4 local_2c0;
  undefined4 local_2bc;
  undefined4 local_2b4;
  undefined4 local_2b0;
  undefined1 *local_2ac;
  undefined1 local_2a8;
  undefined4 local_2a4;
  undefined1 local_294 [128];
  char local_214 [260];
  char local_110 [260];
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a09f7;
  local_c = ExceptionList;
  if (param_1 == 0) {
    DAT_0066d1c4 = (undefined4 *)0x0;
  }
  if (DAT_0065a784 < 0) {
    iVar7 = 0;
  }
  else {
    iVar7 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
  }
  ExceptionList = &local_c;
  FUN_0058b100(local_214,&DAT_005e16fc,&DAT_0065bd48,s_exit_def_005e16f0);
  if (iVar7 != 0) {
    if (DAT_0065a784 < 0) {
      iVar7 = 0;
    }
    else {
      iVar7 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    FUN_0058b100(local_110,s__s_s__s_005e1710,&DAT_0065d6a4,iVar7 + 0x58,s_exit_def_005e1704);
    iVar7 = FUN_004a1c00(local_110,0);
    if (iVar7 != 0) {
      uVar5 = 0xffffffff;
      pcVar8 = local_110;
      do {
        pcVar9 = pcVar8;
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
        pcVar9 = pcVar8 + 1;
        cVar1 = *pcVar8;
        pcVar8 = pcVar9;
      } while (cVar1 != '\0');
      uVar5 = ~uVar5;
      pcVar8 = pcVar9 + -uVar5;
      pcVar9 = local_214;
      for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
        *(undefined4 *)pcVar9 = *(undefined4 *)pcVar8;
        pcVar8 = pcVar8 + 4;
        pcVar9 = pcVar9 + 4;
      }
      for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
        *pcVar9 = *pcVar8;
        pcVar8 = pcVar8 + 1;
        pcVar9 = pcVar9 + 1;
      }
    }
  }
  iVar7 = FUN_004a13f0(local_214,&DAT_005e1718,0);
  if (iVar7 != 0) {
    FUN_00478720();
    local_4 = 0;
    local_2d4 = 0;
    local_2d0 = 0;
    local_2cc = 0;
    local_2c8 = 0;
    local_2c4 = 0;
    local_2c0 = 0;
    local_2bc = 0;
    local_2b4 = 0;
    local_2b0 = 0;
    local_2a8 = 0;
    local_2a4 = 1;
    local_2ac = (undefined1 *)FUN_00482fb0(0x2000);
    *local_2ac = 0;
    local_4 = 1;
    iVar7 = FUN_004788d0(iVar7,local_214);
    if (iVar7 != 0) {
      FUN_00478a10();
      do {
        if ((local_2c4 == 9) || (local_2c4 == 1)) {
          FUN_004795a0();
        }
        if (local_2c4 == 10) break;
        puVar3 = (undefined4 *)FUN_00482fb0(0x24);
        iVar7 = FUN_0047a410(&local_2d4,s__t___d___d___d__level__d_mapinde_005e171c,local_294,
                             puVar3 + 1,puVar3 + 2,puVar3 + 3,puVar3 + 4,puVar3 + 5,puVar3 + 6,
                             (int)puVar3 + 0x1e,(int)puVar3 + 0x1d,puVar3 + 7);
        if (iVar7 == 0) {
          local_4 = 3;
          FUN_004830f0(local_2ac);
          if (local_2d0 != 0) {
            FUN_004830f0(local_2c8);
            FUN_004830f0(local_2d0);
            goto LAB_0050cbfe;
          }
          if (local_2cc == 0) goto LAB_0050cbfe;
          FUN_004830f0(local_2c8);
          goto LAB_0050cbf6;
        }
        puVar2 = DAT_0066d1c4;
        if (param_1 != 0) {
          for (; puVar2 != (undefined4 *)0x0; puVar2 = (undefined4 *)puVar2[8]) {
            iVar7 = FUN_0059a530(*puVar2,local_294);
            if (iVar7 == 0) {
              if (puVar2 != (undefined4 *)0x0) {
                FUN_004830f0(puVar3);
                goto LAB_0050cb85;
              }
              break;
            }
          }
        }
        if (puVar3 != (undefined4 *)0x0) {
          uVar4 = FUN_0059b6bc(local_294);
          *puVar3 = uVar4;
          puVar3[8] = DAT_0066d1c4;
          DAT_0066d1c4 = puVar3;
        }
LAB_0050cb85:
        FUN_004795c0();
      } while (local_2c4 != 10);
      DAT_0066d24c = 0;
      local_4 = 4;
      FUN_004830f0(local_2ac);
      if (local_2d0 == 0) {
        if (local_2cc != 0) {
          FUN_004830f0(local_2c8);
          FUN_004a1540(local_2cc);
        }
      }
      else {
        FUN_004830f0(local_2c8);
        FUN_004830f0(local_2d0);
      }
      local_4 = 0xffffffff;
      FUN_00478730();
      ExceptionList = local_c;
      return 1;
    }
    local_4 = 2;
    FUN_004830f0(local_2ac);
    if (local_2d0 == 0) {
      if (local_2cc != 0) {
        FUN_004830f0(local_2c8);
LAB_0050cbf6:
        FUN_004a1540(local_2cc);
      }
    }
    else {
      FUN_004830f0(local_2c8);
      FUN_004830f0(local_2d0);
    }
LAB_0050cbfe:
    local_4 = 0xffffffff;
    FUN_00478730();
  }
  ExceptionList = local_c;
  return 0;
}


