// FUN_00535760 @ 00535760 size=268

void __thiscall FUN_00535760(int param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  uint local_4;
  
  if ((((param_3 != 0) && (param_2 == 0x40a)) && (*(int *)(param_1 + 0x40) != 0)) &&
     ((*(int *)(param_1 + 400) == 0 && (local_4 = 0, 0 < *(int *)(param_1 + 0x17c))))) {
    do {
      iVar2 = 0;
      if ((*(int *)(param_1 + 0x18c) != 0) &&
         ((local_4 < *(uint *)(param_1 + 0x17c) &&
          (*(int *)(*(int *)(param_1 + 0x18c) + local_4 * 4) != 0)))) {
        iVar1 = FUN_00452690(*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x18c) + local_4 * 4) + 4),
                             2);
        if (iVar1 != 0) {
          FUN_004d6000();
        }
        iVar1 = *(int *)(*(int *)(param_1 + 0x18c) + local_4 * 4);
        *(undefined4 *)(iVar1 + 0x50) = 1;
        *(undefined4 *)(iVar1 + 0x58) = 0;
        if (0 < *(int *)(iVar1 + 0x5c)) {
          piVar3 = (int *)(iVar1 + 0x100);
          do {
            if (*piVar3 != 0) {
              FUN_004367d0(*(undefined4 *)(*piVar3 + 0xc));
              *piVar3 = 0;
            }
            iVar2 = iVar2 + 1;
            piVar3 = piVar3 + 1;
          } while (iVar2 < *(int *)(iVar1 + 0x5c));
        }
      }
      local_4 = local_4 + 1;
    } while ((int)local_4 < *(int *)(param_1 + 0x17c));
  }
  FUN_00436340(param_2,param_3);
  return;
}


