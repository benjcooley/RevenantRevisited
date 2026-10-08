// TPlayerManager_NumPlayers @ 0x0051ee70 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// this = 0x0065a890 (PlayerManager)
// FUN_0051ee70 @ 0051ee70 size=39

int __thiscall FUN_0051ee70(int *param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  
  if (param_2 == 0) {
    return *param_1;
  }
  iVar3 = *param_1;
  iVar1 = 0;
  if (0 < iVar3) {
    piVar2 = (int *)param_1[4];
    do {
      if (*piVar2 != 0) {
        iVar1 = iVar1 + 1;
      }
      piVar2 = piVar2 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  return iVar1;
}


