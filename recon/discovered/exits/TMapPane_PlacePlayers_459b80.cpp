// TMapPane_PlacePlayers @ 0x00459b80 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// re-adds active players that have no sector
// FUN_00459b80 @ 00459b80 size=336

void FUN_00459b80(void)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  int iStack_c;
  int iStack_8;
  int iStack_4;
  
  iVar3 = 0;
  iVar1 = FUN_0051ee70(0);
  if (0 < iVar1) {
    do {
      piVar2 = (int *)FUN_0051eea0(iVar3,0);
      if (piVar2 != (int *)0x0) {
        if (((piVar2[0xdb] & 1U) == 0) || (piVar2[0x11] != 0)) {
          if ((piVar2[0xdb] & 1U) != 0) goto LAB_00459c2b;
          if (piVar2[0x11] != 0) {
            if ((-1 < piVar2[0x14]) && (iVar1 = FUN_00452690(piVar2[0x14],0), iVar1 != 0)) {
              FUN_00451610(iVar1);
            }
            iVar1 = (**(code **)(*piVar2 + 0x20))();
            if (iVar1 != 0) {
              (**(code **)(*piVar2 + 0x2c))();
            }
            iVar1 = (**(code **)(*piVar2 + 0xb4))();
            if ((iVar1 == 0) && (piVar2[0x19] == 0)) {
              FUN_00451b10(piVar2);
            }
            else {
              (**(code **)(*piVar2 + 0x60))();
            }
            goto LAB_00459c2b;
          }
LAB_00459c30:
          iVar1 = (**(code **)(*piVar2 + 0x24))();
          if (iVar1 != 0) {
            (**(code **)(*piVar2 + 0x128))();
          }
        }
        else {
          FUN_00451090(piVar2);
LAB_00459c2b:
          if (piVar2[0x11] == 0) goto LAB_00459c30;
        }
        if (piVar2[0x198] != 0) {
          FUN_00586630(piVar2,piVar2[0x198]);
          iStack_8 = piVar2[5];
          iStack_4 = piVar2[6];
          iStack_c = piVar2[4];
          uStack_18 = 0;
          uStack_14 = 0;
          uStack_10 = 0;
          iVar1 = FUN_00475040(piVar2[0x198],*(undefined2 *)((int)piVar2 + 0xe),&iStack_c,0,0,
                               &uStack_18,0xffffffff,1);
          if (iVar1 != 0) {
            FUN_00451090(iVar1);
          }
          piVar2[0x198] = 0;
        }
      }
      iVar3 = iVar3 + 1;
      iVar1 = FUN_0051ee70(0);
    } while (iVar3 < iVar1);
  }
  return;
}


