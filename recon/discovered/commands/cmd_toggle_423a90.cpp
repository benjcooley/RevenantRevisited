// FUN_00423a90 @ 00423a90 size=423

undefined4 FUN_00423a90(int param_1,int param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  bool bVar5;
  
  if (*(int *)(param_2 + 0x10) == 4) {
    iVar4 = 0;
    iVar2 = FUN_00472d50();
    if (iVar2 < 1) goto LAB_00423b00;
    do {
      iVar2 = FUN_00472d60(iVar4);
      iVar3 = FUN_00479700(iVar2,2);
      if (iVar3 != 0) break;
      iVar4 = iVar4 + 1;
      iVar3 = FUN_00472d50();
    } while (iVar4 < iVar3);
  }
  else {
    if (*(int *)(param_2 + 0x10) != 8) {
      return 4;
    }
    uVar1 = __ftol();
    iVar2 = FUN_00472d60(uVar1);
  }
  if (iVar2 == 0) {
LAB_00423b00:
    FUN_0041ee50(s_No_object_flag_by_that_name_005cb88c);
    iVar2 = *(int *)(param_2 + 0x10);
    if (iVar2 != 9) {
      while (iVar2 != 10) {
        FUN_00478a10();
        iVar2 = *(int *)(param_2 + 0x10);
        if (iVar2 == 9) {
          return 0;
        }
      }
    }
    return 0;
  }
  FUN_00479580();
  iVar4 = FUN_00479700(&DAT_005cb8ac,0);
  if (iVar4 != 0) {
    FUN_00479580();
  }
  iVar4 = FUN_00479700(&DAT_005cb8b0,0);
  if (((iVar4 == 0) && (iVar4 = FUN_00479700(&DAT_005cb8b4,0), iVar4 == 0)) &&
     (iVar4 = FUN_00479700(&DAT_005cb8b8,0), iVar4 == 0)) {
    iVar4 = FUN_00479700(&PTR_DAT_005cb8c0,0);
    if (((iVar4 == 0) && (iVar4 = FUN_00479700(&DAT_005cb8c4,0), iVar4 == 0)) &&
       (iVar4 = FUN_00479700(s_false_005cb8c8,0), iVar4 == 0)) {
      iVar4 = FUN_00472e30(iVar2);
      bVar5 = iVar4 == 0;
      goto LAB_00423be0;
    }
    bVar5 = false;
  }
  else {
    bVar5 = true;
  }
  FUN_00479580();
LAB_00423be0:
  FUN_00472db0(iVar2,bVar5);
  iVar2 = FUN_0059a530(iVar2,s_invisible_005cb8d0);
  if (iVar2 == 0) {
    FUN_00454920(*(undefined4 *)(param_1 + 0x40));
  }
  if (DAT_0066829c == 0) {
    return 0;
  }
  if (DAT_0067682c == 0) {
    return 0;
  }
  FUN_00583fe0(param_1,0x46,*(undefined4 *)(param_1 + 8),0,0xb);
  return 0;
}


