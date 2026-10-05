// TLever_Pulse @ 0x0050dc40 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x110 (the 1998 TExit::Pulse model, player only)
// FUN_0050dc40 @ 0050dc40 size=538

void __fastcall FUN_0050dc40(int *param_1)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  undefined1 *puVar7;
  undefined1 *puVar8;
  undefined1 auStack_18 [4];
  undefined1 auStack_14 [4];
  undefined1 auStack_10 [4];
  undefined1 auStack_c [4];
  undefined1 auStack_8 [4];
  undefined1 auStack_4 [4];
  
  FUN_004708e0();
  if (DAT_00668154 == 0) {
    iVar2 = (**(code **)(*param_1 + 0x154))();
    if (iVar2 != 0) {
      iVar2 = (**(code **)(*param_1 + 0x1f0))();
      if (iVar2 != 0) {
        iVar2 = (**(code **)(*param_1 + 0x24))();
        if (iVar2 != 0) {
          iVar2 = (**(code **)(*param_1 + 0x24))();
          if (*(int *)(iVar2 + 8) != 0) {
            iVar2 = (**(code **)(*param_1 + 0x24))();
            uVar3 = (**(code **)(**(int **)(iVar2 + 8) + 0x88))((short)param_1[3]);
            iVar2 = FUN_0059a530(uVar3,s_CLOSING_005e189c);
            if (iVar2 == 0) {
              FUN_0050d530(0);
            }
            iVar2 = FUN_0059a530(uVar3,s_OPENING_005e18a4);
            if (iVar2 == 0) {
              FUN_0050d530(1);
            }
          }
        }
      }
    }
  }
  if ((DAT_00667fcc != 0) && (DAT_00668154 == 0)) {
    iVar2 = FUN_0046e8a0();
    if (iVar2 != 0) {
      puVar8 = auStack_8;
      puVar7 = auStack_c;
      (**(code **)(*param_1 + 0x27c))(auStack_18,auStack_14,auStack_4);
      iVar2 = ((int)auStack_10 * 0x10 - param_1[4]) + *(int *)(DAT_00667fcc + 0x10);
      iVar4 = (int)puVar7 * 0x10 + (*(int *)(DAT_00667fcc + 0x14) - param_1[5]);
      iVar6 = (int)(iVar2 + (iVar2 >> 0x1f & 0xfU)) >> 4;
      iVar2 = (int)(iVar4 + (iVar4 >> 0x1f & 0xfU)) >> 4;
      piVar5 = (int *)FUN_0046e8a0();
      (**(code **)(*piVar5 + 0xc4))((short)param_1[3]);
      if ((((iVar6 < 0) || (iVar2 < 0)) || ((int)puVar7 <= iVar6)) || ((int)puVar8 <= iVar2)) {
        if ((*(byte *)(param_1 + 0x39) & 2) != 0) {
          (**(code **)(*param_1 + 0x24c))();
        }
        param_1[0x39] = param_1[0x39] & 0xfffffff8;
      }
      else {
        if (((*(uint *)(DAT_00667fcc + 8) & 0x100000) != 0) && ((param_1[0x39] & 1U) == 0)) {
          param_1[0x39] = param_1[0x39] | 4;
        }
        FUN_004cdf30();
        uVar1 = param_1[0x39];
        param_1[0x39] = uVar1 | 1;
        if ((uVar1 & 2) == 0) {
          iVar2 = param_1[0x3a];
          iVar4 = (**(code **)(*param_1 + 0x26c))();
          param_1[0x3a] = iVar2 + 1;
          if (iVar4 < iVar2) {
            param_1[0x3a] = 0;
            iVar2 = (**(code **)(*param_1 + 0x248))(0,0);
            if (iVar2 != 0) {
              param_1[0x39] = param_1[0x39] | 2;
              return;
            }
          }
        }
      }
    }
  }
  return;
}


