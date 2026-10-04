// FUN_0041e690 @ 0041e690 size=592

int FUN_0041e690(int param_1,int param_2,int param_3)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar5 = 0;
  iVar2 = FUN_0059a530(param_1,&DAT_005caa30);
  if (iVar2 == 0) {
    return param_2;
  }
  iVar2 = FUN_0059a530(param_1,&DAT_005caa38);
  if (iVar2 == 0) {
    iVar5 = FUN_00492ac0();
    return iVar5;
  }
  iVar2 = FUN_0059a530(param_1,s_player_005caa40);
  if (iVar2 == 0) {
    return DAT_00667fcc;
  }
  iVar2 = FUN_0059a530(param_1,s_target_005caa48);
  if (iVar2 == 0) {
    if ((param_2 != 0) && ((*(short *)(param_2 + 4) == 0xc || (*(short *)(param_2 + 4) == 0xb)))) {
      piVar1 = *(int **)(param_2 + 0xe0);
      if ((piVar1 != (int *)0x0) &&
         ((*piVar1 == 3 || ((piVar1 != (int *)0x0 && (*piVar1 == 0x19)))))) {
        return piVar1[0x11];
      }
      return 0;
    }
  }
  else {
    iVar2 = FUN_0059a530(param_1,s_current_005caa50);
    if (iVar2 == 0) {
      if (param_3 != 0) {
        return *(int *)(param_3 + 0xd4);
      }
    }
    else {
      iVar2 = FUN_0059a600(param_1,s_party_005caa58,5);
      if ((iVar2 == 0) &&
         (((param_3 != 0 && (iVar2 = FUN_00492ac0(), iVar2 != 0)) ||
          ((param_2 != 0 && (*(short *)(param_2 + 4) == 0xb)))))) {
        iVar2 = FUN_0058b42c(param_1 + 5);
        if ((param_3 == 0) || (iVar3 = FUN_00492ac0(), iVar3 == 0)) {
          iVar3 = param_2;
        }
        param_2 = iVar3;
        iVar6 = 0;
        iVar3 = FUN_0051ee70(0);
        param_3 = iVar2;
        if (0 < iVar3) {
          do {
            iVar3 = FUN_0051eea0(iVar6,0);
            iVar2 = iVar5;
            if ((((iVar3 != 0) && (*(char *)(iVar3 + 0x494) != '\0')) &&
                (iVar4 = FUN_0059a530(iVar3 + 0x494,param_2 + 0x494), iVar4 == 0)) &&
               (iVar2 = iVar3, param_3 != 1)) {
              param_3 = param_3 + -1;
              iVar2 = iVar5;
            }
            iVar6 = iVar6 + 1;
            iVar3 = FUN_0051ee70(0);
            iVar5 = iVar2;
          } while (iVar6 < iVar3);
          if (iVar2 != 0) {
            return iVar2;
          }
        }
        return param_2;
      }
      if (param_3 != 0) {
        if ((*(int *)(param_3 + 0xcc) != 0) &&
           (iVar5 = FUN_0059a530(param_1,*(int *)(param_3 + 0xcc)), iVar5 == 0)) {
          return *(int *)(param_3 + 0xc4);
        }
        if ((*(int *)(param_3 + 0xd0) != 0) &&
           (iVar5 = FUN_0059a530(param_1,*(int *)(param_3 + 0xd0)), iVar5 == 0)) {
          return *(int *)(param_3 + 200);
        }
      }
      iVar5 = FUN_00451fe0(param_1,param_2,0,0);
    }
  }
  return iVar5;
}


