// FUN_004530a0_GetWalkHeightRadius @ 004530a0 size=639

void FUN_004530a0_GetWalkHeightRadius(int *param_1,undefined4 param_2,int param_3,int *param_4,undefined4 *param_5,
                 undefined4 *param_6)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  uint uVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  uint uVar15;
  
  iVar9 = *param_1;
  iVar1 = param_1[1];
  *param_4 = 0;
  iVar2 = FUN_00499e10_TSector_FindLoadedSector(param_2,iVar9 + 8 >> 10,iVar1 + 8 >> 10);
  uVar3 = 0;
  if (iVar2 != 0) {
    uVar3 = FUN_00499720_TSector_ReturnWalkmap(iVar9 + 8 >> 4 & 0x3f,iVar1 + 8 >> 4 & 0x3f);
  }
  *param_5 = uVar3;
  if ((0 < param_3) && (param_3 < 0xf0)) {
    if (0x7f < (int)(param_3 + 0xfU & 0xfffffff0)) {
      param_3 = 0x6f;
    }
    iVar9 = *param_1;
    iVar1 = param_1[1];
    uVar6 = iVar9 - param_3 & 0xfffffff0;
    *param_6 = 0;
    uVar4 = iVar9 + 0x10 + param_3 & 0xfffffff0;
    *param_4 = 0;
    uVar11 = iVar1 - param_3 & 0xfffffff0;
    for (uVar15 = uVar11; (int)uVar15 <= (int)(iVar1 + 0x10 + param_3 & 0xfffffff0U);
        uVar15 = uVar15 + 0x10) {
      if ((int)uVar6 <= (int)uVar4) {
        iVar2 = (int)(uVar15 - uVar11) >> 4;
        iVar9 = uVar6 - 8;
        do {
          iVar7 = *param_1;
          iVar5 = param_1[1];
          iVar10 = (int)((iVar9 - uVar6) + 8) >> 4;
          iVar14 = iVar9 + 0xf;
          if ((iVar7 <= iVar9 + 0xf) && (iVar14 = iVar9, iVar9 <= iVar7)) {
            iVar14 = iVar7;
          }
          iVar13 = uVar15 + 7;
          if ((iVar5 <= iVar13) && (iVar13 = uVar15 - 8, (int)(uVar15 - 8) <= iVar5)) {
            iVar13 = iVar5;
          }
          iVar12 = iVar14 - iVar7;
          if (iVar12 < 0) {
            iVar12 = iVar7 - iVar14;
          }
          iVar7 = iVar13 - iVar5;
          if (iVar7 < 0) {
            iVar7 = iVar5 - iVar13;
          }
          if ((iVar12 < 0x100) && (iVar7 < 0x100)) {
            uVar8 = (uint)(byte)(&DAT_005e9200)[iVar7 + iVar12 * 0x100];
            iVar7 = param_3;
          }
          else {
            uVar8 = iVar7 * iVar7 + iVar12 * iVar12;
            iVar7 = param_3 * param_3;
          }
          if (iVar7 < (int)uVar8) {
            *(undefined4 *)(&DAT_00658328 + (iVar2 + iVar10 * 8) * 4) = 0xffffffff;
          }
          else {
            iVar5 = FUN_00499e10_TSector_FindLoadedSector(param_2,iVar9 + 0x10 >> 10,(int)(uVar15 + 8) >> 10);
            iVar7 = 0;
            if (iVar5 != 0) {
              iVar7 = FUN_00499720_TSector_ReturnWalkmap(iVar9 + 0x10 >> 4 & 0x3f,(int)(uVar15 + 8) >> 4 & 0x3f);
            }
            iVar5 = iVar2 + iVar10 * 8;
            *(int *)(&DAT_00658328 + iVar5 * 4) = iVar7;
            if (iVar7 == 0) {
              *param_6 = 1;
            }
            iVar14 = 0;
            if (((0 < iVar10) && (iVar10 = *(int *)(&DAT_00658308 + iVar5 * 4), -1 < iVar10)) &&
               (iVar14 = iVar7 - iVar10, iVar14 < 0)) {
              iVar14 = iVar10 - iVar7;
            }
            iVar10 = 0;
            if (((0 < iVar2) && (iVar5 = (&DAT_00658324)[iVar5], -1 < iVar5)) &&
               (iVar10 = iVar7 - iVar5, iVar10 < 0)) {
              iVar10 = iVar5 - iVar7;
            }
            if (*param_4 < iVar14) {
              *param_4 = iVar14;
            }
            if (*param_4 < iVar10) {
              *param_4 = iVar10;
            }
          }
          iVar7 = iVar9 + 0x18;
          iVar9 = iVar9 + 0x10;
        } while (iVar7 <= (int)uVar4);
      }
    }
  }
  return;
}


