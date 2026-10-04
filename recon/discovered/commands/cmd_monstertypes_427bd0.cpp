// FUN_00427bd0 @ 00427bd0 size=89

undefined4 FUN_00427bd0(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  char acStack_50 [80];
  
  uVar1 = FUN_005171d0(*(undefined4 *)(param_1 + 0x38));
  FUN_0041ee50(s__d_Monster_types_for__s__005cc9f8,uVar1);
  iVar2 = 0;
  do {
    FUN_005171f0(iVar2,acStack_50);
    if (acStack_50[0] != '\0') {
      FUN_0041ee50(&DAT_005cca14,acStack_50);
    }
    iVar2 = iVar2 + 1;
  } while (iVar2 < 5);
  return 0;
}


