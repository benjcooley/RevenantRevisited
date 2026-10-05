// FUN_0049d2a0 @ 0049d2a0 size=932

/* WARNING: Removing unreachable block (ram,0x0049d5ac) */
/* WARNING: Removing unreachable block (ram,0x0049d5f5) */
/* WARNING: Removing unreachable block (ram,0x0049d5fb) */
/* WARNING: Removing unreachable block (ram,0x0049d5df) */
/* WARNING: Removing unreachable block (ram,0x0049d60f) */
/* WARNING: Removing unreachable block (ram,0x0049d612) */

undefined4 FUN_0049d2a0(void)

{
  undefined1 *puVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  int extraout_ECX;
  int iVar4;
  void *in_stack_00001174;
  void *local_c;
  undefined1 *local_8;
  int local_4;
  
  local_4 = 0xffffffff;
  local_8 = &LAB_0059dc42;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0058c030();
  iVar4 = 0;
  if (0 < *(int *)(extraout_ECX + 0x14)) {
    do {
      FUN_0049d920(iVar4);
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)(extraout_ECX + 0x14));
  }
  iVar4 = DAT_0065a784;
  *(int *)(extraout_ECX + 0x14) = 0;
  *(undefined4 *)(extraout_ECX + 0x18) = 0;
  if ((-1 < iVar4) && (iVar4 = *(int *)(DAT_0065a77c + iVar4 * 4), iVar4 != 0)) {
    FUN_0058b100(&stack0x00000074,s__s_s__s_dialog_def_005daac4,&DAT_0065d6a4,iVar4 + 0x58,
                 &DAT_0065bc18);
    iVar4 = FUN_004a13f0(&stack0x00000074,&DAT_005daad8,0);
    if (iVar4 == 0) {
      if (DAT_0065a784 < 0) {
        iVar4 = 0;
      }
      else {
        iVar4 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
      }
      FUN_0058b100(&stack0x00000074,s__s_s__s_def_005daadc,&DAT_0065d6a4,iVar4 + 0x58,&DAT_0065bc18)
      ;
      iVar4 = FUN_004a13f0(&stack0x00000074,&DAT_005daae8,0);
      if (iVar4 == 0) {
        if (DAT_0065a784 < 0) {
          iVar4 = 0;
        }
        else {
          iVar4 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
        }
        FUN_0058b100(&stack0x00000074,s__s_s__s_def_005daaf4,&DAT_0065d6a4,iVar4 + 0x58,
                     s_english_005daaec);
        iVar4 = FUN_004a13f0(&stack0x00000074,&DAT_005dab00,0);
        if (iVar4 == 0) {
          ExceptionList = in_stack_00001174;
          return 0;
        }
      }
    }
    FUN_00478720();
    local_c = (void *)0x0;
    local_8 = (undefined1 *)0x0;
    local_4 = 0;
    puVar1 = (undefined1 *)FUN_00482fb0(0x2000);
    *puVar1 = 0;
    iVar4 = FUN_004788d0(iVar4,&stack0x00000074);
    if (iVar4 != 0) {
      iVar4 = FUN_00479450();
      if (iVar4 == 0) {
        FUN_00479950(s_Syntax_error_in_header_005dab04,0);
      }
      do {
        iVar4 = FUN_0047a410(&local_c,s__63t__4091s_005dab1c,&stack0x00000034,&stack0x00000178);
        if (iVar4 == 0) {
          FUN_00479950(s_tag__line__expected_005dab28,0);
        }
        FUN_00479950(s_RETURN_expected_005dab3c,0);
        FUN_0059be72(&stack0x00000034);
        puVar2 = (undefined4 *)FUN_00482fb0(8);
        if (puVar2 == (undefined4 *)0x0) {
          puVar2 = (undefined4 *)0x0;
        }
        else {
          puVar2[1] = 0;
          *puVar2 = 0;
        }
        uVar3 = FUN_0059b6bc(&stack0x00000034);
        *puVar2 = uVar3;
        uVar3 = FUN_0059b6bc(&stack0x00000178);
        puVar2[1] = uVar3;
        FUN_0041c840(puVar2);
        FUN_00479450();
      } while( true );
    }
    FUN_004830f0(puVar1);
    if (local_8 == (undefined1 *)0x0) {
      if (local_4 != 0) {
        FUN_004830f0(0);
        FUN_004a1540(local_4);
      }
    }
    else {
      FUN_004830f0(0);
      FUN_004830f0(local_8);
    }
    FUN_00478730();
  }
  ExceptionList = in_stack_00001174;
  return 0;
}


