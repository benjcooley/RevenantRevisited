// FUN_0053a6d0_TLogoScreen_Animate @ 0053a6d0 size=196

void __fastcall FUN_0053a6d0_TLogoScreen_Animate(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 extraout_ECX;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined1 uStack_104;
  undefined1 uStack_103;
  undefined1 uStack_102;
  undefined1 uStack_101;
  undefined1 auStack_100 [256];
  
  iVar1 = *(int *)(param_1 + 0x50);
  if (iVar1 != 0) {
    uVar4 = 0;
    uVar3 = 0x10;
    uVar2 = FUN_0046d710(s_MainMenu_005e4430);
    FUN_004bd680(0,0,uVar2,uVar3,uVar4);
  }
  FUN_0048ff00_TScreen_Animate();
  if (iVar1 != 0) {
    uStack_104 = 0xff;
    uStack_101 = 0xff;
    uStack_102 = 0xaf;
    uStack_103 = 0x48;
    FUN_004811b0(auStack_100,0x100,s_Revenant_v_d__02d_005e443c,DAT_005d79dc,DAT_005d79dd);
    uVar4 = 0x10;
    uVar3 = 0x41;
    uVar2 = extraout_ECX;
    FUN_00419dd0_BlitEffect_SetField(&uStack_104);
    FUN_004be2b0_CompositeBuffer(0xf,0x1c2,0x96,0x28,auStack_100,0,DAT_00667cc0,uVar2,uVar3,uVar4);
  }
  *(undefined4 *)(param_1 + 0x50) = 0;
  return;
}


