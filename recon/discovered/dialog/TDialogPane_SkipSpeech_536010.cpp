// FUN_00536010 @ 00536010 size=219

void __fastcall FUN_00536010(int param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  
  if (((*(int *)(param_1 + 0x40) != 0) && (*(int *)(param_1 + 400) == 0)) &&
     (uVar3 = 0, 0 < *(int *)(param_1 + 0x17c))) {
    do {
      iVar2 = 0;
      if (((*(int *)(param_1 + 0x18c) != 0) && (uVar3 < *(uint *)(param_1 + 0x17c))) &&
         (*(int *)(*(int *)(param_1 + 0x18c) + uVar3 * 4) != 0)) {
        iVar1 = FUN_00452690(*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x18c) + uVar3 * 4) + 4),2)
        ;
        if (iVar1 != 0) {
          FUN_004d6000();
        }
        iVar1 = *(int *)(*(int *)(param_1 + 0x18c) + uVar3 * 4);
        *(undefined4 *)(iVar1 + 0x50) = 1;
        *(undefined4 *)(iVar1 + 0x58) = 0;
        if (0 < *(int *)(iVar1 + 0x5c)) {
          piVar4 = (int *)(iVar1 + 0x100);
          do {
            if (*piVar4 != 0) {
              FUN_004367d0(*(undefined4 *)(*piVar4 + 0xc));
              *piVar4 = 0;
            }
            iVar2 = iVar2 + 1;
            piVar4 = piVar4 + 1;
          } while (iVar2 < *(int *)(iVar1 + 0x5c));
        }
      }
      uVar3 = uVar3 + 1;
    } while ((int)uVar3 < *(int *)(param_1 + 0x17c));
  }
  return;
}


