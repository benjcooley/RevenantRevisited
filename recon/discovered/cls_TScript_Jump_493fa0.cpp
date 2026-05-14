// FUN_00493fa0_TScript_Jump @ 00493fa0 size=757

/* WARNING: Removing unreachable block (ram,0x0049422f) */
/* WARNING: Removing unreachable block (ram,0x00494157) */
/* WARNING: Removing unreachable block (ram,0x0049418b) */
/* WARNING: Removing unreachable block (ram,0x0049417c) */
/* WARNING: Removing unreachable block (ram,0x0049419d) */
/* WARNING: Removing unreachable block (ram,0x004941d3) */
/* WARNING: Removing unreachable block (ram,0x004940ea) */
/* WARNING: Removing unreachable block (ram,0x004940f1) */
/* WARNING: Removing unreachable block (ram,0x00494208) */
/* WARNING: Removing unreachable block (ram,0x00494245) */
/* WARNING: Removing unreachable block (ram,0x0049424b) */
/* WARNING: Removing unreachable block (ram,0x0049425f) */
/* WARNING: Removing unreachable block (ram,0x00494262) */
/* WARNING: Removing unreachable block (ram,0x004941d9) */
/* WARNING: Removing unreachable block (ram,0x004941bd) */
/* WARNING: Removing unreachable block (ram,0x004941ed) */
/* WARNING: Removing unreachable block (ram,0x004941f0) */
/* WARNING: Removing unreachable block (ram,0x0049427b) */

void __fastcall FUN_00493fa0_TScript_Jump(int param_1)

{
  char cVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  char *pcVar5;
  undefined1 local_70 [100];
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d98d;
  local_c = ExceptionList;
  iVar4 = -1;
  pcVar5 = *(char **)(*(int *)(param_1 + 8) + 4);
  do {
    if (iVar4 == 0) break;
    iVar4 = iVar4 + -1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  ExceptionList = &local_c;
  FUN_00478720();
  local_4 = 0;
  puVar2 = (undefined1 *)FUN_00482fb0(0x2000);
  *puVar2 = 0;
  local_4 = 1;
  iVar4 = 1000;
  while (iVar4 = iVar4 + -1, *(int *)(param_1 + 0x4c) < iVar4) {
    iVar3 = FUN_004795f0();
    if (iVar3 == 0) {
      FUN_0058b100(local_70,s_Script_error_at_line__d___s_005da0d0,0,
                   s_BEGIN_without_matching_END_005da0f0);
      if (DAT_00668154 == 0) {
        FUN_0054d170(&DAT_0065c5d0,local_70);
      }
      else {
        FUN_0041ee50(local_70);
      }
    }
    FUN_00479680();
  }
  *(undefined4 *)(param_1 + 0xa4) = 0;
  do {
    FUN_00479680();
    iVar4 = FUN_00479700(s_BEGIN_005da200,0);
    if (iVar4 == 0) {
      iVar4 = FUN_00479700(&DAT_005da208,0);
      if (iVar4 != 0) {
        iVar4 = *(int *)(param_1 + 0xa4) + -1;
        goto LAB_00494143;
      }
    }
    else {
      iVar4 = *(int *)(param_1 + 0xa4) + 1;
LAB_00494143:
      *(int *)(param_1 + 0xa4) = iVar4;
    }
    FUN_004795a0();
  } while( true );
}


