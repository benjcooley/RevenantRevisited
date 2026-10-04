// FUN_004be110 @ 004be110 size=267

undefined4 __thiscall
FUN_004be110(int param_1,char *param_2,int param_3,undefined4 param_4,int param_5,int param_6,
            int *param_7,uint param_8,int param_9,undefined4 param_10,uint param_11,int param_12,
            undefined4 param_13)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  
  if ((param_2 != (char *)0x0) && (*param_2 != '\0')) {
    iVar6 = *(int *)(DAT_0065b020 + param_6 * 4);
    if (iVar6 == 0) {
      iVar6 = DAT_0065b024;
    }
    iVar5 = param_9;
    if (param_9 < 1) {
      iVar5 = *(int *)(param_1 + 0x28);
    }
    iVar1 = *(int *)(iVar6 + 0x50);
    iVar2 = *(int *)(iVar6 + 0x54);
    if ((param_11 & 2) == 0) {
      if ((param_11 & 4) != 0) {
        param_3 = param_3 - iVar5;
      }
    }
    else {
      param_3 = param_3 - iVar5 / 2;
    }
    if (param_7 == (int *)0x0) {
      if (param_12 < 0) {
        param_9 = 0;
      }
      else {
        param_9 = param_12;
        param_8 = param_8 | 0x100000;
      }
    }
    else {
      param_9 = *param_7;
      param_8 = param_8 | 0x80000;
    }
    uVar3 = *(undefined4 *)(iVar6 + 0x54);
    *(undefined4 *)(iVar6 + 0x54) = param_13;
    uVar7 = param_11;
    uVar8 = param_11;
    FUN_00419dd0(&param_9);
    uVar4 = FUN_004be2b0(param_3,param_4,iVar5,(iVar1 + iVar2) * param_5,param_2,param_10,param_6,
                         uVar7,uVar8,param_8);
    *(undefined4 *)(iVar6 + 0x54) = uVar3;
    return uVar4;
  }
  return 0;
}


