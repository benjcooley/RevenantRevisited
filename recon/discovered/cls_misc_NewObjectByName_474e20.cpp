// FUN_00474e20_NewObjectByName @ 00474e20 size=285

undefined4 FUN_00474e20_NewObjectByName(int param_1,undefined4 *param_2,undefined4 param_3,undefined4 param_4)

{
  undefined2 uVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  undefined4 *puVar9;
  int local_38;
  undefined4 local_34;
  
  if (param_1 != 0) {
    iVar8 = 0;
    local_38 = DAT_00659d30 - 1;
    uVar5 = DAT_00659d30;
    if (-1 < local_38) {
      do {
        uVar6 = (int)uVar5 / 2;
        if (uVar6 == 0) {
          if (uVar5 == 0) {
            return 0;
          }
          iVar3 = FUN_0059a530_stricmp(param_1,**(undefined4 **)(DAT_00659d40 + iVar8 * 4));
          iVar7 = iVar8;
          if (iVar3 != 0) {
            return 0;
          }
LAB_00474ec4:
          iVar8 = *(int *)(DAT_00659d40 + iVar7 * 4);
          if (iVar8 == 0) {
            return 0;
          }
          if (*(byte *)(iVar8 + 0x22) < DAT_0065a258_TObjectClass_numclasses) {
            iVar7 = (&DAT_0065a148_TObjectClass_classes)[*(byte *)(iVar8 + 0x22)];
          }
          else {
            iVar7 = 0;
          }
          uVar1 = *(undefined2 *)(iVar8 + 0x20);
          iVar8 = 0xd;
          if (param_2 == (undefined4 *)0x0) {
            puVar9 = &local_34;
            for (; iVar8 != 0; iVar8 = iVar8 + -1) {
              *puVar9 = 0;
              puVar9 = puVar9 + 1;
            }
          }
          else {
            puVar9 = &local_34;
            for (; iVar8 != 0; iVar8 = iVar8 + -1) {
              *puVar9 = *param_2;
              param_2 = param_2 + 1;
              puVar9 = puVar9 + 1;
            }
          }
          local_34._0_2_ = *(undefined2 *)(iVar7 + 8);
          local_34._2_2_ = uVar1;
          uVar4 = FUN_00474bb0_NewObject(&local_34,param_3,param_4);
          return uVar4;
        }
        uVar2 = uVar6;
        if ((uVar5 & 1) == 0) {
          uVar2 = uVar6 - 1;
        }
        iVar7 = uVar2 + iVar8;
        iVar3 = FUN_0059a530_stricmp(param_1,**(undefined4 **)(DAT_00659d40 + iVar7 * 4));
        if (iVar3 == 0) goto LAB_00474ec4;
        if (iVar3 < 0) {
          local_38 = iVar7 + -1;
          if ((uVar5 & 1) == 0) {
            uVar6 = uVar6 - 1;
          }
        }
        else {
          iVar8 = iVar7 + 1;
        }
        uVar5 = uVar6;
      } while (iVar8 <= local_38);
    }
  }
  return 0;
}


