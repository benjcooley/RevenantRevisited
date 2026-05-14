// FUN_00499360_TSector_ObjectFlagsChanged @ 00499360 size=375

void __thiscall FUN_00499360_TSector_ObjectFlagsChanged(int param_1,int param_2,uint param_3,uint param_4)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 uVar3;
  bool bVar4;
  uint uVar5;
  int *piVar6;
  int *piVar7;
  int iVar8;
  
  if ((*(int *)(param_2 + 0x44) == param_1) && (((param_3 ^ param_4) & 0xc00c) != 0)) {
    param_3 = 0;
    if (0 < *(int *)(param_1 + 0xb8)) {
      piVar6 = *(int **)(param_1 + 200);
      do {
        if (*piVar6 == param_2) {
          param_4 = 1;
          goto LAB_004993d5;
        }
        param_3 = param_3 + 1;
        piVar6 = piVar6 + 1;
      } while ((int)param_3 < *(int *)(param_1 + 0xb8));
    }
    param_3 = -1;
    param_4 = 1;
LAB_004993d5:
    do {
      switch(param_4) {
      case 1:
        uVar5 = *(uint *)(param_2 + 8) & 8;
        break;
      case 2:
        if ((*(short *)(param_2 + 4) != 0xc) && (*(short *)(param_2 + 4) != 0xb))
        goto switchD_004993da_default;
      case 0:
        uVar5 = 1;
        break;
      case 3:
        uVar5 = *(uint *)(param_2 + 8) & 4;
        break;
      case 4:
        uVar5 = *(uint *)(param_2 + 8) & 0x8000;
        break;
      case 5:
        uVar5 = *(uint *)(param_2 + 8) & 0x4000;
        break;
      case 6:
        uVar5 = *(uint *)(param_2 + 8) & 0x40000;
        break;
      default:
switchD_004993da_default:
        uVar5 = 0;
      }
      iVar8 = 0;
      bVar4 = false;
      iVar2 = *(int *)(param_1 + 0xb4 + param_4 * 0x18);
      puVar1 = (undefined4 *)(param_1 + 0xb4 + param_4 * 0x18);
      if (0 < iVar2) {
        piVar6 = (int *)puVar1[4];
        do {
          piVar7 = (int *)*piVar6;
          if (piVar7 == (int *)0x0) {
            piVar7 = (int *)puVar1[5];
          }
          if ((*piVar7 == param_3) && (bVar4 = true, uVar5 == 0)) {
            iVar2 = *(int *)(puVar1[4] + iVar8 * 4);
            if (iVar2 != 0) {
              FUN_004830f0(iVar2);
            }
            FUN_0041cb80(iVar8);
            goto LAB_0049949a;
          }
          iVar8 = iVar8 + 1;
          piVar6 = piVar6 + 1;
        } while (iVar8 < iVar2);
      }
      if ((uVar5 != 0) && (!bVar4)) {
        uVar3 = *puVar1;
        piVar6 = (int *)FUN_00482fb0(4);
        *piVar6 = param_3;
        FUN_0041c910(piVar6,uVar3);
      }
LAB_0049949a:
      param_4 = param_4 + 1;
    } while ((int)param_4 < 7);
  }
  return;
}


