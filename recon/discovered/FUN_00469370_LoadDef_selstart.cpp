// FUN_00469370_LoadDef_selstart @ 00469370 size=90

undefined4 __fastcall FUN_00469370_LoadDef_selstart(int param_1)

{
  int iVar1;
  
  iVar1 = FUN_00435150_DefScreen_Open(s_selstart_005d3bc4,&DAT_00659cb8,0x11,0x7e,0x41,0x18a,0x13c,0x15c,0x54,
                       s_widgets_005d3bbc,s_selstart_005d3bb0);
  if (iVar1 == 0) {
    return 0;
  }
  *(undefined4 *)(param_1 + 0x180) = 0;
  *(undefined4 *)(param_1 + 0x17c) = 0;
  *(undefined4 *)(param_1 + 0x184) = 0;
  *(undefined4 *)(param_1 + 0x188) = 0;
  return 1;
}


