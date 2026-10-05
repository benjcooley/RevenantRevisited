// FUN_0050d990 @ 0050d990 size=48

void __thiscall FUN_0050d990(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  
  FUN_004dd3e0(param_2,param_3,param_4);
  uVar1 = **(undefined4 **)(param_2 + 4);
  *(undefined4 **)(param_2 + 4) = *(undefined4 **)(param_2 + 4) + 1;
  *(undefined4 *)(param_1 + 0xe4) = uVar1;
  return;
}


