// FUN_004f0fb0 @ 004f0fb0 size=66

void __thiscall FUN_004f0fb0(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  FUN_00472430(param_2,param_3,param_4);
  puVar1 = *(undefined4 **)(param_2 + 4);
  uVar2 = puVar1[1];
  *(undefined4 *)(param_1 + 0x184) = *puVar1;
  uVar3 = puVar1[2];
  *(undefined4 *)(param_1 + 0x188) = uVar2;
  *(undefined4 **)(param_2 + 4) = puVar1 + 3;
  *(undefined4 *)(param_1 + 0x18c) = uVar3;
  return;
}


