// FUN_00422300 @ 00422300 size=189

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_00422300(undefined4 param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar1 = param_2;
  uVar3 = 0;
  iVar2 = *(int *)(param_2 + 0x10);
  do {
    if (iVar2 != 4) {
      iVar2 = FUN_0047a410(iVar1,&DAT_005cb380,&param_2);
      if (iVar2 == 0) {
        return 4;
      }
      FUN_0045a490(param_2,uVar3,0);
      return 0;
    }
    iVar2 = FUN_00479700(s_nonzero_005cb364,1);
    if (iVar2 == 0) {
      iVar2 = FUN_00479700(s_absolute_005cb36c,1);
      if (iVar2 == 0) {
        iVar2 = FUN_00479700(s_reset_005cb378,1);
        if (iVar2 == 0) {
          return 4;
        }
        _DAT_00667194 = 0;
        _DAT_00667198 = 0;
        _DAT_0066719c = 0x40;
        _DAT_006671a0 = 0x40;
        FUN_00479580();
        return 0;
      }
    }
    else {
      uVar3 = 1;
    }
    FUN_00479580();
    iVar2 = *(int *)(iVar1 + 0x10);
  } while( true );
}


