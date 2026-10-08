// FUN_004211a0 @ 004211a0 size=447

undefined4 FUN_004211a0(undefined4 param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  char *pcVar5;
  int iStack_3c;
  
  iVar2 = param_2;
  iVar1 = FUN_00479700(&DAT_005caff4,0);
  if (iVar1 != 0) {
    iVar2 = FUN_00440970();
    if (iVar2 != 0) {
      return 0;
    }
    FUN_0041ee50(s_Already_at_end_of_selection_list_005caffc);
    return 0;
  }
  iVar1 = FUN_00479700(&DAT_005cb020,0);
  if (iVar1 != 0) {
    iVar2 = FUN_004409a0();
    if (iVar2 != 0) {
      return 0;
    }
    FUN_0041ee50(s_Already_at_start_of_selection_li_005cb028);
    return 0;
  }
  iVar1 = FUN_00479700(&DAT_005cb050,0);
  if (iVar1 != 0) {
    FUN_0044cf80(0,0,0,0,0xffffffff);
    if (iStack_3c == 0) {
      return 0;
    }
    do {
      iVar2 = FUN_004405d0(*(undefined4 *)(iStack_3c + 0x40),1);
      if (iVar2 == 0) {
        FUN_0041ee50(s_Selection_overflowed__max_object_005cb054);
        return 0;
      }
      FUN_0044d080();
    } while (iStack_3c != 0);
    return 0;
  }
  if (*(int *)(iVar2 + 0x10) == 8) {
    iVar2 = FUN_0047a410(iVar2,&DAT_005cb084,&param_2);
    if (iVar2 == 0) {
      return 4;
    }
    iVar2 = FUN_00440940(param_2);
    if (iVar2 != 0) {
      return 0;
    }
    FUN_0041ee50(s_Invalid_selection_index__005cb088);
    return 0;
  }
  iVar1 = FUN_00451fe0(*(undefined4 *)(iVar2 + 0x28),param_1,1,0);
  if ((iVar1 == 0) && (iVar1 = FUN_00451d70(*(undefined4 *)(iVar2 + 0x28),1,0), iVar1 == 0)) {
    uVar4 = 0;
    uVar3 = FUN_0051f2e0(*(undefined4 *)(iVar2 + 0x28));
    iVar1 = FUN_0051eea0(uVar3,uVar4);
    if (iVar1 != 0) goto LAB_00421330;
    pcVar5 = s_Can_t_find_any_object_by_that_na_005cb0a4;
  }
  else {
LAB_00421330:
    iVar2 = FUN_004405d0(*(undefined4 *)(iVar1 + 0x40),0);
    if (iVar2 != 0) goto LAB_00421351;
    pcVar5 = s_Unable_to_select_any_more_object_005cb0cc;
  }
  FUN_0041ee50(pcVar5);
LAB_00421351:
  FUN_00479580();
  return 0;
}


