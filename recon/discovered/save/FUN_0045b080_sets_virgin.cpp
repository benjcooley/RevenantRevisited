// FUN_0045b080 @ 0045b080 size=1862

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Enum "SectionFlags": Some values do not have unique names */

void __thiscall FUN_0045b080(int *param_1,int param_2)

{
  HANDLE hMutex;
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  bool bVar5;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059d04b;
  local_c = ExceptionList;
  if (DAT_0066818c != 0) {
    param_2 = 0;
  }
  if ((param_2 != param_1[0x26a]) || (param_1[0x217] == 0)) {
    ExceptionList = &local_c;
    if (DAT_0065844c != 0) {
      ExceptionList = &local_c;
      FUN_00481e80(DAT_006584b4);
      _DAT_00658448 = s_d__revenant_MapPane_cpp_005d0728;
      _DAT_00658d88 = 0x16e1;
    }
    if (param_2 == 0) {
      if ((((param_1[0x218] != 0) || (param_1[0x216] != 0)) || (param_1[0x217] != 0)) ||
         (param_1[0x219] != 0)) {
        FUN_004aa490(param_1[0x16]);
        (**(code **)(*param_1 + 0x18))();
        if ((undefined4 *)param_1[0x219] != (undefined4 *)0x0) {
          (*(code *)**(undefined4 **)param_1[0x219])(1);
        }
        param_1[0x219] = 0;
        if ((undefined4 *)param_1[0x217] != (undefined4 *)0x0) {
          (*(code *)**(undefined4 **)param_1[0x217])(1);
        }
        param_1[0x217] = 0;
        if ((undefined4 *)param_1[0x216] != (undefined4 *)0x0) {
          (*(code *)**(undefined4 **)param_1[0x216])(1);
        }
        param_1[0x216] = 0;
        if ((undefined4 *)param_1[0x218] != (undefined4 *)0x0) {
          (*(code *)**(undefined4 **)param_1[0x218])(1);
        }
        param_1[0x218] = 0;
      }
      iVar2 = DAT_0066818c;
      param_1[0x218] = 0;
      if (iVar2 == 0) {
        piVar1 = (int *)FUN_00482fb0(0x74);
        uStack_4 = 0xc;
        if (piVar1 == (int *)0x0) {
          piVar1 = (int *)0x0;
        }
        else {
          bVar5 = DAT_005d7a34 != 0;
          FUN_004bcb00();
          uStack_4 = CONCAT31(uStack_4._1_3_,0xd);
          *piVar1 = (int)&PTR_FUN_005a3e7c;
          iVar2 = FUN_004bb440(0x300,0x300,0x180,0x300,2,1,
                               (-(uint)bVar5 & 0xffff0000) + 0x20000 | 0x8800);
          if (iVar2 == 0) {
            FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
          }
        }
        iVar2 = *piVar1;
        uStack_4 = 0xffffffff;
        param_1[0x217] = (int)piVar1;
        (**(code **)(iVar2 + 0x48))(1);
        piVar1 = (int *)FUN_00482fb0(0x74);
        puStack_8 = (undefined1 *)0xe;
        if (piVar1 == (int *)0x0) {
          piVar1 = (int *)0x0;
        }
        else {
          iVar2 = param_1[0x217];
          bVar5 = DAT_005d7a34 != 0;
          FUN_004bcb00();
          puStack_8 = (undefined1 *)CONCAT31(puStack_8._1_3_,0xf);
          *piVar1 = (int)&PTR_FUN_005a3e7c;
          iVar2 = FUN_004bb950(iVar2,(-(uint)bVar5 & 0x3e8000) + 0x18000 | 0x240);
          if (iVar2 == 0) {
            FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
          }
        }
        iVar2 = *piVar1;
        bVar5 = DAT_005d7a34 != 0;
        puStack_8 = (undefined1 *)0xffffffff;
        param_1[0x216] = (int)piVar1;
        param_1[0x21a] = (uint)bVar5;
        (**(code **)(iVar2 + 0x48))(1);
      }
      else {
        piVar1 = (int *)FUN_00482fb0(0x74);
        uStack_4 = 8;
        if (piVar1 == (int *)0x0) {
          piVar1 = (int *)0x0;
        }
        else {
          FUN_004bcb00();
          uStack_4 = CONCAT31(uStack_4._1_3_,9);
          *piVar1 = (int)&PTR_FUN_005a3e7c;
          iVar2 = FUN_004bb440(0x300,0x300,0x180,0x300,2,1,0x18400);
          if (iVar2 == 0) {
            FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
          }
        }
        iVar2 = *piVar1;
        uStack_4 = 0xffffffff;
        param_1[0x217] = (int)piVar1;
        (**(code **)(iVar2 + 0x48))(1);
        piVar1 = (int *)FUN_00482fb0(0x74);
        puStack_8 = (undefined1 *)0xa;
        if (piVar1 == (int *)0x0) {
          piVar1 = (int *)0x0;
        }
        else {
          iVar2 = param_1[0x217];
          FUN_004bcb00();
          puStack_8 = (undefined1 *)CONCAT31(puStack_8._1_3_,0xb);
          *piVar1 = (int)&PTR_FUN_005a3e7c;
          iVar2 = FUN_004bb950(iVar2,0x400240);
          if (iVar2 == 0) {
            FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
          }
        }
        iVar2 = *piVar1;
        puStack_8 = (undefined1 *)0xffffffff;
        param_1[0x216] = (int)piVar1;
        param_1[0x21a] = 1;
        (**(code **)(iVar2 + 0x48))(1);
      }
      piVar1 = (int *)FUN_00482fb0(0x74);
      local_c = (void *)0x10;
      if (piVar1 == (int *)0x0) {
        piVar1 = (int *)0x0;
      }
      else {
        iVar2 = param_1[0x217];
        FUN_004bcb00();
        local_c = (void *)CONCAT31(local_c._1_3_,0x11);
        *piVar1 = (int)&PTR_FUN_005a3e7c;
        iVar2 = FUN_004bb950(iVar2,0x600000);
        if (iVar2 == 0) {
          FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
        }
      }
      iVar2 = *piVar1;
      local_c = (void *)0xffffffff;
      param_1[0x219] = (int)piVar1;
      (**(code **)(iVar2 + 0x48))(1);
      uVar3 = FUN_004aa850(param_1[1],param_1[2],param_1[3],param_1[4],param_1[0x219]);
      (**(code **)(*param_1 + 0x14))(uVar3);
      iVar2 = param_1[0x16];
      uVar4 = FUN_004aa530(iVar2);
      FUN_004aa500(iVar2,uVar4 & 0xfffffffe);
      if (DAT_00668154 == 0) {
        iVar2 = 0;
        if (0 < DAT_00668578) {
          do {
            if (*(int *)(DAT_00668588 + iVar2 * 4) != 0) {
              FUN_00499500();
            }
            iVar2 = iVar2 + 1;
          } while (iVar2 < DAT_00668578);
        }
        FUN_004546a0();
      }
    }
    else {
      if (((param_1[0x217] != 0) || (param_1[0x216] != 0)) || (param_1[0x219] != 0)) {
        FUN_004aa490(param_1[0x16]);
        (**(code **)(*param_1 + 0x18))();
        if ((undefined4 *)param_1[0x219] != (undefined4 *)0x0) {
          (*(code *)**(undefined4 **)param_1[0x219])(1);
        }
        param_1[0x219] = 0;
        if ((undefined4 *)param_1[0x216] != (undefined4 *)0x0) {
          (*(code *)**(undefined4 **)param_1[0x216])(1);
        }
        param_1[0x216] = 0;
        if ((undefined4 *)param_1[0x217] != (undefined4 *)0x0) {
          (*(code *)**(undefined4 **)param_1[0x217])(1);
        }
        param_1[0x217] = 0;
      }
      piVar1 = (int *)FUN_00482fb0(0x74);
      uStack_4 = 0;
      if (piVar1 == (int *)0x0) {
        piVar1 = (int *)0x0;
      }
      else {
        bVar5 = DAT_005d7a34 != 0;
        FUN_004bcb00();
        uStack_4 = CONCAT31(uStack_4._1_3_,1);
        *piVar1 = (int)&PTR_FUN_005a3e7c;
        iVar2 = FUN_004bb5c0(0x300,0x300,(-(uint)bVar5 & 0xffff0000) + 0x20000 | 0x4008890);
        if (iVar2 == 0) {
          FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
        }
      }
      iVar2 = *piVar1;
      uStack_4 = 0xffffffff;
      param_1[0x218] = (int)piVar1;
      (**(code **)(iVar2 + 0x48))(1);
      piVar1 = (int *)FUN_00482fb0(0x74);
      puStack_8 = (undefined1 *)0x2;
      if (piVar1 == (int *)0x0) {
        piVar1 = (int *)0x0;
      }
      else {
        iVar2 = param_1[0x218];
        FUN_004bcb00();
        puStack_8 = (undefined1 *)CONCAT31(puStack_8._1_3_,3);
        *piVar1 = (int)&PTR_FUN_005a3e7c;
        iVar2 = FUN_004bb950(iVar2,&DAT_00400490);
        if (iVar2 == 0) {
          FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
        }
      }
      iVar2 = *piVar1;
      puStack_8 = (undefined1 *)0xffffffff;
      param_1[0x217] = (int)piVar1;
      (**(code **)(iVar2 + 0x48))(1);
      piVar1 = (int *)FUN_00482fb0(0x74);
      local_c = (void *)0x4;
      if (piVar1 == (int *)0x0) {
        piVar1 = (int *)0x0;
      }
      else {
        iVar2 = param_1[0x217];
        bVar5 = DAT_005d7a34 != 0;
        FUN_004bcb00();
        local_c = (void *)CONCAT31(local_c._1_3_,5);
        *piVar1 = (int)&PTR_FUN_005a3e7c;
        iVar2 = FUN_004bb950(iVar2,(-(uint)bVar5 & 0x3e8000) + 0x18000 | 0x200000);
        if (iVar2 == 0) {
          FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
        }
      }
      iVar2 = *piVar1;
      bVar5 = DAT_005d7a34 != 0;
      local_c = (void *)0xffffffff;
      param_1[0x216] = (int)piVar1;
      param_1[0x21a] = (uint)bVar5;
      (**(code **)(iVar2 + 0x48))(1);
      piVar1 = (int *)FUN_00482fb0(0x74);
      if (piVar1 == (int *)0x0) {
        piVar1 = (int *)0x0;
      }
      else {
        iVar2 = param_1[0x217];
        FUN_004bcb00();
        *piVar1 = (int)&PTR_FUN_005a3e7c;
        iVar2 = FUN_004bb950(iVar2,&DAT_00400400);
        if (iVar2 == 0) {
          FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
        }
      }
      iVar2 = *piVar1;
      param_1[0x219] = (int)piVar1;
      (**(code **)(iVar2 + 0x48))(1);
      uVar3 = FUN_004aa850(param_1[1],param_1[2],param_1[3],param_1[4],param_1[0x219]);
      (**(code **)(*param_1 + 0x14))(uVar3);
      iVar2 = param_1[0x16];
      uVar4 = FUN_004aa530(iVar2);
      FUN_004aa500(iVar2,uVar4 | 1);
    }
    if (DAT_0065844c != 0) {
      ReleaseMutex(DAT_006584b4);
      hMutex = DAT_006584b4;
      iVar2 = ReleaseMutex(DAT_006584b4);
      while (iVar2 != 0) {
        iVar2 = ReleaseMutex(hMutex);
      }
      _DAT_00658448 = (char *)0x0;
      _DAT_00658d88 = 0;
    }
    FUN_004546a0();
    param_1[0x26f] = -100000;
    param_1[0x270] = -100000;
    param_1[0x26a] = param_2;
  }
  ExceptionList = local_c;
  return;
}


