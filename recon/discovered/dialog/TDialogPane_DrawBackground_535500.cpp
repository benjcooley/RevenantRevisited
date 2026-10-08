// FUN_00535500 @ 00535500 size=76

void __fastcall FUN_00535500(int *param_1)

{
  int iVar1;
  
  if (param_1[0x14] != 0) {
    iVar1 = 0;
    if (0 < param_1[0x5f]) {
      do {
        FUN_00534470();
        iVar1 = iVar1 + 1;
      } while (iVar1 < param_1[0x5f]);
    }
    FUN_00435de0();
    (**(code **)(*param_1 + 0x2c))(0);
    return;
  }
  FUN_00435de0();
  return;
}


