// FUN_0041b240_TArea_PlayCDMusic @ 0041b240 size=383

void __fastcall FUN_0041b240_TArea_PlayCDMusic(int param_1)

{
  int iVar1;
  DWORD DVar2;
  int iVar3;
  
  iVar1 = DAT_0065abf4;
  iVar3 = DAT_0065abf0;
  if (DAT_0065abec == 0) {
    DVar2 = GetTickCount();
    if (*(int *)(param_1 + 0x250) <= (int)(DVar2 - *(int *)(param_1 + 0x248))) {
      if ((DAT_0065abd8 == 0) && (iVar3 = FUN_0049a590(), iVar3 == 0)) {
        if (*(int *)(param_1 + 0x240) == 0) {
          iVar3 = *(int *)(param_1 + 0x1b8) + 1;
          *(int *)(param_1 + 0x1b8) = iVar3;
          if (*(int *)(param_1 + 0x1bc) <= iVar3) {
            *(undefined4 *)(param_1 + 0x1b8) = 0;
          }
        }
        else {
          do {
            iVar3 = FUN_00483300_RandomRange(0,*(int *)(param_1 + 0x1bc) + -1);
            if (iVar3 != *(int *)(param_1 + 0x1b8)) break;
          } while (1 < *(int *)(param_1 + 0x1bc));
          *(int *)(param_1 + 0x1b8) = iVar3;
        }
        iVar3 = FUN_0049a480(*(undefined4 *)(param_1 + 0x1c0 + *(int *)(param_1 + 0x1b8) * 4));
        *(int *)(param_1 + 0x254) = iVar3;
        iVar3 = iVar3 + *(int *)(param_1 + 0x244) * 1000;
        *(int *)(param_1 + 0x250) = iVar3;
        if (iVar3 < 0x3e9) {
          *(undefined4 *)(param_1 + 0x250) = 30000;
        }
        iVar3 = *(int *)(param_1 + 0x250);
        if (10000 < iVar3) {
          iVar3 = 10000;
        }
        *(int *)(param_1 + 0x250) = iVar3;
        FUN_0049a300(*(undefined4 *)(param_1 + 0x1c0 + *(int *)(param_1 + 0x1b8) * 4));
      }
      DVar2 = GetTickCount();
      *(DWORD *)(param_1 + 0x248) = DVar2;
    }
  }
  else {
    DVar2 = GetTickCount();
    if (iVar1 <= (int)(DVar2 - iVar3)) {
      iVar3 = DAT_0065abe0 + -1;
      FUN_0049a300(iVar3);
      DAT_0065abf4 = FUN_0049a480(iVar3);
      DAT_0065abec = 1;
      DAT_0065abf0 = GetTickCount();
      return;
    }
  }
  return;
}


