// FUN_0053aa90 @ 0053aa90 size=2875

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall FUN_0053aa90(int *param_1,int param_2,int param_3)

{
  undefined4 uVar1;
  int iVar2;
  int *piVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined4 uVar8;
  int iStack_d0;
  undefined4 *puStack_cc;
  undefined4 uStack_c8;
  undefined2 uStack_c4;
  undefined4 uStack_c2;
  undefined4 uStack_be;
  undefined4 uStack_ba;
  undefined2 uStack_b6;
  int iStack_b4;
  int aiStack_b0 [3];
  undefined4 auStack_a4 [16];
  undefined1 uStack_64;
  undefined4 uStack_63;
  
  FUN_00437a80(param_2,param_3);
  if (param_3 == 1) {
    uVar1 = FUN_00436930(s_controller_005e449c);
    iVar2 = FUN_00436900(uVar1);
    param_1[0x71] = iVar2;
    if (iVar2 != 0) {
      FUN_00430c50(DAT_005d697c * 2);
    }
    uVar1 = FUN_00436930(s_RealTime_005e44a8);
    piVar3 = (int *)FUN_00436900(uVar1);
    if (piVar3 != (int *)0x0) {
      if (param_1[0x60] == 0) {
        (**(code **)(*piVar3 + 0x1c))(piVar3[5] & 0xfffeffff);
      }
      else {
        (**(code **)(*piVar3 + 0x1c))(piVar3[5] | 0x10000);
      }
      (**(code **)(*piVar3 + 0x1c))(piVar3[5] | 0x20);
    }
    uVar1 = FUN_00436930(&DAT_005e44b4);
    piVar3 = (int *)FUN_00436900(uVar1);
    if (piVar3 != (int *)0x0) {
      if (param_1[0x61] == 0) {
        uVar4 = piVar3[5] & 0xfffeffff;
      }
      else {
        uVar4 = piVar3[5] | 0x10000;
      }
      (**(code **)(*piVar3 + 0x1c))(uVar4);
      (**(code **)(*piVar3 + 0x1c))(piVar3[5] | 0x20);
    }
    uVar1 = FUN_00436930(&DAT_005e44bc);
    piVar3 = (int *)FUN_00436900(uVar1);
    if (piVar3 != (int *)0x0) {
      if (param_1[99] == 0) {
        uVar4 = piVar3[5] & 0xfffeffff;
      }
      else {
        uVar4 = piVar3[5] | 0x10000;
      }
      (**(code **)(*piVar3 + 0x1c))(uVar4);
      (**(code **)(*piVar3 + 0x1c))(piVar3[5] | 0x20);
    }
    uVar1 = FUN_00436930(s_Dialog_005e44c4);
    piVar3 = (int *)FUN_00436900(uVar1);
    if (piVar3 != (int *)0x0) {
      if (param_1[0x62] == 0) {
        uVar4 = piVar3[5] & 0xfffeffff;
      }
      else {
        uVar4 = piVar3[5] | 0x10000;
      }
      (**(code **)(*piVar3 + 0x1c))(uVar4);
      (**(code **)(*piVar3 + 0x1c))(piVar3[5] | 0x20);
    }
    uVar1 = FUN_00436930(s_Enhanced_005e44cc);
    piVar3 = (int *)FUN_00436900(uVar1);
    if (piVar3 != (int *)0x0) {
      if (((DAT_006697bc == 0) && (DAT_006697c0 == 0)) || (DAT_006680f4 != 0)) {
        iVar2 = *piVar3;
        uVar4 = piVar3[5] | 4;
      }
      else {
        iVar2 = *piVar3;
        uVar4 = piVar3[5] & 0xfffffffb;
      }
      (**(code **)(iVar2 + 0x1c))(uVar4);
      if (param_1[100] == 0) {
        uVar4 = piVar3[5] & 0xfffeffff;
      }
      else {
        uVar4 = piVar3[5] | 0x10000;
      }
      (**(code **)(*piVar3 + 0x1c))(uVar4);
      (**(code **)(*piVar3 + 0x1c))(piVar3[5] | 0x20);
    }
    uVar1 = FUN_00436930(s_Limit_005e44d8);
    piVar3 = (int *)FUN_00436900(uVar1);
    if (piVar3 != (int *)0x0) {
      if (param_1[0x65] == 0) {
        uVar4 = piVar3[5] & 0xfffeffff;
      }
      else {
        uVar4 = piVar3[5] | 0x10000;
      }
      (**(code **)(*piVar3 + 0x1c))(uVar4);
      (**(code **)(*piVar3 + 0x1c))(piVar3[5] | 0x20);
    }
    uVar1 = FUN_00436930(s_NoCombatRes_005e44e0);
    piVar3 = (int *)FUN_00436900(uVar1);
    if (piVar3 != (int *)0x0) {
      if (param_1[0x66] == 0) {
        uVar4 = piVar3[5] & 0xfffeffff;
      }
      else {
        uVar4 = piVar3[5] | 0x10000;
      }
      (**(code **)(*piVar3 + 0x1c))(uVar4);
      (**(code **)(*piVar3 + 0x1c))(piVar3[5] | 0x20);
    }
    uVar1 = FUN_00436930(s_Violence_005e44ec);
    iVar2 = FUN_00436900(uVar1);
    if (iVar2 != 0) {
      FUN_0042e3d0(0,4);
      iVar6 = param_1[0x67];
      *(undefined4 *)(iVar2 + 0xa0) = 1;
      FUN_0042e440(iVar6);
    }
    uVar1 = FUN_00436930(s_Music_005e44f8);
    iVar2 = FUN_00436900(uVar1);
    if (iVar2 != 0) {
      FUN_0042e3d0(0,0x60);
      iVar6 = param_1[0x68];
      *(undefined4 *)(iVar2 + 0xa0) = 0xc;
      FUN_0042e440(iVar6);
    }
    uVar1 = FUN_00436930(s_Sound_005e4500);
    iVar2 = FUN_00436900(uVar1);
    if (iVar2 != 0) {
      FUN_0042e3d0(0,0x7f);
      iVar6 = param_1[0x69];
      *(undefined4 *)(iVar2 + 0xa0) = 0xf;
      FUN_0042e440(iVar6);
    }
    uVar1 = FUN_00436930(s_Gamma_005e4508);
    iVar2 = FUN_00436900(uVar1);
    if (iVar2 != 0) {
      FUN_0042e3d0(0,4);
      iVar6 = param_1[0x6a];
      *(undefined4 *)(iVar2 + 0xa0) = 1;
      FUN_0042e440(iVar6);
    }
    uVar1 = FUN_00436930(s_controller_005e4510);
    iVar2 = FUN_00436900(uVar1);
    if (iVar2 == 0) {
      return 1;
    }
    iVar2 = *(int *)(iVar2 + 0x9c);
    uStack_c8 = DAT_005e451c;
    uStack_c2 = 0;
    uStack_be = 0;
    uStack_c4 = DAT_005e4520;
    uStack_ba = 0;
    uStack_b6 = 0;
    iVar6 = 0;
    do {
      FUN_00439060(iVar6 + iVar2 / 2,aiStack_b0);
      uStack_c4 = CONCAT11(uStack_c4._1_1_,(char)iVar6 + '0');
      uVar1 = FUN_00436930(&uStack_c8);
      piVar3 = (int *)FUN_00436900(uVar1);
      if ((piVar3 != (int *)0x0) && (aiStack_b0[0] != 0)) {
        uStack_64 = DAT_0066fef8;
        puVar7 = &uStack_63;
        for (iVar5 = 0x18; iVar5 != 0; iVar5 = iVar5 + -1) {
          *puVar7 = 0;
          puVar7 = puVar7 + 1;
        }
        *(undefined2 *)puVar7 = 0;
        *(undefined1 *)((int)puVar7 + 2) = 0;
        FUN_0046e7f0(&uStack_64,100,aiStack_b0[0]);
        (**(code **)(*piVar3 + 0x18))(&uStack_64);
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < 8);
    return 1;
  }
  if (param_3 != 3000) {
    if (param_3 == 0xbb9) {
      param_2 = param_2 + 0x18;
      iVar2 = FUN_0059a530(s_Realtime_005e454c,param_2);
      if (iVar2 == 0) {
        param_1[0x60] = 1;
        return 1;
      }
      iVar2 = FUN_0059a530(&DAT_005e4558,param_2);
      if (iVar2 == 0) {
        param_1[0x61] = 1;
        return 1;
      }
      iVar2 = FUN_0059a530(s_Dialog_005e4560,param_2);
      if (iVar2 == 0) {
        param_1[0x62] = 1;
        return 1;
      }
      iVar2 = FUN_0059a530(&DAT_005e4568,param_2);
      if (iVar2 != 0) {
        iVar2 = FUN_0059a530(s_Enhanced_005e4570,param_2);
        if (iVar2 == 0) {
          param_1[100] = 1;
          return 1;
        }
        iVar2 = FUN_0059a530(s_Limit_005e457c,param_2);
        if (iVar2 != 0) {
          iVar2 = FUN_0059a530(s_NoCombatRes_005e4584,param_2);
          if (iVar2 != 0) {
            return 1;
          }
          param_1[0x66] = 1;
          return 1;
        }
        param_1[0x65] = 1;
        return 1;
      }
      param_1[99] = 1;
      return 1;
    }
    if (param_3 != 0xbba) {
      if (param_3 != 4000) {
        if (param_3 != 0x1389) {
          if (param_3 != 0x138a) {
            return 1;
          }
          iVar2 = *(int *)(param_2 + 0xa0);
          param_1[0x6c] = 1;
          param_1[0x70] = iVar2;
          return 1;
        }
        iVar2 = FUN_0059a530(s_Controller_005e45f8,param_2 + 0x18);
        if (iVar2 != 0) {
          return 1;
        }
        uStack_c4 = DAT_005e4608;
        uStack_b6 = 0;
        iVar2 = *(int *)(param_2 + 0x9c);
        uStack_c8 = DAT_005e4604;
        iVar6 = 0;
        uStack_c2 = 0;
        uStack_be = 0;
        uStack_ba = 0;
        do {
          FUN_00439060(iVar6 + iVar2 / 2,aiStack_b0);
          uStack_c4 = CONCAT11(uStack_c4._1_1_,(char)iVar6 + '0');
          uVar1 = FUN_00436930(&uStack_c8);
          piVar3 = (int *)FUN_00436900(uVar1);
          if ((piVar3 != (int *)0x0) && (aiStack_b0[0] != 0)) {
            uStack_64 = DAT_0066fefc;
            puVar7 = &uStack_63;
            for (iVar5 = 0x18; iVar5 != 0; iVar5 = iVar5 + -1) {
              *puVar7 = 0;
              puVar7 = puVar7 + 1;
            }
            *(undefined2 *)puVar7 = 0;
            *(undefined1 *)((int)puVar7 + 2) = 0;
            FUN_0046e7f0(&uStack_64,100,aiStack_b0[0]);
            (**(code **)(*piVar3 + 0x18))(&uStack_64);
          }
          iVar6 = iVar6 + 1;
        } while (iVar6 < 8);
        return 1;
      }
      iVar2 = param_2 + 0x18;
      iVar6 = FUN_0059a530(s_Violence_005e45d4,iVar2);
      if (iVar6 == 0) {
        param_1[0x67] = *(int *)(param_2 + 0x94);
        return 1;
      }
      iVar6 = FUN_0059a530(s_Music_005e45e0,iVar2);
      if (iVar6 != 0) {
        iVar6 = FUN_0059a530(s_Sound_005e45e8,iVar2);
        if (iVar6 == 0) {
          param_1[0x69] = *(int *)(param_2 + 0x94);
          return 1;
        }
        iVar2 = FUN_0059a530(s_Gamma_005e45f0,iVar2);
        if (iVar2 != 0) {
          return 1;
        }
        iVar2 = *(int *)(param_2 + 0x94);
        param_1[0x6a] = iVar2;
        FUN_004a98f0(iVar2);
        return 1;
      }
      iVar2 = *(int *)(param_2 + 0x94);
      param_1[0x68] = iVar2;
      FUN_0049a5c0(iVar2);
      return 1;
    }
    param_2 = param_2 + 0x18;
    iVar2 = FUN_0059a530(s_Realtime_005e4590,param_2);
    if (iVar2 == 0) {
      param_1[0x60] = 0;
      return 1;
    }
    iVar2 = FUN_0059a530(&DAT_005e459c,param_2);
    if (iVar2 == 0) {
      param_1[0x61] = 0;
      return 1;
    }
    iVar2 = FUN_0059a530(s_Dialog_005e45a4,param_2);
    if (iVar2 == 0) {
      param_1[0x62] = 0;
      return 1;
    }
    iVar2 = FUN_0059a530(&DAT_005e45ac,param_2);
    if (iVar2 != 0) {
      iVar2 = FUN_0059a530(s_Enhanced_005e45b4,param_2);
      if (iVar2 == 0) {
        param_1[100] = 0;
        return 1;
      }
      iVar2 = FUN_0059a530(s_Limit_005e45c0,param_2);
      if (iVar2 != 0) {
        iVar2 = FUN_0059a530(s_NoCombatRes_005e45c8,param_2);
        if (iVar2 != 0) {
          return 1;
        }
        param_1[0x66] = 0;
        return 1;
      }
      param_1[0x65] = 0;
      return 1;
    }
    param_1[99] = 0;
    return 1;
  }
  param_2 = param_2 + 0x18;
  iVar2 = FUN_0059a530(&DAT_005e4524,param_2);
  if (iVar2 == 0) {
    DAT_005d7a18 = param_1[0x60];
    DAT_005d7a60 = param_1[0x62];
    DAT_005d7a68 = param_1[0x61];
    DAT_005e91c0 = param_1[100];
    DAT_00668194 = param_1[0x66];
    DAT_005d79e8 = param_1[0x67];
    DAT_005d7a9c = param_1[0x68];
    DAT_006682a8 = (uint)(param_1[0x65] == 0);
    FUN_0049a5c0(DAT_005d7a9c);
    DAT_005d7a48 = param_1[0x6a];
    DAT_005d7aa0 = param_1[0x69];
    FUN_004a98f0(DAT_005d7a48);
    FUN_00453640(DAT_006671a4,1);
    DAT_005d7a64 = param_1[99];
    FUN_00484ed0();
    iVar2 = 0;
    if (0 < DAT_005d697c) {
      iVar6 = 0;
      do {
        puStack_cc = auStack_a4;
        iStack_d0 = 3;
        do {
          iStack_b4 = 3;
          puVar7 = puStack_cc;
          do {
            FUN_00439060(iVar2,aiStack_b0);
            *puVar7 = *(undefined4 *)(iVar6 + param_1[0x6b]);
            FUN_00439110(iVar2,aiStack_b0);
            iVar6 = iVar6 + 4;
            puVar7 = puVar7 + 1;
            iStack_b4 = iStack_b4 + -1;
          } while (iStack_b4 != 0);
          puStack_cc = puStack_cc + 4;
          iStack_d0 = iStack_d0 + -1;
        } while (iStack_d0 != 0);
        iVar2 = iVar2 + 1;
        iStack_b4 = 0;
      } while (iVar2 < DAT_005d697c);
    }
    FUN_00439dc0(s_Controls_005e4528);
    if (param_1[0x5f] == 0) {
      _DAT_0066fea0 = &DAT_0065d358;
      FUN_0048ea40();
    }
    else {
      (**(code **)(*param_1 + 8))();
    }
    param_1[0x5f] = 0;
    FUN_00435010();
    return 1;
  }
  iVar2 = FUN_0059a530(s_cancel_005e4534,param_2);
  if (iVar2 == 0) {
    FUN_004a98f0(DAT_005d7a48);
    if (param_1[0x5f] != 0) {
      (**(code **)(*param_1 + 8))();
      goto LAB_0053b166;
    }
  }
  else {
    iVar2 = FUN_0059a530(s_ControlSetup_005e453c,param_2);
    if (iVar2 != 0) {
      return 1;
    }
    if (param_1[0x5f] != 0) {
      (**(code **)(*param_1 + 8))();
      iVar2 = FUN_00537110();
      if (iVar2 != 0) {
        FUN_0048ed90(&DAT_0066f748,0xffffffff);
        uVar8 = 0xffff;
        uVar1 = FUN_0048ed60(&DAT_0066f748);
        FUN_0048eea0(uVar1,uVar8);
      }
      goto LAB_0053b166;
    }
  }
  _DAT_0066fea0 = &DAT_0065d358;
  FUN_0048ea40();
LAB_0053b166:
  param_1[0x5f] = 0;
  FUN_00435010();
  return 1;
}


