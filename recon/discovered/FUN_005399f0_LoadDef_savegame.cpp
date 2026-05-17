// FUN_005399f0_LoadDef_savegame @ 005399f0 size=181

undefined4 __fastcall FUN_005399f0_LoadDef_savegame(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 extraout_ECX;
  undefined4 uVar3;
  undefined4 uVar4;
  
  FUN_0048d260(DAT_0066829c);
  iVar1 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))(0);
  uVar2 = FUN_004a1ec0(0xd8,0xa0,(-(iVar1 != 0xf) & 2U) + 2);
  *(undefined4 *)(param_1 + 0x19c) = uVar2;
  uVar4 = 0xffff;
  uVar3 = 0x80000000;
  uVar2 = extraout_ECX;
  FUN_00444e20(0);
  FUN_004a22f0(uVar2,uVar3,uVar4);
  *(undefined4 *)(param_1 + 0x1a0) = 0;
  DAT_0066fb04 = 0xffffffff;
  FUN_00435150_DefScreen_Open(s_savegame_005e4330,s_default_005e4328,0x11,0,0,0x280,0x1e0,0x1c2,0xa0,
               s_widgets_005e4320,s_savegame_005e4314);
  return 1;
}


