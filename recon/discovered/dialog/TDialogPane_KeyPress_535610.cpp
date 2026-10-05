// FUN_00535610 @ 00535610 size=334

void __thiscall FUN_00535610(int param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  uint local_4;
  
  if (param_3 != 0) {
    if (param_2 == 0x20) {
      if (((*(int *)(param_1 + 0x40) != 0) && (*(int *)(param_1 + 400) == 0)) &&
         (local_4 = 0, 0 < *(int *)(param_1 + 0x17c))) {
        do {
          iVar2 = 0;
          if (((*(int *)(param_1 + 0x18c) != 0) && (local_4 < *(uint *)(param_1 + 0x17c))) &&
             (*(int *)(*(int *)(param_1 + 0x18c) + local_4 * 4) != 0)) {
            iVar1 = FUN_00452690(*(undefined4 *)
                                  (*(int *)(*(int *)(param_1 + 0x18c) + local_4 * 4) + 4),2);
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
    }
    else if (((0x30 < param_2) && (param_2 < 0x37)) &&
            ((param_2 + -0x31 < *(int *)(param_1 + 0x1d8) && (*(int *)(param_1 + 400) != 0)))) {
      *(int *)(param_1 + 0x1dc) = param_2 + -0x31;
      *(undefined4 *)(param_1 + 0x1e4) = 1;
      return;
    }
  }
  FUN_004361f0(param_2,param_3);
  return;
}


