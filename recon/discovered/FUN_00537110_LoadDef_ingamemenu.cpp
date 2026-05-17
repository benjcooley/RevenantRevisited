// FUN_00537110_LoadDef_ingamemenu @ 00537110 size=84

int FUN_00537110_LoadDef_ingamemenu(void)

{
  char *pcVar1;
  int iVar2;
  
  pcVar1 = s_mpingamemenu_005e4070;
  if (DAT_0066829c == 0) {
    pcVar1 = s_ingamemenu_005e4080;
  }
  iVar2 = FUN_00435150_DefScreen_Open(s_ingamemenu_005e40a0,pcVar1,0x11,0x7e,0x41,0x18a,0x13c,200,0x50,
                       s_widgets_005e4098,s_ingamemenu_005e408c);
  if (iVar2 != 0) {
    FUN_0049c830();
  }
  return iVar2;
}


