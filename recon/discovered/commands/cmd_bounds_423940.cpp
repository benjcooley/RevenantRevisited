// FUN_00423940 @ 00423940 size=332

undefined4 FUN_00423940(int param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  undefined4 unaff_ESI;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  uint uVar7;
  undefined1 auStack_10 [4];
  undefined1 auStack_c [4];
  uint uStack_8;
  undefined1 auStack_4 [4];
  
  iVar2 = param_2;
  iVar1 = FUN_0047a410(param_2,s__d__d__d__d_005cb85c,auStack_4,&uStack_8,auStack_c,auStack_10);
  if (iVar1 == 0) {
    return 4;
  }
  param_2 = (uint)*(ushort *)(param_1 + 0xc);
  if (*(int *)(iVar2 + 0x10) == 8) {
    FUN_0047a410(iVar2,&DAT_005cb868,&param_2);
  }
  iVar2 = FUN_0046e8a0();
  if (iVar2 == 0) {
    FUN_0041ee50(s_Sorry__context_has_no_imagery_005cb86c);
    return 0;
  }
  piVar3 = (int *)FUN_0046e8a0();
  uVar7 = param_2;
  (**(code **)(*piVar3 + 0xb8))(param_2,0,0,1);
  piVar3 = (int *)FUN_0046e8a0();
  uVar6 = 0;
  uVar5 = 0;
  uVar4 = 0;
  (**(code **)(*piVar3 + 200))(uStack_8,0,0,0);
  piVar3 = (int *)FUN_0046e8a0();
  (**(code **)(*piVar3 + 0xb8))(unaff_ESI,uVar6,uVar5,1);
  piVar3 = (int *)FUN_0046e8a0();
  (**(code **)(*piVar3 + 200))(uVar7,uVar4,uStack_8,0);
  if (0 < (int)DAT_00656f00) {
    if (DAT_00656f00 < 0xb) {
      iVar2 = 0;
    }
    else {
      iVar2 = *(int *)(DAT_00656f10 + 0x28);
    }
    if (((*(uint *)(iVar2 + 0x14) >> 0x10 & 1) != 0) && (uStack_8 == *(ushort *)(param_1 + 0xc))) {
      FUN_0044e5f0(*(undefined4 *)(param_1 + 0x40));
    }
  }
  return 0;
}


