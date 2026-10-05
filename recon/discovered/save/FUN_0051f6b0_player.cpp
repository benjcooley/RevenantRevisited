// FUN_0051f6b0 @ 0051f6b0 size=389

void __fastcall FUN_0051f6b0(int *param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int *local_10;
  int local_c;
  
  iVar8 = *param_1;
  if (0 < iVar8) {
    piVar4 = (int *)param_1[4];
    iVar6 = iVar8;
    do {
      iVar2 = *piVar4;
      if (iVar2 != 0) {
        *(undefined4 *)(iVar2 + 0x4ec) = 0;
        *(undefined4 *)(iVar2 + 0x4e4) = 0;
        *(undefined4 *)(iVar2 + 0x4e8) = 0;
        *(undefined4 *)(iVar2 + 0x4e0) = 0;
      }
      piVar4 = piVar4 + 1;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
  }
  local_c = param_1[5];
  if (0 < local_c) {
    local_10 = (int *)param_1[9];
    iVar6 = 0;
    do {
      iVar2 = *local_10;
      iVar7 = iVar6;
      if (iVar2 != 0) {
        *(undefined4 *)(iVar2 + 0x5c) = 0;
        *(undefined4 *)(iVar2 + 0x54) = 0;
        *(undefined4 *)(iVar2 + 0x58) = 0;
        *(undefined4 *)(iVar2 + 0x50) = 0;
        iVar7 = iVar2;
        if (0 < iVar8) {
          piVar4 = (int *)param_1[4];
          iVar5 = iVar8;
          do {
            iVar3 = *piVar4;
            if (iVar3 != 0) {
              cVar1 = *(char *)(iVar3 + 0x494);
              if ((cVar1 != '\0') && (*(int *)(iVar3 + 0x4dc) == *(int *)(iVar2 + 0x4c))) {
                *(int *)(iVar2 + 0x50) = *(int *)(iVar2 + 0x50) + *(int *)(iVar3 + 0x650);
                *(int *)(iVar2 + 0x58) = *(int *)(iVar2 + 0x58) + *(int *)(iVar3 + 0x658);
                *(int *)(iVar2 + 0x54) = *(int *)(iVar2 + 0x54) + *(int *)(iVar3 + 0x654);
                *(int *)(iVar2 + 0x5c) = *(int *)(iVar2 + 0x5c) + *(int *)(iVar3 + 0x65c);
              }
              if (((iVar6 != 0) && (cVar1 != '\0')) &&
                 (*(int *)(iVar3 + 0x4dc) == *(int *)(iVar6 + 0x4c))) {
                *(undefined4 *)(iVar3 + 0x4e0) = *(undefined4 *)(iVar6 + 0x50);
                *(undefined4 *)(iVar3 + 0x4e8) = *(undefined4 *)(iVar6 + 0x58);
                *(undefined4 *)(iVar3 + 0x4e4) = *(undefined4 *)(iVar6 + 0x54);
                *(undefined4 *)(iVar3 + 0x4ec) = *(undefined4 *)(iVar6 + 0x5c);
              }
            }
            piVar4 = piVar4 + 1;
            iVar5 = iVar5 + -1;
          } while (iVar5 != 0);
        }
      }
      local_10 = local_10 + 1;
      local_c = local_c + -1;
      iVar6 = iVar7;
    } while (local_c != 0);
    if ((iVar7 != 0) && (0 < iVar8)) {
      piVar4 = (int *)param_1[4];
      do {
        iVar6 = *piVar4;
        if (((iVar6 != 0) && (*(char *)(iVar6 + 0x494) != '\0')) &&
           (*(int *)(iVar6 + 0x4dc) == *(int *)(iVar7 + 0x4c))) {
          *(undefined4 *)(iVar6 + 0x4e0) = *(undefined4 *)(iVar7 + 0x50);
          *(undefined4 *)(iVar6 + 0x4e8) = *(undefined4 *)(iVar7 + 0x58);
          *(undefined4 *)(iVar6 + 0x4e4) = *(undefined4 *)(iVar7 + 0x54);
          *(undefined4 *)(iVar6 + 0x4ec) = *(undefined4 *)(iVar7 + 0x5c);
        }
        piVar4 = piVar4 + 1;
        iVar8 = iVar8 + -1;
      } while (iVar8 != 0);
    }
  }
  FUN_0057b1f0();
  return;
}


