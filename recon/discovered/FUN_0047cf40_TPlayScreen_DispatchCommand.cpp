// FUN_0047cf40 @ 0047cf40 size=3588

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0047cf40(int param_1,int param_2)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 *puVar5;
  uint uVar6;
  undefined4 uVar7;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uVar2 = DAT_0065a9c4;
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059d472;
  pvStack_c = ExceptionList;
  if ((*(int *)(param_1 + 0x5d4) != 0) && (param_2 != 0xd)) {
    return;
  }
  uVar6 = DAT_00667fcc;
  ExceptionList = &pvStack_c;
  if ((((DAT_00667fcc != 0) && (ExceptionList = &pvStack_c, *(int *)(DAT_00667fcc + 0x84) != 0)) &&
      (ExceptionList = &pvStack_c, iVar3 = FUN_00472e30(s_PAUSE_005d769c), uVar6 = DAT_00667fcc,
      iVar3 == 0)) &&
     ((*(int *)(*(int *)(DAT_00667fcc + 0x84) + 0x1c) == 1 &&
      (FUN_00492490(), uVar6 = DAT_00667fcc, *(int *)(param_1 + 0x5d8) != 0)))) {
    *(undefined4 *)(param_1 + 0x5d8) = 0;
    FUN_0047c580(1);
    if (DAT_00667fcc != 0) {
      FUN_00472db0(&DAT_005d7604,0);
    }
    uVar6 = DAT_00667fcc;
    piVar1 = DAT_0065cb34;
    *(undefined4 *)(param_1 + 0x5dc) = 1;
    if (piVar1 != (int *)0x0) {
      (**(code **)(*piVar1 + 0x2c))();
      uVar6 = DAT_00667fcc;
    }
  }
  if (DAT_00666924 != 0) {
    ExceptionList = pvStack_c;
    return;
  }
  if (((uVar6 != 0) && ((-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) != uVar6)) &&
     (DAT_0065d0d0 != 0)) {
    FUN_004538d0(uVar6,0xc);
    uVar6 = DAT_00667fcc;
  }
  switch(param_2) {
  case 1:
    if (uVar6 != 0) {
      iVar3 = FUN_0045f770(3);
      if (((iVar3 == 0) &&
          ((*(int **)(uVar6 + 0xe0) == (int *)0x0 || (**(int **)(uVar6 + 0xe0) != 0x19)))) ||
         ((*(int **)(DAT_00667fcc + 0xe0) != (int *)0x0 && (**(int **)(DAT_00667fcc + 0xe0) == 0x19)
          ))) {
        if (DAT_0066829c == 0) {
          FUN_0054d390();
        }
        FUN_004d3b90(0,3);
        *(undefined4 *)(DAT_00667fcc + 0xe8) = 1;
      }
      else {
        FUN_004d3fd0();
        *(undefined4 *)(DAT_00667fcc + 0xe8) = 0;
      }
    }
    break;
  case 2:
    if (uVar6 != 0) {
      if ((*(int **)(uVar6 + 0xe0) == (int *)0x0) || (**(int **)(uVar6 + 0xe0) != 0x19)) {
        if (DAT_0066829c == 0) {
          FUN_0054d390();
        }
        FUN_004d3b90(0,0x19);
      }
      else {
        FUN_004d3fd0();
      }
    }
    break;
  case 3:
    if (uVar6 == 0) {
      ExceptionList = pvStack_c;
      return;
    }
    if ((*(int *)(uVar6 + 0xe0) == 0) || (iVar3 = FUN_004dab80(s_sneak_005c618c), iVar3 == 0)) {
      FUN_004cf2e0();
      ExceptionList = pvStack_c;
      return;
    }
    goto LAB_0047d176;
  case 4:
    if (*(int *)(param_1 + 0x5b8) == 0) {
      if (*(int *)(param_1 + 0x6c0) != 3) {
        if (((*(int *)(param_1 + 0x6a0) != 0) && (*(int *)(param_1 + 0x6a4) == 0)) &&
           (*(int *)(param_1 + 0x6a8) == 0)) {
          *(undefined4 *)(param_1 + 0x6a8) = 1;
        }
        if (((*(int *)(param_1 + 0x6ac) != 0) && (*(int *)(param_1 + 0x6b0) == 0)) &&
           (*(int *)(param_1 + 0x6b4) == 0)) {
          *(undefined4 *)(param_1 + 0x6b4) = 1;
        }
        *(undefined4 *)(param_1 + 0x5b8) = 1;
      }
    }
    else {
      if (((*(int *)(param_1 + 0x6ac) != 1) && (*(int *)(param_1 + 0x6b0) == 0)) &&
         (*(int *)(param_1 + 0x6b4) == 0)) {
        *(undefined4 *)(param_1 + 0x6c4) = 2;
        *(undefined4 *)(param_1 + 0x6b0) = 1;
      }
      if (((*(int *)(param_1 + 0x6a0) != 1) && (*(int *)(param_1 + 0x6a4) == 0)) &&
         ((*(int *)(param_1 + 0x6a8) == 0 && (*(int *)(param_1 + 0x6a0) == 0)))) {
        *(undefined4 *)(param_1 + 0x6a4) = 1;
      }
      *(undefined4 *)(param_1 + 0x5b8) = 0;
    }
    break;
  case 5:
    if (*(int *)(param_1 + 0x6ac) == 0) {
      if ((*(int *)(param_1 + 0x6b0) == 0) && (*(int *)(param_1 + 0x6b4) == 0)) {
        *(undefined4 *)(param_1 + 0x6c4) = 2;
        *(undefined4 *)(param_1 + 0x6b0) = 1;
      }
    }
    else if ((*(int *)(param_1 + 0x6b0) == 0) && (*(int *)(param_1 + 0x6b4) == 0)) {
      *(undefined4 *)(param_1 + 0x6b4) = 1;
    }
    break;
  case 6:
    if (*(int *)(param_1 + 0x6a0) == 0) {
      if (((*(int *)(param_1 + 0x6a4) == 0) ||
          ((*(int *)(param_1 + 0x6c0) == 3 && (*(int *)(param_1 + 0x6a4) == 0)))) &&
         (*(int *)(param_1 + 0x6a8) == 0)) {
        *(undefined4 *)(param_1 + 0x6a4) = 1;
      }
    }
    else if ((*(int *)(param_1 + 0x6a4) == 0) && (*(int *)(param_1 + 0x6a8) == 0)) {
      *(undefined4 *)(param_1 + 0x6a8) = 1;
    }
    break;
  case 7:
    FUN_0053cab0(2);
    DAT_0065d1b8 = 2;
    if (DAT_0065d190 == 0) {
      if ((DAT_0065d194 == 0) && (DAT_0065d198 == 0)) {
        DAT_0065d194 = 1;
      }
    }
    else {
      FUN_0048ed90(&DAT_0065a9d8,0xffffffff);
      _DAT_0065aa20 = 0;
      _DAT_0065aa24 = 0;
      (**(code **)(DAT_0065a9d8 + 0x28))();
    }
    FUN_00438a50(0);
    FUN_00438a50(0);
    FUN_00438a50(1);
    FUN_0053cb40(2);
    DAT_0065d1bc = 2;
    if (DAT_0065d190 == 0) {
      if ((DAT_0065d194 == 0) && (DAT_0065d198 == 0)) {
        DAT_0065d194 = 1;
      }
      FUN_00438a50(1);
      uVar4 = 0;
    }
    else {
      FUN_0048ed90(&DAT_006661b0,0xffffffff);
      _DAT_006661f8 = 0;
      _DAT_006661fc = 0;
      (**(code **)(DAT_006661b0 + 0x28))();
      FUN_00438a50(1);
      uVar4 = 0;
    }
    goto LAB_0047d807;
  case 8:
    FUN_0053cab0(1);
    DAT_0065d1b8 = 1;
    if (DAT_0065d190 == 0) {
      if ((DAT_0065d194 == 0) && (DAT_0065d198 == 0)) {
        DAT_0065d194 = 1;
      }
    }
    else {
      FUN_0048ed90(&DAT_0065b140,0xffffffff);
      DAT_0065b188 = 0;
      _DAT_0065b18c = 0;
      (**(code **)(DAT_0065b140 + 0x28))();
    }
    FUN_00438a50(0);
    FUN_00438a50(1);
    FUN_00438a50(0);
    break;
  case 9:
    FUN_0053cab0(0);
    DAT_0065d1b8 = 0;
    if (DAT_0065d190 == 0) {
      if ((DAT_0065d194 == 0) && (DAT_0065d198 == 0)) {
        DAT_0065d194 = 1;
      }
    }
    else {
      FUN_0048ed90(&DAT_0065b7e0,0xffffffff);
      _DAT_0065b828 = 0;
      _DAT_0065b82c = 0;
      (**(code **)(DAT_0065b7e0 + 0x28))();
    }
    FUN_00438a50(1);
    FUN_00438a50(0);
    FUN_00438a50(0);
    FUN_0053cb40(0);
    DAT_0065d1bc = 0;
    if (DAT_0065d190 == 0) {
      if ((DAT_0065d194 == 0) && (DAT_0065d198 == 0)) {
        DAT_0065d194 = 1;
      }
    }
    else {
      FUN_0048ed90(&DAT_0065d4f8,0xffffffff);
      _DAT_0065d540 = 0;
      _DAT_0065d544 = 0;
      (**(code **)(DAT_0065d4f8 + 0x28))();
      (**(code **)(DAT_0065d4f8 + 0x28))();
    }
    goto LAB_0047d791;
  case 10:
    FUN_0053cab0(2);
    DAT_0065d1b8 = 2;
    if (DAT_0065d190 == 0) {
      FUN_0047c500(1);
    }
    else {
      FUN_0048ed90(&DAT_0065a9d8,0xffffffff);
      _DAT_0065aa20 = 0;
      _DAT_0065aa24 = 0;
      (**(code **)(DAT_0065a9d8 + 0x28))();
    }
    FUN_00438a50(0);
    FUN_00438a50(0);
    (**(code **)(*DAT_0065bfec + 0x1c))(DAT_0065bfec[5] | 0x10000);
    FUN_004387f0(1);
    FUN_0053cb40(2);
    DAT_0065d1bc = 2;
    if (DAT_0065d190 == 0) {
      FUN_0047c500(1);
    }
    else {
      FUN_0048ed90(&DAT_006661b0,0xffffffff);
      _DAT_006661f8 = 0;
      _DAT_006661fc = 0;
      (**(code **)(DAT_006661b0 + 0x28))();
    }
    FUN_00438a50(1);
    FUN_00438a50(0);
    (**(code **)(*DAT_0065bff0 + 0x1c))(DAT_0065bff0[5] & 0xfffeffff);
    FUN_004387f0(1);
    break;
  case 0xb:
    FUN_0053cb40(0);
    DAT_0065d1bc = 0;
    if (DAT_0065d190 == 0) {
      FUN_0047c500(1);
    }
    else {
      FUN_0048ed90(&DAT_0065d4f8,0xffffffff);
      _DAT_0065d540 = 0;
      _DAT_0065d544 = 0;
      (**(code **)(DAT_0065d4f8 + 0x28))();
      (**(code **)(DAT_0065d4f8 + 0x28))();
    }
LAB_0047d791:
    FUN_00438a50(0);
    FUN_00438a50(0);
    FUN_00438a50(1);
    break;
  case 0xc:
    FUN_0053cb40(1);
    DAT_0065d1bc = 1;
    if (DAT_0065d190 == 0) {
      FUN_0047c500(1);
    }
    else {
      FUN_0048ed90(&DAT_0065b4f0,0xffffffff);
    }
    FUN_00438a50(0);
    uVar4 = 1;
LAB_0047d807:
    FUN_00438a50(uVar4);
    FUN_00438a50(0);
    break;
  case 0xe:
    if (uVar6 != 0) {
      FUN_004d01e0();
    }
    break;
  case 0xf:
    if (uVar6 != 0) {
      FUN_004d0430();
    }
    break;
  case 0x10:
    if (uVar6 != 0) {
      FUN_004ceff0();
    }
    break;
  case 0x11:
  case 0x12:
  case 0x13:
    if (uVar6 != 0) {
      FUN_004d27f0(param_2 + -0x10);
    }
    break;
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
    FUN_00544630(param_2 + -0x13);
    break;
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
    if (uVar6 != 0) {
      FUN_0051e420(param_2 + -0x18);
    }
    break;
  case 0x21:
    if (uVar6 != 0) {
      FUN_004d2480(1);
    }
    break;
  case 0x22:
    if (uVar6 != 0) {
      FUN_004d2480(2);
    }
    break;
  case 0x23:
    if (uVar6 != 0) {
      FUN_004d2480(3);
    }
    break;
  case 0x24:
  case 0x25:
  case 0x26:
  case 0x27:
  case 0x28:
  case 0x29:
  case 0x2a:
  case 0x2b:
  case 0x2c:
  case 0x2d:
  case 0x2e:
  case 0x2f:
    if (uVar6 != 0) {
      FUN_004d2480(param_2 + -0x20);
    }
    break;
  case 0x30:
  case 0x31:
  case 0x32:
  case 0x33:
  case 0x34:
  case 0x35:
  case 0x36:
  case 0x37:
    if (uVar6 != 0) {
      FUN_004d3150(param_2 + -0x30);
    }
    break;
  case 0x38:
  case 0x39:
  case 0x3a:
    if (uVar6 != 0) {
      FUN_004d27f0(param_2 + -0x37);
    }
    break;
  case 0x3b:
    if (uVar6 != 0) {
      FUN_004d0aa0();
    }
    break;
  case 0x3c:
    if (uVar6 != 0) {
      iVar3 = FUN_0045f770(3);
      uVar2 = DAT_00667fcc;
      if (((iVar3 == 0) &&
          ((*(int **)(uVar6 + 0xe0) == (int *)0x0 || (**(int **)(uVar6 + 0xe0) != 0x19)))) ||
         (*(int *)(*(int *)(uVar6 + 0xe0) + 0x44) == 0)) {
        FUN_004d0fd0(*(undefined1 *)(DAT_00667fcc + 0x36));
      }
      else {
        iVar3 = FUN_0045f770(3);
        if ((iVar3 == 0) &&
           ((piVar1 = *(int **)(uVar2 + 0xe0), piVar1 == (int *)0x0 || (*piVar1 != 0x19)))) {
          FUN_004d1000(0);
        }
        else {
          FUN_004d1000(*(undefined4 *)(*(int *)(uVar2 + 0xe0) + 0x44));
        }
      }
    }
    break;
  case 0x3d:
  case 0x3e:
  case 0x3f:
    if (uVar6 != 0) {
      FUN_004d27f0(param_2 + -0x3c);
    }
    break;
  case 0x40:
  case 0x41:
  case 0x42:
    if (uVar6 != 0) {
      FUN_004d2480(param_2 + -0x3f);
    }
    break;
  case 0x43:
  case 0x44:
  case 0x45:
    if (uVar6 != 0) {
      FUN_004d27f0(param_2 + -0x42);
    }
    break;
  case 0x4a:
  case 0x4b:
    if (uVar6 == 0) {
      ExceptionList = pvStack_c;
      return;
    }
    if ((uVar2 & 0x100) != 0) {
      FUN_004cf490();
      ExceptionList = pvStack_c;
      return;
    }
LAB_0047d176:
    FUN_004cf000();
    break;
  case 0x50:
    if ((DAT_0066829c != 0) && (DAT_0065db48 == 0)) {
      FUN_0047c580(0);
      if (DAT_00667fcc != 0) {
        FUN_0044f140(5,0,0);
        FUN_004cee70(0);
      }
      FUN_00463e00();
      FUN_0048ed90(&DAT_0065db08,0xffffffff);
      uVar7 = 7;
      uVar4 = FUN_0048ed60(&DAT_0065db08);
      FUN_0048eea0(uVar4,uVar7);
    }
    break;
  case 0x51:
    if ((DAT_0066829c != 0) || (DAT_00668130 != 0)) {
      FUN_0047e440(uVar6,DAT_0066829c == 0,0xffffffff);
    }
    break;
  case 0x52:
    _DAT_0066fe3c = 1;
    FUN_0053a8b0();
    FUN_0048f040(&DAT_0066fcc0,(-(uint)(DAT_0066829c != 0) & 0xfffffff8) + 8 | 7);
    FUN_0053aa60();
    _DAT_0065cb40 = 1;
    break;
  case 0x53:
    _DAT_0066fa68 = 1;
    FUN_00539380();
    FUN_0048f040(&DAT_0066f8d0,(-(uint)(DAT_0066829c != 0) & 0xfffffff8) + 8 | 7);
    FUN_00539440();
    _DAT_0065cb40 = 1;
    break;
  case 0x54:
    iVar3 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))(0);
    uVar4 = FUN_004a1ec0(0x280,0x1e0,(-(iVar3 != 0xf) & 2U) + 2);
    puVar5 = (undefined4 *)FUN_00482fb0(0x6c);
    uStack_4 = 0;
    if (puVar5 == (undefined4 *)0x0) {
      puVar5 = (undefined4 *)0x0;
    }
    else {
      FUN_004bcb00();
      uStack_4 = CONCAT31(uStack_4._1_3_,1);
      *puVar5 = &PTR_FUN_005a3ee4;
      FUN_004a3a40(uVar4);
    }
    uStack_4 = 0xffffffff;
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    FUN_00438df0(0,0,PTR_DAT_005d79e0,0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4),
                 *(undefined4 *)(PTR_DAT_005d79e0 + 8),0x80000000,0,0);
    FUN_004a2960(s_ss_bmp_005d76a4,3);
    if (puVar5 != (undefined4 *)0x0) {
      (**(code **)*puVar5)(1);
    }
    FUN_004830f0(uVar4);
    FUN_005399f0();
    FUN_0048f040(&DAT_0066fb08,(-(uint)(DAT_0066829c != 0) & 0xfffffff8) + 8 | 7);
    FUN_00539ab0();
    _DAT_0065cb40 = 1;
    break;
  case 0x55:
    iVar3 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))(0);
    uVar4 = FUN_004a1ec0(0x280,0x1e0,(-(iVar3 != 0xf) & 2U) + 2);
    puVar5 = (undefined4 *)FUN_00482fb0(0x6c);
    uStack_4 = 2;
    if (puVar5 == (undefined4 *)0x0) {
      puVar5 = (undefined4 *)0x0;
    }
    else {
      FUN_004bcb00();
      uStack_4 = CONCAT31(uStack_4._1_3_,3);
      *puVar5 = &PTR_FUN_005a3ee4;
      FUN_004a3a40(uVar4);
    }
    uStack_4 = 0xffffffff;
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    FUN_00438df0(0,0,PTR_DAT_005d79e0,0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4),
                 *(undefined4 *)(PTR_DAT_005d79e0 + 8),0x80000000,0,0);
    FUN_004a2960(s_ss_bmp_005d76ac,3);
    if (puVar5 != (undefined4 *)0x0) {
      (**(code **)*puVar5)(1);
    }
    FUN_004830f0(uVar4);
    FUN_0047e850();
  }
  ExceptionList = pvStack_c;
  return;
}


