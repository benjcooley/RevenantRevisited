// FUN_00498780_TSector_LoadFromStream @ 00498780 size=663

undefined4 __thiscall FUN_00498780_TSector_LoadFromStream(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint *puVar8;
  int iVar9;
  int iVar10;
  bool bVar11;
  uint local_14;
  
  piVar4 = *(int **)(param_2 + 4);
  bVar11 = *piVar4 != DAT_005da750;
  *(int **)(param_2 + 4) = piVar4 + 1;
  if (bVar11) {
    return 0;
  }
  iVar1 = piVar4[1];
  *(int **)(param_2 + 4) = piVar4 + 2;
  if (0xd < iVar1) {
    *(int *)(param_1 + 0xb4) = piVar4[2];
    *(int **)(param_2 + 4) = piVar4 + 3;
  }
  iVar5 = DAT_00676828;
  iVar2 = **(int **)(param_2 + 4);
  *(int **)(param_2 + 4) = *(int **)(param_2 + 4) + 1;
  DAT_0065a254 = DAT_0065a254 | 1;
  if ((iVar5 == 0) || (bVar11 = true, ((byte)DAT_006767fc & 8) == 0)) {
    bVar11 = false;
  }
  local_14 = 0;
  if (0 < iVar2) {
    do {
      piVar4 = (int *)FUN_00471ce0_CreateObjectFromStream(param_2,iVar1,param_3);
      if (piVar4 != (int *)0x0) {
        if (((DAT_00676828 == 0) || (!bVar11)) ||
           ((((short)piVar4[1] != 0xc || (iVar5 = (**(code **)(*piVar4 + 0x1a8))(), iVar5 == 0)) &&
            (((short)piVar4[1] != 0xf ||
             (iVar5 = FUN_0059a530_stricmp(piVar4[0xe],s_monstergen_005da778), iVar5 != 0)))))) {
          iVar5 = *(int *)(param_1 + 8);
          iVar10 = piVar4[4];
          iVar9 = piVar4[5];
          *(short *)((int)piVar4 + 0xe) = (short)*(undefined4 *)(param_1 + 4);
          piVar4[0x11] = param_1;
          iVar6 = iVar5 * 0x400;
          if (iVar10 < iVar6) {
            iVar10 = iVar10 + ((iVar6 - iVar10) + 0x3ffU & 0xfffffc00);
          }
          iVar5 = iVar5 + 1;
          if (iVar5 * 0x400 <= iVar10) {
            iVar10 = iVar10 + (iVar10 + iVar5 * -0x400 + 0x400U >> 10) * -0x400;
          }
          for (; iVar9 < *(int *)(param_1 + 0xc) * 0x400; iVar9 = iVar9 + 0x400) {
          }
          for (; (*(int *)(param_1 + 0xc) + 1) * 0x400 <= iVar9; iVar9 = iVar9 + -0x400) {
          }
          if ((piVar4[4] != iVar10) || (piVar4[5] != iVar9)) {
            piVar4[4] = iVar10;
            piVar4[5] = iVar9;
            piVar4[6] = piVar4[6];
          }
        }
        else {
          FUN_0046e6d0(piVar4);
          piVar4 = (int *)0x0;
        }
      }
      FUN_0041c910(piVar4,local_14);
      if (piVar4 != (int *)0x0) {
        iVar5 = 1;
        do {
          switch(iVar5) {
          case 0:
            goto switchD_00498937_caseD_0;
          case 1:
            uVar7 = piVar4[2] & 8;
            break;
          case 2:
            if (((short)piVar4[1] == 0xc) || ((short)piVar4[1] == 0xb))
            goto switchD_00498937_caseD_0;
            goto switchD_00498937_default;
          case 3:
            uVar7 = piVar4[2] & 4;
            break;
          case 4:
            uVar7 = piVar4[2] & 0x8000;
            break;
          case 5:
            uVar7 = piVar4[2] & 0x4000;
            break;
          case 6:
            uVar7 = piVar4[2] & 0x40000;
            break;
          default:
            goto switchD_00498937_default;
          }
          if (uVar7 != 0) {
switchD_00498937_caseD_0:
            uVar3 = *(undefined4 *)(param_1 + 0xb4 + iVar5 * 0x18);
            puVar8 = (uint *)FUN_00482fb0(4);
            *puVar8 = local_14;
            FUN_0041c910(puVar8,uVar3);
          }
switchD_00498937_default:
          iVar5 = iVar5 + 1;
        } while (iVar5 < 7);
      }
      if ((iVar1 < 3) && (piVar4 != (int *)0x0)) {
        piVar4[0x10] = ((*(int *)(param_1 + 4) << 6 | *(uint *)(param_1 + 8)) << 6 |
                       *(uint *)(param_1 + 0xc)) << 0xc | local_14 & 0xfff;
      }
      local_14 = local_14 + 1;
    } while ((int)local_14 < iVar2);
  }
  *(undefined4 *)(param_1 + 0xb0) = param_3;
  return 1;
}


