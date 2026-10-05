// FUN_00535550 @ 00535550 size=85

void __fastcall FUN_00535550(int param_1)

{
  uint uVar1;
  
  uVar1 = 0;
  if (0 < *(int *)(param_1 + 0x17c)) {
    do {
      if (((*(int *)(param_1 + 0x18c) != 0) && (uVar1 < *(uint *)(param_1 + 0x17c))) &&
         (*(int *)(*(int *)(param_1 + 0x18c) + uVar1 * 4) != 0)) {
        FUN_00534b60();
      }
      uVar1 = uVar1 + 1;
    } while ((int)uVar1 < *(int *)(param_1 + 0x17c));
  }
  FUN_00436010();
  return;
}


