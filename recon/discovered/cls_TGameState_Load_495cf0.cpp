// FUN_00495cf0_TGameState_Load @ 00495cf0 size=673

/* WARNING: Removing unreachable block (ram,0x00495f0a) */
/* WARNING: Removing unreachable block (ram,0x00495f40) */
/* WARNING: Removing unreachable block (ram,0x00495f46) */
/* WARNING: Removing unreachable block (ram,0x00495e43) */
/* WARNING: Removing unreachable block (ram,0x00495e4c) */
/* WARNING: Removing unreachable block (ram,0x00495e4e) */
/* WARNING: Removing unreachable block (ram,0x00495e6f) */
/* WARNING: Removing unreachable block (ram,0x00495e71) */
/* WARNING: Removing unreachable block (ram,0x00495e83) */
/* WARNING: Removing unreachable block (ram,0x00495e85) */
/* WARNING: Removing unreachable block (ram,0x00495e8a) */
/* WARNING: Removing unreachable block (ram,0x00495e8c) */
/* WARNING: Removing unreachable block (ram,0x00495ea8) */
/* WARNING: Removing unreachable block (ram,0x00495eb1) */
/* WARNING: Removing unreachable block (ram,0x00495eb8) */
/* WARNING: Removing unreachable block (ram,0x00495ecd) */
/* WARNING: Removing unreachable block (ram,0x00495f2a) */
/* WARNING: Removing unreachable block (ram,0x00495f5a) */
/* WARNING: Removing unreachable block (ram,0x00495f5d) */

void __thiscall FUN_00495cf0_TGameState_Load(int *param_1,undefined4 param_2)

{
  undefined1 *puVar1;
  int *piVar2;
  int iVar3;
  undefined1 local_110 [260];
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059da42;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  if (0 < *param_1) {
    iVar3 = 0;
    ExceptionList = &pvStack_c;
    if (0 < *param_1) {
      piVar2 = param_1 + 0x1001;
      ExceptionList = &pvStack_c;
      do {
        if (*piVar2 != 0) {
          FUN_004830f0(*piVar2);
        }
        *piVar2 = 0;
        iVar3 = iVar3 + 1;
        piVar2 = piVar2 + 1;
      } while (iVar3 < *param_1);
    }
    *param_1 = 0;
  }
  if (DAT_0065a784 < 0) {
    iVar3 = 0;
  }
  else {
    iVar3 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
  }
  FUN_0058b100(local_110,s__s_s__s_005da474,&DAT_0065d6a4,iVar3 + 0x58,param_2);
  iVar3 = FUN_004a1c00(local_110,0);
  if (iVar3 == 0) {
    FUN_0058b100(local_110,&DAT_005da47c,&DAT_0065bd48,param_2);
  }
  FUN_00478720();
  local_4 = 0;
  puVar1 = (undefined1 *)FUN_00482fb0(0x2000);
  *puVar1 = 0;
  local_4 = 1;
  FUN_004789c0(local_110);
  FUN_004795a0();
  *param_1 = 0;
  do {
    FUN_00479680();
    FUN_00481c10(s_Expected_gamestate_identifier_in_005da4b0,param_2);
  } while( true );
}


