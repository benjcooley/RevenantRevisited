// FUN_0054cb00 @ 0054cb00 size=166

void __fastcall FUN_0054cb00(int param_1)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  
  iVar5 = *(int *)(param_1 + 0x9c);
  iVar2 = *(int *)(param_1 + 0x98);
  iVar4 = iVar5 - iVar2;
  if (iVar4 < 1) {
    iVar4 = iVar2 - iVar5;
  }
  if (iVar4 < 5) {
    *(int *)(param_1 + 0x98) = iVar5;
  }
  else {
    if (iVar2 < iVar5) {
      iVar2 = iVar2 + 4;
    }
    else {
      if (iVar2 <= iVar5) goto LAB_0054cb3d;
      iVar2 = iVar2 + -4;
    }
    *(int *)(param_1 + 0x98) = iVar2;
  }
LAB_0054cb3d:
  iVar5 = (*(int *)(param_1 + 0x98) * 0x9b) / 0xb0;
  if (iVar5 < 0x11) {
    iVar5 = 0;
  }
  else {
    iVar5 = iVar5 + -0x10;
  }
  uVar1 = *(int *)(param_1 + 0x98) - 0xba;
  uVar6 = 0x100;
  uVar3 = FUN_0046d710(s_texthealthbar_005e581c);
  FUN_004bd8c0((0 < (int)uVar1) - 1 & uVar1,1,uVar3,uVar6,iVar5);
  return;
}


