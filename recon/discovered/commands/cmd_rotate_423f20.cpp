// FUN_00423f20 @ 00423f20 size=165

undefined4 FUN_00423f20(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  char acStack_8 [4];
  char acStack_4 [4];
  
  uVar1 = param_2;
  iVar2 = FUN_00479700(&PTR_DAT_005cb8fc,0);
  if (iVar2 != 0) {
    FUN_00479580();
  }
  iVar3 = FUN_0047a410(uVar1,s__i__i__i_005cb900,&param_2,acStack_8,acStack_4);
  if (iVar3 == 0) {
    return 4;
  }
  if (iVar2 != 0) {
    *(char *)(param_1 + 0x34) = *(char *)(param_1 + 0x34) + (char)param_2;
    *(char *)(param_1 + 0x35) = *(char *)(param_1 + 0x35) + acStack_8[0];
    *(char *)(param_1 + 0x36) = *(char *)(param_1 + 0x36) + acStack_4[0];
    return 0;
  }
  *(char *)(param_1 + 0x34) = (char)param_2;
  *(char *)(param_1 + 0x35) = acStack_8[0];
  *(char *)(param_1 + 0x36) = acStack_4[0];
  return 0;
}


