// FUN_004504d0 @ 004504d0 size=1785

/* WARNING: Removing unreachable block (ram,0x00450589) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_004504d0(int param_1,int param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int *piVar5;
  uint uVar6;
  int iVar7;
  unkbyte10 extraout_ST0;
  uint uStack_78;
  int iStack_74;
  int iStack_70;
  int iStack_6c;
  uint uStack_68;
  int iStack_64;
  int iStack_60;
  int iStack_5c;
  int aiStack_58 [3];
  int iStack_4c;
  int *piStack_3c;
  
  iVar2 = DAT_00668510;
  if (DAT_00668154 == 0) {
    if ((param_2 == 2) && (*(int *)(param_1 + 0x11c) != 0)) {
      FUN_0044ee00(param_3,param_4);
    }
    if (DAT_00667fcc == 0) {
      return;
    }
    iVar2 = FUN_004d1050();
    if (iVar2 == 0) {
      return;
    }
    iStack_64 = *(int *)(DAT_00667fcc + 0x10);
    iStack_60 = *(undefined4 *)(DAT_00667fcc + 0x14);
    iStack_5c = *(undefined4 *)(DAT_00667fcc + 0x18);
    FUN_0046dad0((*(int *)(param_1 + 0x78) - *(int *)(param_1 + 4)) + DAT_00668510,
                 (*(int *)(param_1 + 0x7c) - *(int *)(param_1 + 8)) + DAT_00668514,aiStack_58,
                 *(int *)(DAT_00667fcc + 0x18) + 0x32);
    uVar1 = FUN_0046dc60(&iStack_64,aiStack_58);
    FUN_004d0c70(uVar1);
    return;
  }
  *(undefined4 *)(param_1 + 0x118) = 0;
  *(undefined4 *)(param_1 + 0x114) = 0;
  iVar4 = DAT_00668514;
  if (iVar2 < 1) {
    if (DAT_00668514 < 1) {
      uStack_78 = 0xa0;
    }
    else {
      uStack_78 = ((DAT_00668514 < *(int *)(PTR_DAT_005d79e0 + 8) + -1) - 1 & 0xffffffe0) + 0x80;
    }
LAB_0045057a:
    if (DAT_006671f0 != 0) {
      iVar2 = __ftol();
      uStack_78 = (0x60U - iVar2 & 0xff) + uStack_78 & 0xff;
    }
    fcos((float10)(int)uStack_78 * (float10)_DAT_005a4978);
    uVar1 = __ftol();
    fsin(extraout_ST0);
    *(undefined4 *)(param_1 + 0x114) = uVar1;
    uVar1 = __ftol();
    *(undefined4 *)(param_1 + 0x118) = uVar1;
  }
  else {
    if (*(int *)(PTR_DAT_005d79e0 + 4) + -1 <= iVar2) {
      if (DAT_00668514 < 1) {
        uStack_78 = 0xe0;
      }
      else {
        uStack_78 = (DAT_00668514 < *(int *)(PTR_DAT_005d79e0 + 8) + -1) - 1 & 0x20;
      }
      goto LAB_0045057a;
    }
    if (DAT_00668514 < 1) {
      uStack_78 = 0xc0;
      goto LAB_0045057a;
    }
    if (*(int *)(PTR_DAT_005d79e0 + 8) + -1 <= DAT_00668514) {
      uStack_78 = 0x40;
      goto LAB_0045057a;
    }
  }
  if (param_3 < 0) {
    return;
  }
  if ((((param_4 < 0) || (*(int *)(param_1 + 0xc) <= param_3)) ||
      (*(int *)(param_1 + 0x10) <= param_4)) ||
     (((param_2 != 1 || (iVar2 = 0, *(int *)(param_1 + 0x894) != 0)) &&
      ((iVar2 = *(int *)(param_1 + 0x894), iVar2 == 0 || (param_2 == 1)))))) {
    if (param_4 < 0) {
      return;
    }
    if (*(int *)(param_1 + 0xc) <= param_3) {
      return;
    }
    if (*(int *)(param_1 + 0x10) <= param_4) {
      return;
    }
    if (param_2 != 1) {
      return;
    }
    FUN_0046dad0((*(int *)(param_1 + 0x78) - *(int *)(param_1 + 4)) + DAT_00668510,
                 (*(int *)(param_1 + 0x7c) - *(int *)(param_1 + 8)) + iVar4,&iStack_64,
                 *(int *)(DAT_00667fcc + 0x18) + 0x32);
    FUN_00440d70(iStack_64,iStack_60);
    return;
  }
  if (0 < (int)DAT_00656f00) {
    if (DAT_00656f00 < 0xb) {
      iVar4 = 0;
    }
    else {
      iVar4 = DAT_00656f10[10];
    }
    if (((*(uint *)(iVar4 + 0x14) >> 0x10 & 1) != 0) && (iVar2 == 0)) {
      if (*(int *)(param_1 + 0x108) < 0) {
        return;
      }
      iVar2 = *(int *)(param_1 + 0x10c);
      if (iVar2 < 0) {
        return;
      }
      uVar6 = (((*(int *)(param_1 + 0x110) - *(int *)(param_1 + 0x7c)) - param_4) * 1000) / 0x362 +
              *(int *)(param_1 + 0xfc);
      if (((int)uVar6 < 0) || ((int)uVar6 < 0x100)) {
        uVar6 = ((int)uVar6 < 0) - 1 & uVar6;
      }
      else {
        uVar6 = 0xff;
      }
      uVar1 = *(undefined4 *)(param_1 + 0x108);
      FUN_00499e10(DAT_00666970,*(undefined4 *)(param_1 + 0x100),*(undefined4 *)(param_1 + 0x104),
                   uVar1,iVar2,uVar6);
      FUN_00499750(uVar1,iVar2,uVar6);
      param_3 = *(int *)(param_1 + 0x2c) + -0x20 + param_3;
      param_4 = *(int *)(param_1 + 0x30) + -0x20 + param_4;
      iStack_4c = param_4 + 0x3f;
      if (*(int *)(param_1 + 0x50) != 0) {
        return;
      }
      iVar2 = *(int *)(param_1 + 0x130);
      if (iVar2 < 0x40) {
        piVar3 = (int *)(param_1 + (iVar2 + 0xb) * 0x1c);
        *piVar3 = param_3;
        piVar3[1] = param_4;
        piVar3[2] = param_3 + 0x3f;
        piVar3[3] = iStack_4c;
        *(undefined4 *)(param_1 + 0x144 + iVar2 * 0x1c) = 0;
        *(int *)(param_1 + 0x130) = *(int *)(param_1 + 0x130) + 1;
        return;
      }
      FUN_004546a0();
      *(undefined4 *)(param_1 + 0x130) = 0;
      return;
    }
  }
  if (DAT_0065b9fc != 0) {
    iVar2 = FUN_00452520(param_3,param_4,0);
    if (iVar2 == 0) {
      return;
    }
    FUN_004405d0(*(undefined4 *)(iVar2 + 0x40),1);
    return;
  }
  if (DAT_006573f4 <= DAT_006573f8) {
    return;
  }
  iVar2 = (&DAT_00656ff4)[DAT_006573f8];
  if (iVar2 < 0) {
    return;
  }
  FUN_0044cf80(0,0x80,0,0,0xffffffff);
  piVar3 = piStack_3c;
  while (piStack_3c = piVar3, piVar3 != (int *)0x0) {
    if (piVar3[0x10] == iVar2) goto LAB_00450841;
    FUN_0044d080();
    piVar3 = piStack_3c;
  }
  piVar3 = (int *)FUN_0051f330(iVar2);
LAB_00450841:
  if (piVar3 == (int *)0x0) {
    return;
  }
  if ((piVar3[2] & 2U) != 0) {
    return;
  }
  if ((piVar3[2] & 0x400U) == 0) {
    if (*(int *)(param_1 + 0x894) == 0) {
      iStack_74 = (*(int *)(param_1 + 0x78) - *(int *)(param_1 + 0x100)) + param_3;
      if (iStack_74 < 1) {
        iStack_74 = (*(int *)(param_1 + 0x100) - *(int *)(param_1 + 0x78)) - param_3;
      }
      iVar2 = (*(int *)(param_1 + 0x7c) - *(int *)(param_1 + 0x104)) + param_4;
      if (iVar2 < 1) {
        iVar2 = (*(int *)(param_1 + 0x104) - *(int *)(param_1 + 0x7c)) - param_4;
      }
      if (iVar2 + iStack_74 < 4) {
        return;
      }
    }
    FUN_00440b20();
  }
  iStack_60 = piVar3[5];
  iStack_5c = piVar3[6];
  iStack_64 = piVar3[4];
  if (DAT_006573f8 < DAT_006573f4) {
    uVar1 = (&DAT_00656ff4)[DAT_006573f8];
  }
  else {
    uVar1 = 0xffffffff;
  }
  FUN_00450bd0(uVar1,param_3 - *(int *)(param_1 + 0x108),param_4 - *(int *)(param_1 + 0x10c));
  iStack_70 = piVar3[4];
  iStack_6c = piVar3[5];
  uStack_68 = piVar3[6];
  if (0 < (int)DAT_00656f00) {
    if (DAT_00656f00 < 4) {
      iVar2 = 0;
    }
    else {
      iVar2 = DAT_00656f10[3];
    }
    if ((~*(uint *)(iVar2 + 0x14) >> 0x10 & 1) != 0) goto LAB_004509b2;
  }
  uStack_68 = ((*(int *)(param_1 + 0x110) - *(int *)(param_1 + 0x104)) - param_4) +
              *(int *)(param_1 + 0x10c);
  if (0 < (int)DAT_00656f00) {
    if (DAT_00656f00 == 0) {
      iVar2 = 0;
    }
    else {
      iVar2 = *DAT_00656f10;
    }
    if ((*(uint *)(iVar2 + 0x14) >> 0x10 & 1) != 0) {
      uStack_68 = uStack_68 & 0xfffffff0;
    }
  }
  (**(code **)(*piVar3 + 8))(&iStack_70,0xffffffff,0);
LAB_004509b2:
  iVar7 = uStack_68 - iStack_5c;
  iVar4 = iStack_6c - iStack_60;
  aiStack_58[0] = iStack_70 - iStack_64;
  DAT_006573fc = 0;
  iVar2 = DAT_00656ff4;
  if ((int)DAT_006573f4 < 1) {
    iVar2 = -1;
  }
  if (iVar2 < 0) {
    DAT_006573fc = 0;
    return;
  }
  do {
    FUN_0044cf80(0,0x80,0,0,0xffffffff);
    piVar5 = piStack_3c;
    while (piStack_3c = piVar5, piVar5 != (int *)0x0) {
      if (piVar5[0x10] == iVar2) goto LAB_00450a31;
      FUN_0044d080();
      piVar5 = piStack_3c;
    }
    piVar5 = (int *)FUN_0051f330(iVar2);
LAB_00450a31:
    if ((piVar5 != (int *)0x0) && (piVar5 != piVar3)) {
      iStack_60 = piVar5[5] + iVar4;
      iStack_5c = piVar5[6] + iVar7;
      iStack_64 = piVar5[4] + aiStack_58[0];
      (**(code **)(*piVar5 + 8))(&iStack_64,0xffffffff,0);
    }
    DAT_006573fc = DAT_006573fc + 1;
    if ((int)DAT_006573f4 <= DAT_006573fc) {
      return;
    }
    iVar2 = (&DAT_00656ff4)[DAT_006573fc];
    if (iVar2 < 0) {
      return;
    }
  } while( true );
}


