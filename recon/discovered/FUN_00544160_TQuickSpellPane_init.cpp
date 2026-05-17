// FUN_00544160_TQuickSpellPane_Initialize @ 00544160 size=819

undefined4 FUN_00544160_TQuickSpellPane_Initialize(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_005a1ef5;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  FUN_00434e40_TButtonPane_Initialize();
  iVar1 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
  iVar1 = (-(uint)(iVar1 != 0x10) & 0xffff8400) + 0xf81f;
  puVar2 = (undefined4 *)FUN_00482fb0(0x150);
  uStack_4 = 0;
  if (puVar2 == (undefined4 *)0x0) {
    puVar2 = (undefined4 *)0x0;
  }
  else {
    uVar3 = FUN_0046d710(s_RingU_005e51f8);
    uVar4 = FUN_0046d710(s_RingD_005e5200);
    uVar5 = FUN_0046d710(s_RingG_005e51f0);
    FUN_00546a70(&DAT_005e5208,DAT_005e4fe8,DAT_005e4ff8,0x20,0x20,0,&LAB_005440a0,uVar4,uVar3,0,0,1
                 ,1,5,iVar1,0,uVar5);
    *puVar2 = &PTR_FUN_005b9c54;
  }
  uStack_4 = 0xffffffff;
  FUN_00436ae0(puVar2);
  puVar2 = (undefined4 *)FUN_00482fb0(0x150);
  uStack_4 = 1;
  if (puVar2 == (undefined4 *)0x0) {
    puVar2 = (undefined4 *)0x0;
  }
  else {
    uVar3 = FUN_0046d710(s_RingU_005e5214);
    uVar4 = FUN_0046d710(s_RingD_005e521c);
    uVar5 = FUN_0046d710(s_RingG_005e520c);
    FUN_00546a70(&DAT_005e5224,DAT_005e4fec,DAT_005e4ffc,0x20,0x20,0,&LAB_005440d0,uVar4,uVar3,0,0,1
                 ,2,5,iVar1,0,uVar5);
    *puVar2 = &PTR_FUN_005b9c54;
  }
  uStack_4 = 0xffffffff;
  FUN_00436ae0(puVar2);
  puVar2 = (undefined4 *)FUN_00482fb0(0x150);
  uStack_4 = 2;
  if (puVar2 == (undefined4 *)0x0) {
    puVar2 = (undefined4 *)0x0;
  }
  else {
    uVar3 = FUN_0046d710(s_RingU_005e5230);
    uVar4 = FUN_0046d710(s_RingD_005e5238);
    uVar5 = FUN_0046d710(s_RingG_005e5228);
    FUN_0042c2d0(&DAT_005e5240,DAT_005e4ff0,DAT_005e5000,0x20,0x20,0,&LAB_00544100,uVar4,uVar3,0,0,1
                 ,0xffffffff,0,iVar1,0,uVar5);
    puVar2[0x52] = 3;
    puVar2[0x53] = 5;
    *puVar2 = &PTR_FUN_005b9c54;
  }
  uStack_4 = 0xffffffff;
  FUN_00436ae0(puVar2);
  puVar2 = (undefined4 *)FUN_00482fb0(0x150);
  uStack_4 = 3;
  if (puVar2 == (undefined4 *)0x0) {
    puVar2 = (undefined4 *)0x0;
  }
  else {
    uVar3 = FUN_0046d710(s_RingU_005e524c);
    uVar4 = FUN_0046d710(s_RingD_005e5254);
    uVar5 = FUN_0046d710(s_RingG_005e5244);
    FUN_0042c2d0(&DAT_005e525c,DAT_005e4ff4,DAT_005e5004,0x20,0x20,0,&LAB_00544130,uVar4,uVar3,0,0,1
                 ,0xffffffff,0,iVar1,0,uVar5);
    puVar2[0x52] = 4;
    puVar2[0x53] = 5;
    *puVar2 = &PTR_FUN_005b9c54;
  }
  uStack_4 = 0xffffffff;
  FUN_00436ae0(puVar2);
  ExceptionList = pvStack_c;
  return 1;
}


