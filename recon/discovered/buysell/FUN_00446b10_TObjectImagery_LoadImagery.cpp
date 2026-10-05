// FUN_00446b10 @ 00446b10 size=141

int FUN_00446b10(uint param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  
  if ((((-1 < (int)param_1) && (DAT_006581e8 != 0)) && (param_1 < DAT_006581d8)) &&
     ((iVar2 = *(int *)(DAT_006581e8 + param_1 * 4), iVar2 != 0 &&
      ((int)param_1 < (int)DAT_006581d8)))) {
    if (iVar2 == 0) {
      iVar2 = DAT_006581ec;
    }
    if (*(int *)(iVar2 + 0x70) != 0) {
      *(int *)(iVar2 + 0x6c) = *(int *)(iVar2 + 0x6c) + 1;
      iVar2 = *(int *)(iVar2 + 0x70);
      *(uint *)(iVar2 + 8) = param_1;
      return iVar2;
    }
    if ((**(uint **)(iVar2 + 0x54) < DAT_006582c4) &&
       (*(undefined4 **)(&DAT_006581fc + **(uint **)(iVar2 + 0x54) * 4) != (undefined4 *)0x0)) {
      iVar1 = (**(code **)**(undefined4 **)(&DAT_006581fc + **(uint **)(iVar2 + 0x54) * 4))
                        (param_1,param_2);
      *(int *)(iVar2 + 0x70) = iVar1;
      if (iVar1 != 0) {
        *(uint *)(iVar1 + 8) = param_1;
        *(undefined4 *)(iVar2 + 0x6c) = 1;
        return iVar1;
      }
    }
  }
  return 0;
}


