// FUN_0048e5b0 @ 0048e5b0 size=40

void __thiscall FUN_0048e5b0(uint *param_1,uint param_2,undefined4 param_3)

{
  undefined4 uVar1;
  
  uVar1 = 0;
  if ((0 < (int)*param_1) && (param_2 < *param_1)) {
    uVar1 = **(undefined4 **)(param_1[4] + param_2 * 4);
  }
  FUN_0048df70(uVar1,param_3);
  return;
}


