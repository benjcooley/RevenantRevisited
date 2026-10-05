// FUN_0052c720 @ 0052c720 size=82

void __thiscall FUN_0052c720(int param_1,int param_2)

{
  undefined4 *puVar1;
  
  if ((-1 < param_2) &&
     (puVar1 = *(undefined4 **)(*(int *)(param_1 + 0x10) + param_2 * 4), puVar1 != (undefined4 *)0x0
     )) {
    *puVar1 = 0xffffffff;
    if (puVar1[2] != 0) {
      FUN_00482f80(puVar1[2]);
    }
    puVar1[2] = 0;
    puVar1[1] = 0;
    FUN_004830f0(puVar1);
  }
  FUN_0041cb40(param_2);
  return;
}


