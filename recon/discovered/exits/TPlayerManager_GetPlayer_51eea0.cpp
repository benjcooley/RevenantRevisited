// TPlayerManager_GetPlayer @ 0x0051eea0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// this = 0x0065a890
// FUN_0051eea0 @ 0051eea0 size=77

int __thiscall FUN_0051eea0(uint *param_1,uint param_2,int param_3)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  
  if (param_3 == 0) {
    if (param_2 < *param_1) {
      return *(int *)(param_1[4] + param_2 * 4);
    }
  }
  else {
    uVar3 = 0;
    iVar2 = 0;
    if (0 < (int)*param_1) {
      piVar1 = (int *)param_1[4];
      do {
        if (*piVar1 != 0) {
          if (uVar3 == param_2) {
            return *piVar1;
          }
          uVar3 = uVar3 + 1;
        }
        iVar2 = iVar2 + 1;
        piVar1 = piVar1 + 1;
      } while (iVar2 < (int)*param_1);
    }
  }
  return 0;
}


