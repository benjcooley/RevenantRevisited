// FUN_0052ca70 @ 0052ca70 size=769

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0052ca70(int *param_1,undefined *param_2,undefined4 param_3)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  undefined2 extraout_var;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 unaff_ESI;
  int iVar6;
  int iVar7;
  undefined4 *puVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined1 *puVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  undefined4 local_5c;
  undefined4 local_58;
  int *local_54;
  undefined1 auStack_50 [80];
  
  if (((param_2 != PTR_DAT_005d79e0) || (param_1[0x14] != 0)) || (DAT_0065b688 != 0)) {
    iVar6 = 0;
    iVar7 = (DAT_0065b644 + -0xdc) / 0x2d;
    param_1[0x23] = iVar7;
    if (0 < iVar7) {
      iVar7 = 0xdc;
      do {
        FUN_004bd680(iVar7,10,param_1[0x21],param_3,0);
        iVar6 = iVar6 + 1;
        iVar7 = iVar7 + 0x2d;
      } while (iVar6 < param_1[0x23]);
    }
    if (param_1[0x18] == 0) {
      return;
    }
    iVar7 = FUN_004a1ec0(0x14,0x14,*(uint *)(PTR_DAT_005d79e0 + 0x38) & 0x3001f,0);
    *(undefined4 *)(iVar7 + 0x18) = _DAT_006668d0;
    local_5c = 0;
    local_58 = 0;
    local_54 = (int *)0x0;
    FUN_0046dfb0();
    piVar1 = local_54;
    while (local_54 = piVar1, piVar1 != (int *)0x0) {
      if ((param_1[0x1b] == 0) || ((short)piVar1[0x1f] + -0x10b != param_1[0x19])) {
        iVar6 = (short)piVar1[0x1f] + -0x10b;
        if ((param_1[0x22] <= iVar6) && (iVar6 < param_1[0x23] + param_1[0x22])) {
          piVar2 = (int *)FUN_0046e8a0();
          iVar6 = (**(code **)(*piVar2 + 0xd8))((short)piVar1[3]);
          if ((iVar6 == 0) || (param_2 != PTR_DAT_005d79e0)) {
            iVar5 = ((int)(short)piVar1[0x1f] - param_1[0x22]) * 0x2d;
            iVar6 = iVar5 + -0x2e13;
            (**(code **)(*piVar1 + 0x108))(iVar6,10,param_2);
            iVar3 = FUN_0059a530(piVar1[0xe],s_Pouch_005e37c4);
            if ((iVar3 == 0) &&
               ((piVar2 = (int *)FUN_004701f0(0), piVar2 != (int *)0x0 &&
                (iVar3 = (**(code **)(*piVar2 + 0x130))(), iVar3 != 0)))) {
              FUN_004a31a0(iVar3,unaff_ESI,CONCAT22(extraout_var,DAT_006668d0));
              FUN_004bd680(iVar6,0x1e,unaff_ESI,0x100,0);
              uVar4 = FUN_00470040(&local_5c,10);
              FUN_0058d252(uVar4);
              uVar13 = 0x20;
              uVar12 = 0x400;
              uVar4 = FUN_00429950(0xff,0xff,0xff);
              uVar10 = extraout_ECX;
              FUN_00419dd0(uVar4);
              puVar8 = &local_5c;
              uVar9 = 0;
              uVar4 = DAT_0065abc4;
              iVar3 = FUN_0052d870(DAT_0065abc4);
              FUN_004be2b0(iVar5 + -0x2dff,0x24,0x14,iVar3 + 2,puVar8,uVar9,uVar4,uVar10,uVar12,
                           uVar13);
            }
            iVar5 = (**(code **)(*piVar1 + 0x198))();
            if (1 < iVar5) {
              uVar4 = (**(code **)(*piVar1 + 0x198))(auStack_50,10);
              FUN_0058d252(uVar4);
              uVar13 = 0x10;
              uVar12 = 0x404;
              uVar10 = extraout_ECX_00;
              FUN_00444e20(0xffffffff);
              puVar11 = auStack_50;
              uVar9 = 0;
              uVar4 = DAT_0065a9cc;
              iVar5 = FUN_0052d870(DAT_0065a9cc);
              FUN_004be2b0(iVar6,10,0x28,iVar5 << 1,puVar11,uVar9,uVar4,uVar10,uVar12,uVar13);
            }
          }
        }
      }
      FUN_0046dfb0();
      piVar1 = local_54;
    }
    if (param_1[0x19] < 0) {
      param_1[0x1a] = -1;
    }
    FUN_004830f0(iVar7);
  }
  (**(code **)(*param_1 + 0x2c))(0);
  return;
}


