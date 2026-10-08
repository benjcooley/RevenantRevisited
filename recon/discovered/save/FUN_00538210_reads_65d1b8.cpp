// FUN_00538210 @ 00538210 size=2555

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_00538210(int *param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int *piVar2;
  uint uVar3;
  int *piVar4;
  int iVar5;
  undefined4 uVar6;
  int *piVar7;
  char *pcVar8;
  
  FUN_00436530(param_2,param_3,param_4);
  iVar1 = param_1[0x5f];
  if (iVar1 == 0) {
    return;
  }
  if (param_2 == 5) {
    if (param_3 < 0) {
      return;
    }
    if (param_4 < 0) {
      return;
    }
    if (param_1[3] <= param_3) {
      return;
    }
    if (param_1[4] <= param_4) {
      return;
    }
    if (DAT_0065c9e0 != 0) {
      param_3 = param_3 + -8;
      param_4 = param_4 + -0x2a;
      if ((((param_3 < 0) || (param_4 < 0)) || (3 < param_3 / 0x2d)) ||
         (((2 < param_4 / 0x2c || (0x27 < param_3 % 0x2d)) || (0x27 < param_4 % 0x2c)))) {
        iVar1 = -1;
      }
      else {
        iVar1 = (param_3 / 0x2d) * 3 + param_1[0x62] + param_4 / 0x2c;
      }
      iVar1 = FUN_004701f0(iVar1);
      if (iVar1 == 0) {
        if (DAT_0065d1b8 != 1) {
          return;
        }
        FUN_005496a0(DAT_00667fcc);
        return;
      }
      FUN_0047eff0();
      FUN_005496a0(iVar1);
      return;
    }
    if (-1 < param_1[0x60]) {
      return;
    }
    if (-1 < DAT_0065b878) {
      return;
    }
    if (-1 < DAT_0065b090) {
      return;
    }
    param_3 = param_3 + -8;
    param_4 = param_4 + -0x2a;
    if ((((param_3 < 0) || (param_4 < 0)) || (3 < param_3 / 0x2d)) ||
       (((2 < param_4 / 0x2c || (0x27 < param_3 % 0x2d)) || (0x27 < param_4 % 0x2c)))) {
      iVar1 = -1;
    }
    else {
      iVar1 = (param_3 / 0x2d) * 3 + param_1[0x62] + param_4 / 0x2c;
    }
    piVar2 = (int *)FUN_004701f0(iVar1);
    if (piVar2 == (int *)0x0) {
      if (param_1[0x5f] == 0) {
        return;
      }
      FUN_0046ff80();
      (**(code **)(*param_1 + 0x28))();
      return;
    }
    uVar3 = (**(code **)(*piVar2 + 0xd4))(s_eqslot_005e41f4);
    if (((short)piVar2[1] == 3) && ((int *)piVar2[0x19] == DAT_00667fcc)) {
      piVar4 = (int *)(**(code **)(*DAT_00667fcc + 0xa8))(s_Spell_Pouch_005e41fc);
      if (piVar4 != (int *)0x0) {
        (**(code **)(*piVar4 + 0x58))(piVar2,0xffffffff);
        iVar1 = FUN_0049c430(s_pouch_005e4208);
        if (iVar1 < 0) {
          return;
        }
        iVar5 = FUN_0049b650(iVar1);
        if (iVar5 == 0) {
          return;
        }
        FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
        return;
      }
      pcVar8 = s_EQUIPUNABLE_005e4210;
    }
    else {
      if (uVar3 != 0xffffffff) {
        if (10 < uVar3) {
          return;
        }
        iVar1 = FUN_00519300(piVar2,uVar3);
        if (iVar1 == 0) {
          return;
        }
        iVar1 = FUN_005199b0(piVar2,uVar3);
        if (iVar1 != 0) {
          FUN_00473a10();
        }
        (**(code **)(*param_1 + 0x28))();
        if ((short)piVar2[1] != 6) {
          return;
        }
        FUN_004cf000();
        return;
      }
      pcVar8 = s_EQUIPUNABLE_005e421c;
    }
    uVar6 = FUN_0049d800(pcVar8);
    FUN_0054d170(&DAT_0065c5d0,uVar6);
    return;
  }
  if (param_2 == 1) {
    if (param_3 < 0) {
      return;
    }
    if (param_4 < 0) {
      return;
    }
    if (param_1[3] <= param_3) {
      return;
    }
    if (param_1[4] <= param_4) {
      return;
    }
    iVar1 = param_3 + -8;
    if (((iVar1 < 0) || (param_4 + -0x2a < 0)) ||
       ((3 < iVar1 / 0x2d ||
        (((iVar5 = (param_4 + -0x2a) / 0x2c, 2 < iVar5 || (0x27 < iVar1 % 0x2d)) ||
         (0x27 < (param_4 + -0x2a) % 0x2c)))))) {
      iVar5 = -1;
    }
    else {
      iVar5 = (iVar1 / 0x2d) * 3 + param_1[0x62] + iVar5;
    }
    param_1[100] = param_3;
    param_1[0x60] = iVar5;
    param_1[0x61] = iVar5;
    param_1[0x65] = param_4;
    param_1[99] = 0;
    return;
  }
  if (param_2 != 4) {
    return;
  }
  if (((param_3 < 10) || (0x1e < param_3)) || ((param_4 < 10 || (0x1d < param_4)))) {
    if (-1 < param_1[0x60]) {
      if (param_3 < 0) goto LAB_00538721;
      if (((param_4 < 0) || (param_1[3] <= param_3)) || (param_1[4] <= param_4)) goto LAB_00538900;
      iVar5 = param_3 + -8;
      iVar1 = param_4 + -0x2a;
      if (((iVar5 < 0) || (iVar1 < 0)) ||
         ((3 < iVar5 / 0x2d ||
          ((((2 < iVar1 / 0x2c || (0x27 < iVar5 % 0x2d)) || (0x27 < iVar1 % 0x2c)) ||
           (iVar1 = (iVar5 / 0x2d) * 3 + param_1[0x62] + iVar1 / 0x2c, iVar1 < 0))))))
      goto LAB_00538728;
      piVar2 = (int *)FUN_004701f0(param_1[0x60]);
      piVar4 = (int *)FUN_004701f0(iVar1);
      if (piVar2 == (int *)0x0) goto LAB_00538728;
      if (piVar4 != (int *)0x0) {
        if (param_1[99] == 0) {
LAB_00538884:
          if (piVar4 != piVar2) goto LAB_005388e0;
          param_2 = -1;
        }
        else {
          if (piVar4 == piVar2) {
            if (param_1[99] != 0) goto LAB_005388e0;
            goto LAB_00538884;
          }
          param_2 = piVar2[0x10];
        }
        if ((((DAT_0066829c == 0) || (iVar1 == param_1[0x60])) ||
            (((short)piVar4[1] == 0x11 ||
             (iVar5 = (**(code **)(*piVar4 + 0xb8))(DAT_00667fcc,param_2), iVar5 != 0)))) &&
           (iVar5 = (**(code **)(*piVar4 + 0xbc))(DAT_00667fcc,param_2), iVar5 != 0))
        goto LAB_00538721;
      }
LAB_005388e0:
      (**(code **)(*(int *)param_1[0x5f] + 0x58))(piVar2,iVar1);
      pcVar8 = &DAT_005e4230;
      goto LAB_005386ea;
    }
LAB_00538900:
    if ((((-1 < param_3) && (-1 < param_4)) && (param_3 < param_1[3])) && (param_4 < param_1[4])) {
      iVar1 = param_3 + -8;
      iVar5 = param_4 + -0x2a;
      if (((iVar1 < 0) || (iVar5 < 0)) ||
         (((3 < iVar1 / 0x2d || ((2 < iVar5 / 0x2c || (0x27 < iVar1 % 0x2d)))) ||
          (0x27 < iVar5 % 0x2c)))) {
        iVar1 = -1;
      }
      else {
        iVar1 = (iVar1 / 0x2d) * 3 + param_1[0x62] + iVar5 / 0x2c;
      }
      if (DAT_0065b878 < 0) {
        if (-1 < DAT_0065b090) {
          _DAT_0065b078 = 1;
          piVar4 = (int *)FUN_004701f0(DAT_0065b090 + 0x10b);
          piVar7 = (int *)FUN_004701f0(iVar1);
          piVar2 = DAT_0065d674;
          if (piVar4 == (int *)0x0) {
            return;
          }
          for (; piVar2 != (int *)0x0; piVar2 = (int *)piVar2[0x19]) {
            if (piVar2 == piVar4) {
              return;
            }
          }
          if (piVar7 != (int *)0x0) {
            if (piVar7 == piVar4) {
              param_2 = -1;
            }
            else {
              param_2 = piVar4[0x10];
            }
            if ((((DAT_0066829c == 0) || ((short)piVar7[1] == 0x11)) ||
                (iVar5 = (**(code **)(*piVar7 + 0xb8))(DAT_00667fcc,param_2), iVar5 != 0)) &&
               (iVar5 = (**(code **)(*piVar7 + 0xbc))(DAT_00667fcc,param_2), iVar5 != 0)) {
              (**(code **)(*param_1 + 0x28))();
              goto LAB_00538728;
            }
          }
          (**(code **)(*(int *)param_1[0x5f] + 0x58))(piVar4,iVar1);
          iVar1 = FUN_0049c430(&DAT_005e4238);
          if ((-1 < iVar1) && (iVar5 = FUN_0049b650(iVar1), iVar5 != 0)) {
            FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
          }
          (**(code **)(*param_1 + 0x28))();
        }
        goto LAB_00538728;
      }
      piVar2 = (int *)DAT_00667fcc[DAT_0065b878 + 0xa8];
      piVar4 = (int *)FUN_004701f0(iVar1);
      if ((piVar2 != (int *)0x0) && (-1 < iVar1)) {
        if (piVar4 == (int *)0x0) {
          FUN_005199b0(0,DAT_0065b878);
          (**(code **)(*(int *)param_1[0x5f] + 0x58))(piVar2,iVar1);
          iVar1 = FUN_0049c430(&DAT_005e4234);
          if ((-1 < iVar1) && (iVar5 = FUN_0049b650(iVar1), iVar5 != 0)) {
            FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
          }
        }
        else {
          iVar5 = FUN_00519300(piVar4,DAT_0065b878);
          if (iVar5 == 0) {
            if (param_1[99] == 0) {
LAB_00538a70:
              if (piVar4 != piVar2) goto LAB_00538acf;
              param_2 = -1;
            }
            else {
              if (piVar4 == piVar2) {
                if (param_1[99] != 0) goto LAB_00538acf;
                goto LAB_00538a70;
              }
              param_2 = piVar2[0x10];
            }
            if (((((DAT_0066829c != 0) && (iVar1 != param_1[0x60])) && ((short)piVar4[1] != 0x11))
                && (iVar1 = (**(code **)(*piVar4 + 0xb8))(DAT_00667fcc,param_2), iVar1 == 0)) ||
               (iVar1 = (**(code **)(*piVar4 + 0xbc))(DAT_00667fcc,param_2), iVar1 == 0))
            goto LAB_00538acf;
          }
          else {
            FUN_005199b0(piVar4,DAT_0065b878);
          }
        }
        (**(code **)(*param_1 + 0x28))();
      }
LAB_00538acf:
      if ((short)piVar2[1] == 6) {
        FUN_004cf000();
      }
      goto LAB_00538728;
    }
  }
  else {
    iVar5 = *(int *)(iVar1 + 100);
    if (iVar5 == 0) goto LAB_00538728;
    if (param_1[0x60] < 0) {
      if (iVar1 != iVar5) {
        param_1[0x5f] = iVar5;
        FUN_00538e40();
        (**(code **)(*param_1 + 0x28))();
        iVar1 = FUN_00539260(0);
        iVar5 = FUN_00539260(1);
        if ((iVar5 != 0) && (iVar1 != 0)) {
          param_1[0x62] = 0;
          FUN_00539230(1);
          FUN_00539230(0);
        }
        if (DAT_0066829c != 0) {
          FUN_00584680();
        }
      }
      goto LAB_00538728;
    }
    iVar1 = FUN_004701f0(param_1[0x60]);
    if ((iVar1 == 0) ||
       (iVar5 = (**(code **)(**(int **)(param_1[0x5f] + 100) + 0x94))(), 0xfe < iVar5))
    goto LAB_00538728;
    iVar5 = **(int **)(param_1[0x5f] + 100);
    uVar6 = (**(code **)(iVar5 + 0x94))();
    (**(code **)(iVar5 + 0x58))(iVar1,uVar6);
    pcVar8 = s_pouch_005e4228;
LAB_005386ea:
    iVar1 = FUN_0049c430(pcVar8);
    if ((-1 < iVar1) && (iVar5 = FUN_0049b650(iVar1), iVar5 != 0)) {
      FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
    }
  }
LAB_00538721:
  (**(code **)(*param_1 + 0x28))();
LAB_00538728:
  param_1[0x60] = -1;
  param_1[99] = 0;
  if ((((-1 < param_3) && (-1 < param_4)) && (param_3 < param_1[3])) && (param_4 < param_1[4])) {
    FUN_0043a100(0,0,0);
    FUN_0043a140(0);
    FUN_0043a170(&DAT_0066f8c4);
    FUN_0043a240(0,0,0);
    (**(code **)(*param_1 + 0x28))();
  }
  return;
}


