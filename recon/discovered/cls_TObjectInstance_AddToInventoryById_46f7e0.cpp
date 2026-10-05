// FUN_0046f7e0 @ 0046f7e0 size=347

undefined4 __thiscall FUN_0046f7e0(int *param_1,uint param_2,int param_3,undefined4 param_4)

{
  undefined2 uVar1;
  int iVar2;
  int *piVar3;
  undefined4 uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  undefined4 *puVar9;
  int iStack_3c;
  undefined4 uStack_34;
  uint uVar8;
  
  iVar2 = (**(code **)(*param_1 + 0x170))();
  if (iVar2 != 0) {
    piVar3 = (int *)(**(code **)(*param_1 + 0x170))();
    uVar4 = (**(code **)(*piVar3 + 0x50))(param_2,param_3,param_4);
    return uVar4;
  }
  iVar2 = 0;
  iStack_3c = DAT_00659d18 - 1;
  uVar6 = DAT_00659d18;
  if (-1 < iStack_3c) {
    while (uVar5 = (int)uVar6 / 2, uVar5 != 0) {
      uVar8 = uVar5;
      if ((uVar6 & 1) == 0) {
        uVar8 = uVar5 - 1;
      }
      iVar7 = uVar8 + iVar2;
      uVar8 = *(uint *)(*(int *)(DAT_00659d28 + iVar7 * 4) + 0x1c);
      if (param_2 == uVar8) goto LAB_0046f8a5;
      if (param_2 < uVar8) {
        iStack_3c = iVar7 + -1;
        if ((uVar6 & 1) == 0) {
          uVar5 = uVar5 - 1;
        }
      }
      else {
        iVar2 = iVar7 + 1;
      }
      uVar6 = uVar5;
      if (iStack_3c < iVar2) {
        return 0;
      }
    }
    if ((uVar6 != 0) &&
       (iVar7 = iVar2, param_2 == *(uint *)(*(int *)(DAT_00659d28 + iVar2 * 4) + 0x1c))) {
LAB_0046f8a5:
      iVar2 = *(int *)(DAT_00659d28 + iVar7 * 4);
      if (iVar2 != 0) {
        if (*(byte *)(iVar2 + 0x22) < DAT_0065a258) {
          iVar7 = (&DAT_0065a148)[*(byte *)(iVar2 + 0x22)];
        }
        else {
          iVar7 = 0;
        }
        uVar1 = *(undefined2 *)(iVar2 + 0x20);
        puVar9 = &uStack_34;
        for (iVar2 = 0xd; iVar2 != 0; iVar2 = iVar2 + -1) {
          *puVar9 = 0;
          puVar9 = puVar9 + 1;
        }
        uStack_34._0_2_ = *(undefined2 *)(iVar7 + 8);
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
  return 0;
}


