// FUN_00428250 @ 00428250 size=134

undefined4 FUN_00428250(int param_1,int param_2)

{
  int iVar1;
  
  if (*(int *)(param_2 + 0x28) == 0) {
    FUN_0041ee50(s_State_must_be_included_005ccadc);
    return 4;
  }
  iVar1 = FUN_00479700(&DAT_005ccaf4,0);
  if (iVar1 != 0) {
    *(uint *)(param_1 + 0x110) = *(uint *)(param_1 + 0x110) & 0xfffffffd;
    return 0;
  }
  iVar1 = FUN_00479700(&PTR_DAT_005ccaf8,0);
  if (iVar1 != 0) {
    *(uint *)(param_1 + 0x110) = *(uint *)(param_1 + 0x110) | 2;
    return 0;
  }
  FUN_0041ee50(s_State_must_be_included_005ccafc);
  return 4;
}


