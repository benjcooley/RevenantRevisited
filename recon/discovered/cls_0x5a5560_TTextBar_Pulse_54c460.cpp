// FUN_0054c460 @ 0054c460 size=407

void __fastcall FUN_0054c460(uint param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  uint extraout_ECX;
  uint uVar4;
  int iVar5;
  uint uStack_4;
  
  iVar3 = *(int *)(param_1 + 0x68);
  iVar1 = *(int *)(param_1 + 0x60) + -1;
  if (iVar3 <= iVar1) {
    piVar2 = (int *)(*(int *)(param_1 + 0x6c) + 8 + iVar1 * 0x5c);
    do {
      iVar5 = *piVar2;
      *piVar2 = iVar5 + -1;
      if ((iVar5 + -1 < 1) && (iVar5 = *(int *)(param_1 + 0x60) + -1, iVar1 == iVar5)) {
        *(int *)(param_1 + 0x60) = iVar5;
      }
      iVar1 = iVar1 + -1;
      piVar2 = piVar2 + -0x17;
    } while (iVar3 <= iVar1);
  }
  iVar3 = DAT_006766b0;
  if ((DAT_0066829c != 0) && (DAT_00667fcc != 0)) {
    if (DAT_006766b0 != *(int *)(param_1 + 0x120)) {
      if (*(int *)(param_1 + 0x120) + 1 < DAT_006766ac) {
        *(int *)(param_1 + 0x120) = DAT_006766ac + -1;
      }
      iVar1 = *(int *)(param_1 + 0x120) + 1;
      uStack_4 = param_1;
      if (iVar1 <= iVar3) {
        do {
          piVar2 = (int *)FUN_00570760(iVar1);
          if (piVar2 != (int *)0x0) {
            iVar3 = *piVar2;
            if (iVar3 != *(int *)(DAT_00667fcc + 0x664)) {
              if (iVar3 < 0) {
                iVar3 = 0;
                uVar4 = DAT_00667fcc;
LAB_0054c553:
                uStack_4 = DAT_00670660;
              }
              else {
                iVar3 = FUN_0051eea0(iVar3,0);
                uVar4 = extraout_ECX;
                if ((iVar3 == 0) ||
                   (uVar4 = (uint)*(byte *)(iVar3 + 0x494), *(byte *)(iVar3 + 0x494) == 0))
                goto LAB_0054c553;
                iVar5 = *(int *)(iVar3 + 0x4d8);
                if (0xf < iVar5) {
                  iVar5 = 0x10;
                }
                uVar4 = *(uint *)(&DAT_005e20b8 + iVar5 * 4);
                uStack_4 = uVar4;
              }
              if ((iVar3 == 0) || (*piVar2 < 0)) {
                piVar2 = piVar2 + 1;
                FUN_00444e20(0);
                FUN_0054d0c0(8,uVar4,piVar2);
              }
              else {
                piVar2 = piVar2 + 1;
                FUN_00419dd0(&uStack_4);
                FUN_0054d0c0(0x40,uVar4,piVar2);
                if (DAT_005d7a70 != 0) {
                  iVar3 = FUN_0049c430(s_viles_005e580c);
                  if (-1 < iVar3) {
                    iVar5 = FUN_0049b650(iVar3);
                    if (iVar5 != 0) {
                      FUN_0049b990(iVar3,0x7f,1,0,0x50,700);
                    }
                  }
                }
              }
            }
          }
          iVar1 = iVar1 + 1;
          iVar3 = DAT_006766b0;
        } while (iVar1 <= DAT_006766b0);
      }
    }
    *(int *)(param_1 + 0x120) = iVar3;
  }
  return;
}


