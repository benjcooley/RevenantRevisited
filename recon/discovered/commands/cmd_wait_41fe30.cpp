// FUN_0041fe30 @ 0041fe30 size=544

undefined4 FUN_0041fe30(undefined4 param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  if (*(int *)(param_2 + 0x10) == 8) {
    FUN_004712d0(*(undefined4 *)(param_2 + 0x14));
    FUN_00479580();
    return 1;
  }
  iVar1 = FUN_00479700(s_screenfade_005cadbc,0);
  if (iVar1 == 0) {
    iVar1 = FUN_00479700(s_buysell_005cadc8,0);
    if (iVar1 == 0) {
      iVar1 = FUN_00479700(s_response_005cadd0,0);
      if ((((iVar1 == 0) && (iVar1 = FUN_00479700(s_responsenohide_005caddc,0), iVar1 == 0)) &&
          (iVar1 = FUN_00479700(s_respnohide_005cadec,0), iVar1 == 0)) &&
         (iVar1 = FUN_00479700(s_respctrlon_005cadf8,0), iVar1 == 0)) {
        iVar1 = FUN_00479700(&DAT_005cae10,0);
        if (((iVar1 != 0) || (iVar1 = FUN_00479700(&DAT_005cae18,0), iVar1 != 0)) ||
           (iVar1 = FUN_00479700(s_object_005cae1c,0), iVar1 != 0)) {
          FUN_00479580();
          if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
            return 4;
          }
          iVar1 = FUN_00451fe0(*(undefined4 *)(param_2 + 0x28),param_1,0,0);
          if (iVar1 == 0) {
            FUN_0041ee50(s_Couldn_t_find_wait_char_005cae24);
          }
          FUN_00471310(iVar1);
          FUN_00479580();
          return 1;
        }
        iVar1 = FUN_00479700(s_death_005cae40,0);
        if (iVar1 == 0) {
          return 1;
        }
        FUN_00479580();
        if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
          return 4;
        }
        iVar1 = FUN_0041e690(*(undefined4 *)(param_2 + 0x28),param_3,param_4);
        FUN_00479580();
        if (iVar1 == 0) {
          FUN_0041ee50(s_Couldn_t_find_wait_char_005cae48);
        }
        iVar2 = FUN_00479700(&DAT_005cae64,0);
        if (iVar2 != 0) {
          FUN_00478a10();
          iVar1 = FUN_0041e690(*(undefined4 *)(param_2 + 0x28),iVar1,param_4);
        }
        FUN_00471350(iVar1);
        return 1;
      }
      iVar1 = FUN_00479700(s_respctrlon_005cae04,0);
      if (iVar1 == 0) {
        uVar3 = 2;
      }
      else {
        uVar3 = 10;
      }
    }
    else {
      uVar3 = 7;
    }
  }
  else {
    uVar3 = 6;
  }
  FUN_004712b0(uVar3,0);
  FUN_00479580();
  return 1;
}


