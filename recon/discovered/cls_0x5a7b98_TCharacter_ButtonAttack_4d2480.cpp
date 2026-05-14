// FUN_004d2480_ButtonAttack @ 004d2480 size=867

/* WARNING: Type propagation algorithm not settling */

undefined4 __thiscall FUN_004d2480_ButtonAttack(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int *piVar5;
  int aiStack_34 [13];
  
  iVar1 = (**(code **)(*param_1 + 0x1c0))();
  if (0 < iVar1) {
    if ((param_1[0x44] & 0x80000U) == 0) {
      iVar1 = *(int *)(param_1[0x36] + 0x48);
      if ((iVar1 != 0) && ((*(uint *)(iVar1 + 0x24) & 0x2000000) != 0)) {
        return 0;
      }
      iVar1 = *(int *)(param_1[0x36] + 0x4c);
      if ((iVar1 != 0) && ((*(byte *)(iVar1 + 0x24) & 0x80) != 0)) {
        return 0;
      }
    }
    piVar5 = (int *)param_1[0x38];
    aiStack_34[5] = 0;
    if ((piVar5 == (int *)0x0) ||
       (((iVar1 = *piVar5, iVar1 != 3 && ((piVar5 == (int *)0x0 || (iVar1 != 0x19)))) ||
        (piVar5 = (int *)piVar5[0x11], piVar5 == (int *)0x0)))) {
      iVar1 = FUN_004cd690_FindCharacters(aiStack_34 + 0xc,1,0xffffffff,*(undefined1 *)((int)param_1 + 0x36),0x20,7
                          );
      piVar5 = (int *)((iVar1 < 1) - 1 & aiStack_34[0xc]);
    }
    else if ((iVar1 != 3) && (iVar1 != 0x19)) {
      piVar5 = (int *)0x0;
    }
    if ((((param_1[0x58] != 0) && ((*(uint *)(param_1[0x58] + 0x24) & 0x4000) != 0)) &&
        (iVar1 = FUN_0047e920_PlayScreen_GameFrame(), iVar1 - param_1[0x59] <= *(int *)(param_1[0x58] + 0xc0))) &&
       (param_1[0x5b] < 3)) {
      param_1[0x5b] = param_1[0x5b] + 1;
      return 1;
    }
    if ((((short)param_1[1] == 0xb) && (piVar5 != (int *)0x0)) &&
       (((short)piVar5[1] != 0xb && ((DAT_0066829c == 0 || (DAT_0067682c != 0)))))) {
      if (param_2 == param_1[0xa3]) {
        iVar1 = param_1[0xa4];
        param_1[0xa4] = iVar1 + 1;
        if (2 < iVar1 + 1) {
          aiStack_34[5] = 1;
        }
      }
      else {
        param_1[0xa4] = 1;
      }
      param_1[0xa3] = param_2;
      if ((aiStack_34[5] != 0) && (iVar1 = FUN_00483300_RandomRange(0,10), iVar1 == 0)) {
        iVar2 = FUN_00483300_RandomRange(1,0x32);
        iVar1 = *piVar5;
        aiStack_34[0] = -1;
        aiStack_34[4] = 0xffffffff;
        aiStack_34[3] = 0xffffffff;
        aiStack_34[2] = 0xffffffff;
        aiStack_34[1] = 100;
        piVar5[0x48] = 0;
        uVar3 = (**(code **)(iVar1 + 0x1e0))();
        (**(code **)(iVar1 + 0x1cc))(uVar3);
        aiStack_34[7] = 0;
        do {
          iVar1 = FUN_004d1ff0_FindButtonAttack(0,iVar2 + 0x32,aiStack_34,aiStack_34 + 4,aiStack_34 + 3,
                               aiStack_34 + 2,aiStack_34 + 1);
          if ((iVar1 != 0) ||
             (iVar1 = FUN_004d1eb0_FindPcntAttack(0,iVar2 + 0x32,aiStack_34,aiStack_34 + 4,aiStack_34 + 3,
                                   aiStack_34 + 2,aiStack_34 + 1), iVar1 != 0)) {
            iVar4 = *(int *)(*(int *)(piVar5[0x3f] + 0xdc) + aiStack_34[0] * 4);
            if (iVar4 == 0) {
              iVar4 = *(int *)(piVar5[0x3f] + 0xe0);
            }
            if ((*(uint *)(iVar4 + 0x24) & 0x1800000) == 0) break;
            iVar1 = 0;
          }
          aiStack_34[7] = aiStack_34[7] + 1;
        } while (aiStack_34[7] < 10);
        if (iVar1 != 0) {
          aiStack_34[1] = 0;
          iVar1 = FUN_004d2120_DoAttack(aiStack_34[0],aiStack_34[4],aiStack_34[3],aiStack_34[2],0,param_1);
          if (iVar1 != 0) {
            return 0;
          }
        }
      }
    }
    iVar1 = FUN_00483300_RandomRange(1,0x32);
    iVar2 = FUN_00483300_RandomRange(1,0x32);
    aiStack_34[0xb] = 0xffffffff;
    aiStack_34[10] = 0xffffffff;
    aiStack_34[9] = 0xffffffff;
    aiStack_34[8] = 0xffffffff;
    aiStack_34[6] = 0xffffffff;
    if (aiStack_34[5] != 0) {
      aiStack_34[6] = 100;
    }
    iVar1 = FUN_004d1dd0(param_2,iVar1 + iVar2,aiStack_34 + 0xb,aiStack_34 + 10,aiStack_34 + 9,
                         aiStack_34 + 8,aiStack_34 + 6,0,piVar5);
    if (iVar1 != 0) {
      uVar3 = FUN_004d2120_DoAttack(aiStack_34[0xb],aiStack_34[10],aiStack_34[9],aiStack_34[8],aiStack_34[6],
                           piVar5);
      return uVar3;
    }
  }
  return 0;
}


