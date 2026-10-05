// FUN_0049ceb0 @ 0049ceb0 size=729

/* WARNING: Removing unreachable block (ram,0x0049d0f0) */
/* WARNING: Removing unreachable block (ram,0x0049d13a) */
/* WARNING: Removing unreachable block (ram,0x0049d140) */
/* WARNING: Removing unreachable block (ram,0x0049d124) */
/* WARNING: Removing unreachable block (ram,0x0049d154) */
/* WARNING: Removing unreachable block (ram,0x0049d157) */

undefined4 FUN_0049ceb0(void)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  int *extraout_ECX;
  int iVar5;
  void *in_stack_00001174;
  void *local_c;
  undefined1 *local_8;
  int local_4;
  
  local_4 = 0xffffffff;
  local_8 = &LAB_0059dc0c;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0058c030();
  iVar5 = 0;
  if (0 < *extraout_ECX) {
    do {
      FUN_0049d920(iVar5);
      iVar5 = iVar5 + 1;
    } while (iVar5 < *extraout_ECX);
  }
  piVar1 = extraout_ECX + 5;
  *extraout_ECX = 0;
  iVar5 = 0;
  extraout_ECX[1] = 0;
  if (0 < *piVar1) {
    do {
      FUN_0049d920(iVar5);
      iVar5 = iVar5 + 1;
    } while (iVar5 < *piVar1);
  }
  *piVar1 = 0;
  extraout_ECX[6] = 0;
  FUN_0058b100(&stack0x00000074,s__s_s_def_005daa70,&DAT_0065bd48,&DAT_0065bc18);
  FUN_00478720();
  local_c = (void *)0x0;
  local_8 = (undefined1 *)0x0;
  local_4 = 0;
  puVar2 = (undefined1 *)FUN_00482fb0(0x2000);
  *puVar2 = 0;
  iVar5 = FUN_004789c0(&stack0x00000074);
  if (iVar5 == 0) {
    FUN_004830f0(puVar2);
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
    ExceptionList = in_stack_00001174;
    return 0;
  }
  iVar5 = FUN_00479450();
  if (iVar5 == 0) {
    FUN_00479950(s_Syntax_error_in_header_005daa7c,0);
  }
  do {
    iVar5 = FUN_0047a410(&local_c,s__63t__4091s_005daa94,&stack0x00000034,&stack0x00000178);
    if (iVar5 == 0) {
      FUN_00479950(s_tag__line__expected_005daaa0,0);
    }
    FUN_00479950(s_RETURN_expected_005daab4,0);
    FUN_0059be72(&stack0x00000034);
    puVar3 = (undefined4 *)FUN_00482fb0(8);
    if (puVar3 == (undefined4 *)0x0) {
      puVar3 = (undefined4 *)0x0;
    }
    else {
      puVar3[1] = 0;
      *puVar3 = 0;
    }
    uVar4 = FUN_0059b6bc(&stack0x00000034);
    *puVar3 = uVar4;
    uVar4 = FUN_0059b6bc(&stack0x00000178);
    puVar3[1] = uVar4;
    FUN_0041c840(puVar3);
    FUN_00479450();
  } while( true );
}


