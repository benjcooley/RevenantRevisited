// FUN_004bd490_Blit @ 004bd490 size=292

undefined4 __thiscall
FUN_004bd490_Blit(int *param_1,uint *param_2,int *param_3,undefined4 param_4,undefined4 param_5)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int *piVar4;
  
  if (param_3 != (int *)0x0) {
    iVar1 = (**(code **)(*param_1 + 0x14))();
    if (((iVar1 != 0) && (DAT_005db0b8 != 0)) &&
       ((param_1[1] != param_3[1] || (param_1[2] != param_3[2])))) {
      FUN_00481c10(s_Unable_to_blit_from_different_si_005defcc,0);
    }
    iVar1 = (**(code **)(*param_3 + 0x54))();
    if (iVar1 != 0) {
      uVar2 = (**(code **)(*param_3 + 0x60))(param_2,param_1,param_4,param_5);
      return uVar2;
    }
    if (((param_3[0xe] ^ param_1[0xe]) & 0x3001fU) != 0) {
      *param_2 = *param_2 | 0x8000000;
    }
  }
  if (((*param_2 & 0x400000) == 0) &&
     (iVar1 = (**(code **)(*param_1 + 0x50))(param_2,param_3,param_4,param_5), iVar1 == 0)) {
    return 0;
  }
  iVar1 = (**(code **)(*param_1 + 0x38))();
  if (iVar1 == 0) {
    return 1;
  }
  if (param_3 != (int *)0x0) {
    iVar1 = (**(code **)(*param_3 + 0x38))();
    if (iVar1 == 0) {
      return 1;
    }
    iVar1 = (**(code **)(*param_3 + 0x38))();
    iVar3 = (**(code **)(*param_1 + 0x38))();
    if (iVar3 == iVar1) {
      return 1;
    }
  }
  if ((*param_2 & 0x400) != 0) {
    uVar2 = 0;
    if (param_3 != (int *)0x0) {
      uVar2 = (**(code **)(*param_3 + 0x38))();
    }
    piVar4 = (int *)(**(code **)(*param_1 + 0x38))();
    iVar1 = (**(code **)(*piVar4 + 0x50))(param_2,uVar2,param_4,param_5);
    if (iVar1 == 0) {
      return 0;
    }
  }
  return 1;
}


