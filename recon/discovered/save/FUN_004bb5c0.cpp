// FUN_004bb5c0 @ 004bb5c0 size=412

/* WARNING: Type propagation algorithm not settling */

void __thiscall FUN_004bb5c0(int param_1,int param_2,int param_3,uint param_4)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  int *piVar4;
  int *piVar5;
  int *piVar6;
  int *piVar7;
  int *piVar8;
  int local_20;
  int local_1c;
  int local_18;
  int local_14 [4];
  int local_4;
  
  if ((param_4 & 0x80) != 0) {
    if (DAT_00669324 == 0) {
      if (DAT_0066959c != 0) {
        iVar2 = 1;
        do {
          if (param_2 == iVar2) {
            iVar2 = 1;
            goto LAB_004bb624;
          }
          iVar2 = iVar2 << 1;
        } while (iVar2 != 0);
        goto LAB_004bb66c;
      }
    }
    else if ((DAT_0066959c != 0) || (param_2 != param_3)) {
      if ((DAT_0066959c != 0) && (param_2 == param_3)) {
        iVar2 = 1;
        do {
          if (param_2 == iVar2) {
            iVar2 = 1;
            goto LAB_004bb660;
          }
          iVar2 = iVar2 << 1;
        } while (iVar2 != 0);
      }
      goto LAB_004bb66c;
    }
  }
LAB_004bb727:
  local_14[1] = 0;
  local_14[2] = 0;
  local_14[3] = param_2 + -1;
  local_4 = param_3 + -1;
  FUN_004bad80(param_2,param_3,local_14 + 1,1,param_4 & 0xbfffffff);
  return;
  while (iVar2 = iVar2 << 1, iVar2 != 0) {
LAB_004bb660:
    if (param_3 == iVar2) goto LAB_004bb727;
  }
  goto LAB_004bb66c;
  while (iVar2 = iVar2 << 1, iVar2 != 0) {
LAB_004bb624:
    if (param_3 == iVar2) goto LAB_004bb727;
  }
LAB_004bb66c:
  if (param_3 < param_2) {
    puVar3 = &param_3;
    piVar4 = &local_20;
    piVar7 = &local_18;
    piVar8 = &param_2;
    piVar6 = &local_1c;
    piVar5 = local_14;
  }
  else {
    puVar3 = &param_2;
    piVar4 = &local_1c;
    piVar7 = local_14;
    piVar8 = &param_3;
    piVar6 = &local_20;
    piVar5 = &local_18;
  }
  FUN_004bb760(*puVar3,piVar7,piVar4,*(undefined4 *)(param_1 + 100));
  if (DAT_00669324 == 0) {
    FUN_004bb760(*piVar8,piVar5,piVar6,*(undefined4 *)(param_1 + 100));
  }
  else {
    iVar2 = *piVar7;
    iVar1 = *piVar8;
    *piVar5 = iVar2;
    *piVar6 = (iVar2 + -1 + iVar1) / iVar2;
  }
  FUN_004bb440(param_2,param_3,local_14[0],local_18,local_1c,local_20,param_4);
  return;
}


