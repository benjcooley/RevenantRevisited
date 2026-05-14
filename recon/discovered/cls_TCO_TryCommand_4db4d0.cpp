// FUN_004db4d0_TComplexObject_TryCommand @ 004db4d0 size=1049

undefined4 __thiscall FUN_004db4d0_TComplexObject_TryCommand(int *param_1,int param_2,undefined4 param_3,uint param_4)

{
  short *psVar1;
  int *piVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  undefined4 uVar7;
  uint uVar8;
  int unaff_EBX;
  int iVar9;
  int iVar10;
  
  iVar4 = param_1[0x36];
  if (((iVar4 != 0) && ((*(byte *)(iVar4 + 0x60) & 0x10) != 0)) && (param_1[0x37] != iVar4)) {
    return 2;
  }
  iVar10 = param_2;
  if (param_2 == 0) {
    iVar10 = param_1[0x38];
  }
  *(uint *)(iVar10 + 0x60) = *(uint *)(iVar10 + 0x60) & 0xffffffd9;
  if (iVar4 == 0) {
    iVar9 = iVar10 + 4;
    if ((param_4 & 1) == 0) {
      iVar4 = (**(code **)(*param_1 + 0x138))(iVar9,0xffffffff);
    }
    else {
      iVar4 = (**(code **)(*param_1 + 0x138))(iVar9,100);
    }
LAB_004db73d:
    if (iVar4 < 0) {
      return 3;
    }
  }
  else {
    if ((int *)param_1[0x15] == (int *)0x0) {
      uVar6 = 0;
    }
    else {
      uVar6 = (**(code **)(*(int *)param_1[0x15] + 0x8c))((short)param_1[3]);
    }
    if ((((uVar6 & 0x8000) == 0) || (param_1[0x37] == 0)) ||
       (iVar4 = param_1[0x38], param_1[0x37] != iVar4)) {
      iVar9 = iVar10 + 4;
      if ((param_4 & 1) == 0) {
        iVar4 = (**(code **)(*param_1 + 0x13c))(param_1[0x36] + 4,iVar9,0xffffffff);
      }
      else {
        iVar4 = (**(code **)(*param_1 + 0x13c))(param_1[0x36] + 4,iVar9,100);
      }
    }
    else {
      iVar9 = iVar4 + 4;
      iVar10 = iVar4;
      if ((param_4 & 1) == 0) {
        iVar4 = (**(code **)(*param_1 + 0x138))(iVar9,0xffffffff);
      }
      else {
        iVar4 = (**(code **)(*param_1 + 0x138))(iVar9,100);
      }
    }
    iVar5 = FUN_0059a530_stricmp(param_1[0x36] + 4,iVar9);
    uVar6 = *(uint *)(iVar10 + 0x60);
    uVar8 = (uint)(iVar5 != 0) << 1;
    *(uint *)(iVar10 + 0x60) = uVar8 | uVar6 & 0xfffffffd;
    if (iVar4 < 0) {
      if ((uVar6 & 0x80) != 0) goto LAB_004db73d;
      iVar4 = *param_1;
      *(uint *)(iVar10 + 0x60) = (uint)(iVar5 != 0) << 2 | uVar8 | uVar6 & 0xfffffff9;
      if (param_2 == 0) {
        iVar4 = (**(code **)(iVar4 + 0x138))(iVar9,0xffffffff);
      }
      else {
        iVar4 = (**(code **)(iVar4 + 0x138))(iVar9,100);
        if (-1 < iVar4) goto LAB_004db74d;
        iVar4 = (**(code **)(*param_1 + 0x138))(iVar9,0xffffffff);
      }
      if (((iVar4 < 0) &&
          ((param_2 == 0 ||
           (iVar4 = (**(code **)(*param_1 + 0x13c))(param_1[0x38] + 4,iVar9,100), iVar4 < 0)))) &&
         (iVar4 = (**(code **)(*param_1 + 0x13c))(param_1[0x38] + 4,iVar9,0xffffffff), iVar4 < 0)) {
        iVar4 = *param_1;
        if (param_2 != 0) {
          iVar4 = (**(code **)(iVar4 + 0x13c))(iVar9,param_1[0x38] + 4,100);
          if (-1 < iVar4) goto LAB_004db74d;
          iVar4 = *param_1;
        }
        iVar4 = (**(code **)(iVar4 + 0x13c))(iVar9,param_1[0x38] + 4,0xffffffff);
        if (iVar4 < 0) {
          iVar4 = param_1[0x38];
          if (param_2 != 0) {
            iVar4 = (**(code **)(*param_1 + 0x13c))(iVar4 + 4,iVar4 + 4,100);
            if (-1 < iVar4) goto LAB_004db74d;
            iVar4 = param_1[0x38];
          }
          iVar4 = (**(code **)(*param_1 + 0x13c))(iVar4 + 4,iVar4 + 4,0xffffffff);
          goto LAB_004db73d;
        }
      }
    }
  }
LAB_004db74d:
  (**(code **)(*param_1 + 0x18))(iVar4);
  (**(code **)(*param_1 + 0x200))(iVar10);
  uVar6 = *(uint *)(iVar10 + 0x24);
  if (-1 < (int)uVar6) {
    psVar1 = (short *)(*(int *)(*(int *)(param_1[0x15] + 4) + 0x54) + 0x32 + iVar4 * 0x4c);
    if ((int)(((int)uVar6 < 1) - 1 & uVar6) < *psVar1 + -1) {
      uVar3 = (ushort)uVar6 & ((int)uVar6 < 1) - 1;
    }
    else {
      uVar3 = *psVar1 - 1;
    }
    *(ushort *)(param_1 + 0x17) = uVar3;
    *(undefined4 *)(iVar10 + 0x24) = 0xffffffff;
  }
  if (*(int *)(iVar10 + 0x44) != 0) {
    if ((int *)param_1[0x15] == (int *)0x0) {
      uVar6 = 0;
    }
    else {
      uVar6 = (**(code **)(*(int *)param_1[0x15] + 0x8c))((short)param_1[3]);
    }
    if ((uVar6 & 0x10) != 0) {
      *(short *)(param_1 + 0x17) = *(short *)(*(int *)(iVar10 + 0x44) + 0x5c) + 1;
    }
  }
  piVar2 = (int *)param_1[0x15];
  if (unaff_EBX == 0) {
    if (piVar2 != (int *)0x0) {
      iVar4 = *piVar2;
      uVar7 = (**(code **)(*param_1 + 0x138))(iVar9,0xffffffff);
      uVar6 = (**(code **)(iVar4 + 0x8c))(uVar7);
      goto LAB_004db830;
    }
  }
  else if (piVar2 != (int *)0x0) {
    iVar4 = *piVar2;
    uVar7 = (**(code **)(*param_1 + 0x138))(iVar9,100);
    uVar6 = (**(code **)(iVar4 + 0x8c))(uVar7);
    goto LAB_004db830;
  }
  uVar6 = 0;
LAB_004db830:
  if ((int *)param_1[0x15] == (int *)0x0) {
    uVar8 = 0;
  }
  else {
    uVar8 = (**(code **)(*(int *)param_1[0x15] + 0x8c))((short)param_1[3]);
  }
  if ((((uVar8 & 0x400) != 0) || ((uVar6 & 0x400) != 0)) &&
     ((*(uint *)(iVar10 + 0x60) & 0x400) == 0)) {
    (**(code **)(*param_1 + 0x1f8))(iVar10);
  }
  if ((*(uint *)(iVar10 + 0x60) & 0x800) == 0) {
    if ((int *)param_1[0x15] == (int *)0x0) {
      uVar8 = 0;
    }
    else {
      uVar8 = (**(code **)(*(int *)param_1[0x15] + 0x8c))((short)param_1[3]);
    }
    if ((uVar8 & 1) == 0) {
      if ((int *)param_1[0x15] == (int *)0x0) {
        uVar8 = 0;
      }
      else {
        uVar8 = (**(code **)(*(int *)param_1[0x15] + 0x8c))((short)param_1[3]);
      }
      if ((((uVar8 & 0x400) == 0) && ((uVar6 & 1) == 0)) && ((uVar6 & 0x400) == 0)) {
        (**(code **)(*param_1 + 0x208))(0,0);
        return 2;
      }
    }
  }
  (**(code **)(*param_1 + 0x208))(iVar10,0);
  return 2;
}


