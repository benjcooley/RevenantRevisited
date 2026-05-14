// FUN_0041b120 @ 0041b120 size=83

void __fastcall FUN_0041b120(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  
  *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x40;
  iVar1 = FUN_0049b880(param_1 + 0x138);
  if (iVar1 == 0) {
    *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) & 0xffffffbf;
    return;
  }
  uVar7 = 700;
  uVar6 = 0x50;
  uVar5 = 0;
  uVar4 = 0;
  uVar3 = 0x7f;
  uVar2 = FUN_0049c430(param_1 + 0x138);
  FUN_0049b990(uVar2,uVar3,uVar4,uVar5,uVar6,uVar7);
  return;
}


