// FUN_0046f940 @ 0046f940 size=395

undefined4 __thiscall FUN_0046f940(int *param_1,int param_2,int param_3,undefined4 param_4)

{
  undefined2 uVar1;
  int iVar2;
  int *piVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  undefined4 *puVar10;
  int iStack_3c;
  undefined4 uStack_34;
  
  iVar2 = (**(code **)(*param_1 + 0x170))();
  if (iVar2 != 0) {
    piVar3 = (int *)(**(code **)(*param_1 + 0x170))();
    uVar4 = (**(code **)(*piVar3 + 0x54))(param_2,param_3,param_4);
    return uVar4;
  }
  if (param_2 != 0) {
    iVar2 = 0;
    iStack_3c = DAT_00659d30 - 1;
    uVar7 = DAT_00659d30;
    if (-1 < iStack_3c) {
      while (uVar8 = (int)uVar7 / 2, uVar8 != 0) {
        uVar5 = uVar8;
        if ((uVar7 & 1) == 0) {
          uVar5 = uVar8 - 1;
        }
        iVar9 = uVar5 + iVar2;
        iVar6 = FUN_0059a530(param_2,**(undefined4 **)(DAT_00659d40 + iVar9 * 4));
        if (iVar6 == 0) goto LAB_0046fa30;
        if (iVar6 < 0) {
          iStack_3c = iVar9 + -1;
          if ((uVar7 & 1) == 0) {
            uVar8 = uVar8 - 1;
          }
        }
        else {
          iVar2 = iVar9 + 1;
        }
        uVar7 = uVar8;
        if (iStack_3c < iVar2) {
          return 0;
        }
      }
      if ((uVar7 != 0) &&
         (iVar6 = FUN_0059a530(param_2,**(undefined4 **)(DAT_00659d40 + iVar2 * 4)), iVar9 = iVar2,
         iVar6 == 0)) {
LAB_0046fa30:
        iVar2 = *(int *)(DAT_00659d40 + iVar9 * 4);
        if (iVar2 != 0) {
          if (*(byte *)(iVar2 + 0x22) < DAT_0065a258) {
            iVar9 = (&DAT_0065a148)[*(byte *)(iVar2 + 0x22)];
          }
          else {
            iVar9 = 0;
          }
          uVar1 = *(undefined2 *)(iVar2 + 0x20);
          puVar10 = &uStack_34;
          for (iVar2 = 0xd; iVar2 != 0; iVar2 = iVar2 + -1) {
            *puVar10 = 0;
            puVar10 = puVar10 + 1;
          }
          uStack_34._0_2_ = *(undefined2 *)(iVar9 + 8);
          uStack_34._2_2_ = uVar1;
          piVar3 = (int *)FUN_00474bb0(&uStack_34,0xffffffff,1);
          if (piVar3 != (int *)0x0) {
            if (param_3 != 1) {
              (**(code **)(*piVar3 + 0x19c))(param_3);
            }
            uVar4 = (**(code **)(*param_1 + 0x58))(piVar3,param_4);
            return uVar4;
          }
        }
      }
    }
  }
  return 0;
}


