// FUN_004a1ec0 @ 004a1ec0 size=569

int * FUN_004a1ec0(int param_1,int param_2,uint param_3,int param_4)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  int local_10;
  int local_c;
  
  iVar3 = param_2;
  iVar2 = param_1;
  uVar4 = param_3 & 0x3001f;
  if (uVar4 < 9) {
    if (uVar4 == 8) {
      iVar6 = 3;
      goto LAB_004a1f25;
    }
    if (uVar4 == 1) {
      iVar6 = 1;
      goto LAB_004a1f25;
    }
    if ((uVar4 != 2) && (uVar4 != 4)) {
      return (int *)0x0;
    }
  }
  else {
    if (uVar4 == 0x10) {
      iVar6 = 4;
      goto LAB_004a1f25;
    }
    if ((uVar4 != 0x10000) && (uVar4 != 0x20000)) {
      return (int *)0x0;
    }
  }
  iVar6 = 2;
LAB_004a1f25:
  iVar7 = iVar6 * param_1 * param_2;
  iVar6 = iVar7 + 0x48 + param_4;
  local_10 = 0;
  param_1 = 0;
  param_2 = 0;
  local_c = 0;
  if ((param_3 & 0x20) != 0) {
    local_10 = iVar2 * 2 * iVar3;
    iVar6 = iVar6 + local_10;
  }
  if ((param_3 & 0x40) != 0) {
    param_1 = iVar2 * 2 * iVar3;
    iVar6 = iVar6 + param_1;
  }
  if ((param_3 & 0x100) != 0) {
    param_2 = iVar2 * 2 * iVar3;
    iVar6 = iVar6 + param_2;
  }
  if ((param_3 & 0x200) != 0) {
    iVar6 = iVar6 + 0x600;
    local_c = 0x600;
  }
  piVar5 = (int *)FUN_00482fb0(iVar6);
  if (piVar5 == (int *)0x0) {
    return (int *)0x0;
  }
  piVar5[1] = iVar3;
  uVar4 = 0;
  *piVar5 = iVar2;
  piVar5[4] = param_3;
  if ((param_3 & 0x20) != 0) {
    uVar4 = 0x400;
  }
  if ((param_3 & 0x40) != 0) {
    uVar4 = uVar4 | 0x800;
  }
  if ((param_3 & 0x80) != 0) {
    uVar4 = uVar4 | 0x1000;
  }
  if ((param_3 & 0x100) != 0) {
    uVar4 = uVar4 | 0x2000;
  }
  piVar5[5] = uVar4;
  piVar5[6] = 0;
  piVar5[7] = param_4;
  if (param_4 == 0) {
    piVar5[8] = 0;
  }
  else {
    iVar2 = iVar7 + 0x48 + (int)piVar5;
    piVar1 = piVar5 + 8;
    if (iVar2 == 0) {
      *piVar1 = 0;
    }
    else {
      *piVar1 = iVar2 - (int)piVar1;
    }
  }
  piVar5[9] = param_2;
  if (param_2 == 0) {
    piVar5[10] = 0;
  }
  else {
    piVar1 = piVar5 + 10;
    iVar2 = (int)piVar5 + param_4 + iVar7 + 0x48;
    if (iVar2 == 0) {
      *piVar1 = 0;
    }
    else {
      *piVar1 = iVar2 - (int)piVar1;
    }
  }
  piVar5[0xb] = local_10;
  if (local_10 == 0) {
    piVar5[0xc] = 0;
  }
  else {
    piVar1 = piVar5 + 0xc;
    iVar2 = (int)piVar5 + param_4 + param_2 + iVar7 + 0x48;
    if (iVar2 == 0) {
      *piVar1 = 0;
    }
    else {
      *piVar1 = iVar2 - (int)piVar1;
    }
  }
  piVar5[0xd] = param_1;
  if (param_1 == 0) {
    piVar5[0xe] = 0;
  }
  else {
    piVar1 = piVar5 + 0xe;
    iVar2 = (int)piVar5 + param_4 + local_10 + param_2 + iVar7 + 0x48;
    if (iVar2 == 0) {
      *piVar1 = 0;
    }
    else {
      *piVar1 = iVar2 - (int)piVar1;
    }
  }
  piVar5[0xf] = local_c;
  if (local_c != 0) {
    iVar2 = (int)piVar5 + param_4 + local_10 + param_1 + param_2 + iVar7 + 0x48;
    piVar1 = piVar5 + 0x10;
    if (iVar2 != 0) {
      piVar5[0x11] = iVar7;
      *piVar1 = iVar2 - (int)piVar1;
      return piVar5;
    }
    *piVar1 = 0;
    piVar5[0x11] = iVar7;
    return piVar5;
  }
  piVar5[0x11] = iVar7;
  piVar5[0x10] = 0;
  return piVar5;
}


