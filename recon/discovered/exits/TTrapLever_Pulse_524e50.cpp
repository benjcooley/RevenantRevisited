// TTrapLever_Pulse @ 0x00524e50 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x110
// FUN_00524e50 @ 00524e50 size=359

void __fastcall FUN_00524e50(int *param_1)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  int *piStack_40;
  
  piVar3 = (int *)0x0;
  iVar1 = param_1[0x38];
  if (((iVar1 != 0) && (param_1[0x36] != 0)) &&
     ((iVar1 == 0 || ((*(int **)(iVar1 + 0xd8) == (int *)0x0 || (**(int **)(iVar1 + 0xd8) != 2))))))
  {
    iVar1 = (**(code **)(*param_1 + 0x228))();
    if (iVar1 == 0) {
      iVar1 = (**(code **)(*param_1 + 0x230))();
      if (iVar1 != 0) {
        if (((short)param_1[3] == 1) || ((short)param_1[3] == 3)) {
          (**(code **)(*param_1 + 0x18))(2);
        }
        else {
          (**(code **)(*param_1 + 0x18))(3);
        }
        if (param_1[0x21] != 0) {
          FUN_00492640(7,param_1[0xe],*(undefined4 *)param_1[0x13],0,0,0,0);
          FUN_00471260();
          param_1[0x36] = 0;
          return;
        }
        uVar2 = 0xffff;
        FUN_0044cf80(0,0,4,0,0xffffffff);
        if (piStack_40 != (int *)0x0) {
          do {
            iVar1 = FUN_0059a530(*(undefined4 *)(piStack_40[0x12] + 4),&DAT_005e2fa8);
            if (iVar1 == 0) {
              iVar1 = (**(code **)(*piStack_40 + 0x210))();
              if (iVar1 == 0) {
                iVar1 = (**(code **)(*piStack_40 + 0x230))();
                if (iVar1 != 0) {
                  iVar1 = (**(code **)(*param_1 + 4))(piStack_40);
                  if (iVar1 < (int)(uVar2 & 0xffff)) {
                    uVar2 = (**(code **)(*param_1 + 4))(piStack_40);
                    piVar3 = piStack_40;
                  }
                }
              }
            }
            FUN_0044d080();
          } while (piStack_40 != (int *)0x0);
          if (piVar3 != (int *)0x0) {
            (**(code **)(*piVar3 + 0xbc))(piVar3,0xffffffff);
          }
        }
        param_1[0x36] = 0;
      }
    }
  }
  return;
}


