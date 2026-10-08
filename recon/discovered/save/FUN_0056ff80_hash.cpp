// FUN_0056ff80 @ 0056ff80 size=299

void __thiscall FUN_0056ff80(uint *param_1,char *param_2,uint param_3,char param_4)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  uint uVar19;
  
  if (param_4 != '\0') {
    *param_1 = 1;
  }
  uVar3 = *param_1 & 0xffff;
  uVar19 = *param_1 >> 0x10;
  while (0 < (int)param_3) {
    uVar1 = param_3;
    if (0x15af < (int)param_3) {
      uVar1 = 0x15b0;
    }
    param_3 = param_3 - uVar1;
    if (0xf < (int)uVar1) {
      uVar2 = uVar1 >> 4;
      uVar1 = uVar1 + uVar2 * -0x10;
      do {
        iVar4 = uVar3 + (int)*param_2;
        iVar5 = iVar4 + param_2[1];
        iVar6 = iVar5 + param_2[2];
        iVar7 = iVar6 + param_2[3];
        iVar8 = iVar7 + param_2[4];
        iVar9 = iVar8 + param_2[5];
        iVar10 = iVar9 + param_2[6];
        iVar11 = iVar10 + param_2[7];
        iVar12 = iVar11 + param_2[8];
        iVar13 = iVar12 + param_2[9];
        iVar14 = iVar13 + param_2[10];
        iVar15 = iVar14 + param_2[0xb];
        iVar16 = iVar15 + param_2[0xc];
        iVar17 = iVar16 + param_2[0xd];
        iVar18 = iVar17 + param_2[0xe];
        uVar3 = iVar18 + param_2[0xf];
        uVar19 = uVar19 + iVar4 + iVar5 + iVar6 + iVar7 + iVar8 + iVar9 + iVar10 + iVar11 + iVar12 +
                 iVar13 + iVar14 + iVar15 + iVar16 + iVar17 + iVar18 + uVar3;
        param_2 = param_2 + 0x10;
        uVar2 = uVar2 - 1;
      } while (uVar2 != 0);
    }
    for (; uVar1 != 0; uVar1 = uVar1 - 1) {
      uVar3 = uVar3 + (int)*param_2;
      param_2 = param_2 + 1;
      uVar19 = uVar19 + uVar3;
    }
    uVar3 = uVar3 % 0xfff1;
    uVar19 = uVar19 % 0xfff1;
  }
  *param_1 = uVar19 << 0x10 | uVar3;
  return;
}


