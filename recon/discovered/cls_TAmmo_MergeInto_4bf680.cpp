// FUN_004bf680 @ 004bf680 size=165

undefined4 __thiscall FUN_004bf680(int *param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  FUN_0046fee0(param_2);
  if (0xf < *(short *)((int)param_1 + 6)) {
    FUN_00481c10(PTR_s_Too_many_ammo_object_types_in_am_005df108,0);
  }
  iVar1 = (**(code **)(*param_1 + 0x218))();
  if (((iVar1 != 0) && (param_1[0x19] == DAT_0065d674)) && (param_1[0x15] != 0)) {
    iVar1 = FUN_0059a530(param_1[0xe],s_Arrow_005df368);
    if (iVar1 != 0) {
      FUN_004bf990(param_1[0x15],(short)param_1[3],(int)*(short *)((int)param_1 + 6),1);
      return 0;
    }
    uVar2 = (**(code **)(*param_1 + 0x198))();
    FUN_004bf990(param_1[0x15],(short)param_1[3],(int)*(short *)((int)param_1 + 6),uVar2);
  }
  return 0;
}


