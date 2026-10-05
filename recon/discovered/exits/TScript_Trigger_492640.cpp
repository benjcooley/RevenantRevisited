// TScript_Trigger @ 0x00492640 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// manual trigger request (SCRIPT_ENGINE.md)
// FUN_00492640 @ 00492640 size=360

undefined4 __thiscall
FUN_00492640(int param_1,int param_2,char *param_3,char *param_4,int param_5,undefined4 param_6,
            undefined4 param_7,undefined4 param_8)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  int *piVar5;
  int iVar6;
  char *pcVar7;
  int iVar8;
  char *pcVar9;
  
  iVar6 = *(int *)(param_1 + 4);
  do {
    if (iVar6 == 0) {
      return 0;
    }
    iVar8 = 0;
    if (0 < *(int *)(iVar6 + 0x44)) {
      do {
        piVar5 = *(int **)(*(int *)(iVar6 + 0x1c) + iVar8 * 4);
        if (piVar5 == (int *)0x0) {
          piVar5 = *(int **)(iVar6 + 0x20);
        }
        if ((piVar5 != (int *)0x0) && (*piVar5 == param_2)) {
          if (param_3 == (char *)0x0) {
            if (param_4 != (char *)0x0) {
LAB_004926a1:
              iVar2 = FUN_0059a530(piVar5 + 2,param_4);
              if (iVar2 != 0) goto LAB_004926b6;
            }
          }
          else {
            iVar2 = FUN_0059a530(piVar5 + 2,param_3);
            if (iVar2 != 0) {
              if (param_4 != (char *)0x0) goto LAB_004926a1;
              goto LAB_004926b6;
            }
          }
          iVar6 = 0;
          if ((param_5 != 0) && (iVar8 = FUN_0059a530(param_6,&DAT_005da10c), iVar8 == 0)) {
            iVar6 = param_5;
          }
          if ((*(int *)(param_1 + 0x10) != -1) &&
             ((iVar6 == 0 || (*(int *)(iVar6 + 0x40) != *(int *)(param_1 + 0x10))))) {
            FUN_00494620(param_5);
            return 0;
          }
          *(int *)(param_1 + 0x18) = param_2;
          if (param_3 == (char *)0x0) goto LAB_0049274d;
          uVar3 = 0xffffffff;
          goto code_r0x0049272c;
        }
LAB_004926b6:
        iVar8 = iVar8 + 1;
      } while (iVar8 < *(int *)(iVar6 + 0x44));
    }
    iVar6 = *(int *)(iVar6 + 8);
  } while( true );
  while( true ) {
    uVar3 = uVar3 - 1;
    pcVar7 = param_3 + 1;
    cVar1 = *param_3;
    param_3 = pcVar7;
    if (cVar1 == '\0') break;
code_r0x0049272c:
    pcVar7 = param_3;
    if (uVar3 == 0) break;
  }
  uVar3 = ~uVar3;
  pcVar7 = pcVar7 + -uVar3;
  pcVar9 = (char *)(param_1 + 0x20);
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined4 *)pcVar9 = *(undefined4 *)pcVar7;
    pcVar7 = pcVar7 + 4;
    pcVar9 = pcVar9 + 4;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pcVar9 = *pcVar7;
    pcVar7 = pcVar7 + 1;
    pcVar9 = pcVar9 + 1;
  }
LAB_0049274d:
  if (param_4 != (char *)0x0) {
    uVar3 = 0xffffffff;
    do {
      pcVar7 = param_4;
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      pcVar7 = param_4 + 1;
      cVar1 = *param_4;
      param_4 = pcVar7;
    } while (cVar1 != '\0');
    uVar3 = ~uVar3;
    pcVar7 = pcVar7 + -uVar3;
    pcVar9 = (char *)(param_1 + 0x34);
    for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined4 *)pcVar9 = *(undefined4 *)pcVar7;
      pcVar7 = pcVar7 + 4;
      pcVar9 = pcVar9 + 4;
    }
    for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
      *pcVar9 = *pcVar7;
      pcVar7 = pcVar7 + 1;
      pcVar9 = pcVar9 + 1;
    }
  }
  *(undefined4 *)(param_1 + 0xcc) = param_6;
  *(int *)(param_1 + 0xc4) = param_5;
  *(undefined4 *)(param_1 + 0xd0) = param_8;
  *(undefined4 *)(param_1 + 200) = param_7;
  return 1;
}


