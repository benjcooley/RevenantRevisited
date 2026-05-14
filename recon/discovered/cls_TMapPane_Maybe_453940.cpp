// FUN_00453940_TMapPane_CenterOnPos @ 00453940 size=125

void __thiscall FUN_00453940_TMapPane_CenterOnPos(int param_1,undefined4 *param_2,undefined4 param_3,uint param_4)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  
  if (((*(byte *)(param_1 + 0xd8) & 1) != 0) && (*(int *)(param_1 + 0xdc) == DAT_00667fcc)) {
    FUN_00535d80(0);
    iVar3 = FUN_0047ed20();
    if (iVar3 == 3) {
      FUN_0047ecc0();
    }
  }
  uVar1 = *param_2;
  uVar2 = param_2[2];
  *(undefined4 *)(param_1 + 0xe4) = param_2[1];
  *(undefined4 *)(param_1 + 0xe0) = uVar1;
  *(undefined4 *)(param_1 + 0xe8) = uVar2;
  *(undefined4 *)(param_1 + 0xec) = param_3;
  *(uint *)(param_1 + 0xd8) = param_4 & 0xfffffffe | 2;
  return;
}


