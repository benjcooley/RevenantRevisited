// TTrapPressPlate_Pulse @ 0x005251e0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x110
// FUN_005251e0 @ 005251e0 size=434

void __fastcall FUN_005251e0(int *param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  int *piStack_40;
  
  iVar1 = (**(code **)(*param_1 + 0x230))();
  if (iVar1 != 0) {
    iVar1 = (**(code **)(*param_1 + 0x228))();
    if (iVar1 == 0) {
      (**(code **)(*param_1 + 500))();
    }
  }
  iVar1 = param_1[0x38];
  piVar4 = (int *)0x0;
  if ((((iVar1 != 0) && (param_1[0x36] != 0)) &&
      ((iVar1 == 0 || ((*(int **)(iVar1 + 0xd8) == (int *)0x0 || (**(int **)(iVar1 + 0xd8) != 2)))))
      ) && ((short)param_1[3] == 0)) {
    (**(code **)(*param_1 + 0x18))(1);
    if (param_1[0x21] != 0) {
      FUN_00492640(7,param_1[0xe],*(undefined4 *)param_1[0x13],0,0,0,0);
      FUN_00471260();
      param_1[0x36] = 0;
      return;
    }
    uVar3 = 0xffff;
    FUN_0044cf80(0,0,4,0,0xffffffff);
    if (piStack_40 != (int *)0x0) {
      do {
        iVar1 = FUN_0059a530(*(undefined4 *)(piStack_40[0x12] + 4),&DAT_005e2fb0);
        if (iVar1 == 0) {
          iVar1 = (**(code **)(*piStack_40 + 0x210))();
          if (iVar1 == 0) {
            iVar1 = (**(code **)(*piStack_40 + 0x230))();
            if (iVar1 != 0) {
              iVar1 = (**(code **)(*param_1 + 4))(piStack_40);
              if (iVar1 < (int)(uVar3 & 0xffff)) {
                uVar3 = (**(code **)(*param_1 + 4))(piStack_40);
                piVar4 = piStack_40;
              }
            }
          }
        }
        FUN_0044d080();
      } while (piStack_40 != (int *)0x0);
      if (piVar4 != (int *)0x0) {
        (**(code **)(*piVar4 + 0xbc))(piVar4,0xffffffff);
      }
    }
    param_1[0x36] = 0;
    iVar1 = FUN_0049c430(s_pressplate_005e2fb8);
    if (-1 < iVar1) {
      iVar2 = FUN_0049b650(iVar1);
      if (iVar2 != 0) {
        FUN_0049b990(iVar1,0x7f - (uVar3 & 0xffff) / 0xff,1,0,0x50,700);
      }
    }
  }
  return;
}


