// FUN_00499be0_TSector_ClearPreloadSectors @ 00499be0 size=371

void FUN_00499be0_TSector_ClearPreloadSectors(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int local_8;
  
  iVar7 = DAT_00668790 + -1;
  if (iVar7 < 0) {
    DAT_006687a8 = 0;
    DAT_005da754 = 0xffffffff;
    return;
  }
  do {
    iVar6 = 0;
    iVar1 = *(int *)(DAT_006687a0 + iVar7 * 4);
    if (param_1 == *(int *)(iVar1 + 4)) {
      if ((0 < param_2) && (param_3 != 0)) {
        if ((*(uint *)(iVar1 + 8) < 0x20) && (*(uint *)(iVar1 + 0xc) < 0x20)) {
          iVar4 = *(uint *)(iVar1 + 8) * 0x400;
          iVar2 = *(uint *)(iVar1 + 0xc) * 0x400;
          local_8 = iVar4 + 0x3ff;
          iVar6 = iVar2 + 0x3ff;
        }
        else {
          iVar4 = 0;
          iVar2 = 0;
          local_8 = 0;
        }
        iVar5 = 0;
        if (0 < param_2) {
          piVar3 = (int *)(param_3 + 4);
          do {
            if ((((piVar3[-1] <= local_8) && (iVar4 <= piVar3[1])) && (*piVar3 <= iVar6)) &&
               (iVar2 <= piVar3[2])) {
              if (iVar5 < param_2) goto LAB_00499c8d;
              break;
            }
            iVar5 = iVar5 + 1;
            piVar3 = piVar3 + 4;
          } while (iVar5 < param_2);
        }
        goto LAB_00499c97;
      }
LAB_00499c8d:
      if (param_4 != 0) goto LAB_00499c97;
    }
    else {
LAB_00499c97:
      *(undefined4 *)(iVar1 + 0x10) = 0;
      if (*(int *)(iVar1 + 0x94) < 2) {
        FUN_00498a40_TSector_Save();
        iVar6 = 0;
        if (0 < DAT_00668578) {
          do {
            if (iVar1 == *(int *)(DAT_00668588 + iVar6 * 4)) {
              FUN_0041cb80(iVar6);
            }
            iVar6 = iVar6 + 1;
          } while (iVar6 < DAT_00668578);
        }
        if (iVar1 != 0) {
          FUN_00498190_TSector_Dtor();
          FUN_004830f0(iVar1);
        }
        FUN_0041cb80(iVar7);
      }
      else {
        *(int *)(iVar1 + 0x94) = *(int *)(iVar1 + 0x94) + -1;
        FUN_0041cb80(iVar7);
      }
    }
    iVar7 = iVar7 + -1;
    if (iVar7 < 0) {
      DAT_006687a8 = 0;
      DAT_005da754 = 0xffffffff;
      return;
    }
  } while( true );
}


