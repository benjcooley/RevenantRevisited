// FUN_00528e70 @ 00528e70 size=48

void __thiscall FUN_00528e70(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  
  FUN_00472430(param_2,param_3,param_4);
  uVar1 = **(undefined4 **)(param_2 + 4);
  *(undefined4 **)(param_2 + 4) = *(undefined4 **)(param_2 + 4) + 1;
  *(undefined4 *)(param_1 + 0xd8) = uVar1;
  return;
}


