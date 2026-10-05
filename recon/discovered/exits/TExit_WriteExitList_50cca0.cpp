// TExit_WriteExitList @ 0x0050cca0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// static; only when dirty
// FUN_0050cca0 @ 0050cca0 size=380

undefined4 FUN_0050cca0(void)

{
  char cVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  char *pcVar7;
  char *pcVar8;
  char local_208 [260];
  char local_104 [260];
  
  if (DAT_0066d24c != 0) {
    iVar3 = FUN_0050c8f0(1);
    if (iVar3 == 0) {
      return 0;
    }
    if (DAT_0065a784 < 0) {
      iVar3 = 0;
    }
    else {
      iVar3 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    FUN_0058b100(local_104,&DAT_005e1768,&DAT_0065bd48,s_exit_def_005e175c);
    if (iVar3 != 0) {
      if (DAT_0065a784 < 0) {
        iVar3 = 0;
      }
      else {
        iVar3 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
      }
      FUN_0058b100(local_208,s__s_s__s_005e177c,&DAT_0065d6a4,iVar3 + 0x58,s_exit_def_005e1770);
      iVar3 = FUN_00481260(local_208,0);
      if (iVar3 != 0) {
        uVar5 = 0xffffffff;
        pcVar7 = local_208;
        do {
          pcVar8 = pcVar7;
          if (uVar5 == 0) break;
          uVar5 = uVar5 - 1;
          pcVar8 = pcVar7 + 1;
          cVar1 = *pcVar7;
          pcVar7 = pcVar8;
        } while (cVar1 != '\0');
        uVar5 = ~uVar5;
        pcVar7 = pcVar8 + -uVar5;
        pcVar8 = local_104;
        for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
          *(undefined4 *)pcVar8 = *(undefined4 *)pcVar7;
          pcVar7 = pcVar7 + 4;
          pcVar8 = pcVar8 + 4;
        }
        for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
          *pcVar8 = *pcVar7;
          pcVar7 = pcVar7 + 1;
          pcVar8 = pcVar8 + 1;
        }
      }
    }
    iVar3 = FUN_004457c0(local_104,&DAT_005e1784);
    puVar2 = DAT_0066d1c4;
    if (iVar3 == 0) {
      return 0;
    }
    for (; puVar2 != (undefined4 *)0x0; puVar2 = (undefined4 *)puVar2[8]) {
      iVar4 = FUN_0058b56e(iVar3,s__s___d___d___d__level__d_mapinde_005e1788,*puVar2,puVar2[1],
                           puVar2[2],puVar2[3],puVar2[4],puVar2[5],puVar2[6],
                           *(undefined1 *)((int)puVar2 + 0x1e),*(undefined1 *)((int)puVar2 + 0x1d),
                           *(undefined1 *)(puVar2 + 7));
      if (iVar4 == 0) {
        FUN_0058b4f1(iVar3);
        return 0;
      }
    }
    FUN_0058b4f1(iVar3);
    DAT_0066d24c = 0;
  }
  return 1;
}


