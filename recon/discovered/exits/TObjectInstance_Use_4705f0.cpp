// TObjectInstance_Use @ 0x004705f0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// USE trigger requests (SCRIPT_ENGINE.md 7)
// FUN_004705f0 @ 004705f0 size=751

undefined4 __thiscall FUN_004705f0(int *param_1,int param_2)

{
  short sVar1;
  short sVar2;
  int *piVar3;
  char cVar4;
  int *piVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  undefined4 *puVar9;
  undefined4 uVar10;
  undefined4 uStack_64;
  undefined4 uStack_60;
  undefined4 uStack_5c;
  undefined4 uStack_28;
  
  if (DAT_0066829c != 0) {
    if (((*(short *)(param_2 + 4) == 0xb) && ((*(uint *)(param_2 + 0x36c) & 2) != 0)) &&
       ((short)param_1[1] != 0x11)) {
      FUN_0051d680();
    }
    if (((short)param_1[1] == 5) || ((short)param_1[1] == 0x11)) {
      if (DAT_00676e5c != '\0') goto LAB_00470682;
    }
    else if (DAT_0067682c == 0) {
      if (DAT_00676e5c == '\0') {
        uStack_5c = (int *)0x47065a;
        cVar4 = FUN_005847c0();
        if (cVar4 != '\0') {
          return 1;
        }
      }
      goto LAB_00470682;
    }
    uStack_5c = (int *)0x470682;
    FUN_00584710();
  }
LAB_00470682:
  piVar5 = (int *)FUN_00452690();
  if (piVar5 == (int *)0x0) {
    if (param_1[0x21] != 0) {
      uStack_60 = *(undefined4 *)param_1[0x13];
      uStack_64 = (undefined1 *)param_1[0xe];
      uStack_5c = (int *)param_2;
      FUN_00492640(7);
    }
    if ((param_2 != 0) && (*(int *)(param_2 + 0x84) != 0)) {
      uStack_64 = (undefined1 *)param_1[0xe];
      uStack_60 = 0;
      uStack_5c = param_1;
      FUN_00492640(7);
    }
  }
  else {
    if (param_1[0x21] != 0) {
      uStack_64 = (undefined1 *)piVar5[0xe];
      uStack_5c = (int *)param_2;
      uStack_60 = 0;
      FUN_00492640(7);
    }
    if (((((piVar5[2] & 0x40000000U) != 0) && (piVar5[0x19] == param_1[0x19])) &&
        ((short)piVar5[1] == (short)param_1[1])) &&
       ((iVar6 = (**(code **)(*param_1 + 0xd4))(), iVar6 != 0 &&
        (iVar6 = (**(code **)(*piVar5 + 0xd4))(), iVar6 != 0)))) {
      iVar6 = (int)*(short *)((int)param_1 + 6) - (int)*(short *)((int)piVar5 + 6);
      if (iVar6 < 1) {
        iVar6 = (int)*(short *)((int)piVar5 + 6) - (int)*(short *)((int)param_1 + 6);
      }
      if (iVar6 == 1) {
        piVar3 = (int *)param_1[0x19];
        if (piVar3 != (int *)0x0) {
          uStack_5c = (int *)0x47078e;
          (**(code **)(*param_1 + 0x60))();
          uStack_5c = (int *)0x470795;
          (**(code **)(*piVar5 + 0x60))();
        }
        uVar7 = DAT_00666970;
        uStack_5c = (int *)0x0;
        uStack_64 = &stack0xffffffb8;
        uStack_60 = DAT_00666970;
        (**(code **)(*param_1 + 8))();
        (**(code **)(*param_1 + 0x40))(param_1[2] | 0x8000);
        (**(code **)(*param_1 + 0x8c))();
        (**(code **)(*param_1 + 0x40))(param_1[2] | 0x1000);
        (**(code **)(*piVar5 + 8))(&uStack_5c,uVar7,0);
        (**(code **)(*piVar5 + 0x40))(piVar5[2] | 0x8000);
        (**(code **)(*piVar5 + 0x8c))();
        (**(code **)(*piVar5 + 0x40))(piVar5[2] | 0x1000);
        iVar6 = param_1[1];
        sVar1 = *(short *)((int)param_1 + 6);
        sVar2 = *(short *)((int)piVar5 + 6);
        puVar9 = &uStack_64;
        for (iVar8 = 0xd; iVar8 != 0; iVar8 = iVar8 + -1) {
          *puVar9 = 0;
          puVar9 = puVar9 + 1;
        }
        if (sVar2 < sVar1) {
          sVar2 = sVar1;
        }
        uStack_64 = (undefined1 *)CONCAT22(sVar2 + 1,(short)iVar6);
        uVar10 = 0;
        uStack_5c = (int *)CONCAT22((undefined2)DAT_00666970,(undefined2)uStack_5c);
        uStack_60 = 0;
        uVar7 = FUN_00450e40(&uStack_64,0xffffffff);
        piVar5 = (int *)FUN_00452690(uVar7,uVar10);
        if ((piVar5 != (int *)0x0) && (piVar3 != (int *)0x0)) {
          (**(code **)(*piVar3 + 0x50))(*(undefined4 *)(piVar5[0x13] + 0x1c),uStack_28,0xffffffff);
          (**(code **)(*piVar5 + 0x40))(piVar5[2] | 0x1000);
        }
        return 1;
      }
    }
  }
  return 0;
}


