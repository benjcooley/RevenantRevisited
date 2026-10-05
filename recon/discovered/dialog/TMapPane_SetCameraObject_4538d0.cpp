// FUN_004538d0 @ 004538d0 size=98

void __thiscall FUN_004538d0(int param_1,int param_2,uint param_3)

{
  int iVar1;
  
  if ((((*(byte *)(param_1 + 0xd8) & 1) != 0) && (*(int *)(param_1 + 0xdc) == DAT_00667fcc)) &&
     (param_2 != DAT_00667fcc)) {
    FUN_00535d80(0);
    iVar1 = FUN_0047ed20();
    if (iVar1 == 3) {
      FUN_0047ecc0();
    }
  }
  *(int *)(param_1 + 0xdc) = param_2;
  *(uint *)(param_1 + 0xd8) = param_3 & 0xfffffffd | 1;
  return;
}


