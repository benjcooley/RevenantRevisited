// FUN_0054ab80 @ 0054ab80 size=645

void __fastcall FUN_0054ab80(int param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  undefined4 uVar14;
  undefined4 uVar15;
  
  if ((DAT_006680c8 == 0) && (DAT_00667fcc != (int *)0x0)) {
    piVar3 = (int *)DAT_00667fcc[0x38];
    if ((piVar3 == (int *)0x0) || ((*piVar3 != 3 && ((piVar3 == (int *)0x0 || (*piVar3 != 0x19))))))
    {
      piVar3 = (int *)0x0;
    }
    else {
      piVar3 = (int *)piVar3[0x11];
    }
    if (*(int *)(param_1 + 0xd4) != 0) {
      FUN_00414d70(4,4,1,*(undefined4 *)(param_1 + 0x70),0,0x80,0x40,
                   ((*(int *)(param_1 + 0xd4) * 0xff) / 6) * 0x1000000 | 0xffffff,0,0,0x80,0x40,0,4)
      ;
      uVar15 = 1;
      uVar14 = 6;
      uVar13 = 0x77;
      uVar12 = 0x11;
      uVar11 = 0x7d;
      uVar10 = 0x31;
      uVar9 = 2;
      uVar8 = 1;
      uVar7 = 2;
      uVar6 = 0xf;
      uVar5 = 0x44;
      uVar1 = (**(code **)(*DAT_00667fcc + 0x1d8))(0x44,0xf,2,1,2,0x31,0x7d,0x11,0x77,6,1);
      uVar2 = (**(code **)(*DAT_00667fcc + 0x1c0))(uVar1);
      FUN_0054a5d0(uVar2,uVar1,uVar5,uVar6,uVar7,uVar8,uVar9,uVar10,uVar11,uVar12,uVar13,uVar14,
                   uVar15);
      uVar15 = 1;
      uVar14 = 4;
      uVar13 = 0x45;
      uVar12 = 0xc;
      uVar11 = 0x4d;
      uVar10 = 0x42;
      uVar9 = 2;
      uVar8 = 0x13;
      uVar7 = 2;
      uVar6 = 0x1f;
      uVar5 = 0x44;
      uVar1 = (**(code **)(*DAT_00667fcc + 0x1e8))(0x44,0x1f,2,0x13,2,0x42,0x4d,0xc,0x45,4,1);
      uVar2 = (**(code **)(*DAT_00667fcc + 0x1d0))(uVar1);
      FUN_0054a5d0(uVar2,uVar1,uVar5,uVar6,uVar7,uVar8,uVar9,uVar10,uVar11,uVar12,uVar13,uVar14,
                   uVar15);
      uVar15 = 1;
      uVar14 = 4;
      uVar13 = 0x2d;
      uVar12 = 0xc;
      uVar11 = 0x35;
      uVar10 = 0x51;
      uVar9 = 2;
      uVar8 = 0x22;
      uVar7 = 2;
      uVar6 = 0x2c;
      uVar5 = 0x44;
      uVar1 = (**(code **)(*DAT_00667fcc + 0x1e0))(0x44,0x2c,2,0x22,2,0x51,0x35,0xc,0x2d,4,1);
      uVar2 = (**(code **)(*DAT_00667fcc + 0x1c8))(uVar1);
      FUN_0054a5d0(uVar2,uVar1,uVar5,uVar6,uVar7,uVar8,uVar9,uVar10,uVar11,uVar12,uVar13,uVar14,
                   uVar15);
    }
    if ((piVar3 == (int *)0x0) &&
       (piVar3 = (int *)FUN_00452690_TMapPane_GetInstance(*(undefined4 *)(param_1 + 0xb8),0), piVar3 == (int *)0x0)) {
      return;
    }
    if (*(int *)(param_1 + 0xdc) != 0) {
      FUN_00414d70(*(int *)(param_1 + 0xc) + -0x88,4,1,*(undefined4 *)(param_1 + 0x74),0,0x80,0x40,
                   ((*(int *)(param_1 + 0xdc) * 0xff) / 6) * 0x1000000 | 0xffffff,0,0,0x80,0x40,0,4)
      ;
      uVar14 = 0xffffffff;
      uVar13 = 6;
      uVar12 = 0x77;
      uVar11 = 0x11;
      uVar10 = 0x7d;
      uVar9 = 0x31;
      uVar8 = 2;
      uVar7 = 1;
      uVar6 = 2;
      iVar4 = *(int *)(param_1 + 0xc) + -0xc1;
      uVar5 = 0xf;
      uVar1 = (**(code **)(*piVar3 + 0x1d8))(iVar4,0xf,2,1,2,0x31,0x7d,0x11,0x77,6,0xffffffff);
      uVar2 = (**(code **)(*piVar3 + 0x1c0))(uVar1);
      FUN_0054a5d0(uVar2,uVar1,iVar4,uVar5,uVar6,uVar7,uVar8,uVar9,uVar10,uVar11,uVar12,uVar13,
                   uVar14);
      uVar14 = 0xffffffff;
      uVar13 = 4;
      uVar12 = 0x45;
      uVar11 = 0xc;
      uVar10 = 0x4d;
      uVar9 = 0x42;
      uVar8 = 2;
      uVar7 = 0x13;
      uVar6 = 2;
      iVar4 = *(int *)(param_1 + 0xc) + -0x91;
      uVar5 = 0x1f;
      uVar1 = (**(code **)(*piVar3 + 0x1e8))(iVar4,0x1f,2,0x13,2,0x42,0x4d,0xc,0x45,4,0xffffffff);
      uVar2 = (**(code **)(*piVar3 + 0x1d0))(uVar1);
      FUN_0054a5d0(uVar2,uVar1,iVar4,uVar5,uVar6,uVar7,uVar8,uVar9,uVar10,uVar11,uVar12,uVar13,
                   uVar14);
      uVar14 = 0xffffffff;
      uVar13 = 4;
      uVar12 = 0x2d;
      uVar11 = 0xc;
      uVar10 = 0x35;
      uVar9 = 0x51;
      uVar8 = 2;
      uVar7 = 0x22;
      uVar6 = 2;
      iVar4 = *(int *)(param_1 + 0xc) + -0x79;
      uVar5 = 0x2c;
      uVar1 = (**(code **)(*piVar3 + 0x1e0))(iVar4,0x2c,2,0x22,2,0x51,0x35,0xc,0x2d,4,0xffffffff);
      uVar2 = (**(code **)(*piVar3 + 0x1c8))(uVar1);
      FUN_0054a5d0(uVar2,uVar1,iVar4,uVar5,uVar6,uVar7,uVar8,uVar9,uVar10,uVar11,uVar12,uVar13,
                   uVar14);
    }
  }
  return;
}


