// FUN_0049bd90 @ 0049bd90 size=274

void __thiscall FUN_0049bd90(int *param_1,int param_2)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int *piVar6;
  
  if ((((DAT_00668114 == 0) && (-1 < param_2)) && (param_2 <= param_1[7])) &&
     ((iVar2 = *(int *)(param_1[0xb] + param_2 * 4), iVar2 != 0 && (*param_1 != 0)))) {
    piVar5 = param_1 + 0x1f;
    iVar4 = 0;
    piVar6 = piVar5;
    do {
      if ((*piVar6 != 0) && (iVar3 = _AIL_3D_user_data_8(*piVar6,0), param_2 == iVar3)) {
        _AIL_end_3D_sample_4(param_1[iVar4 + 0x1f]);
        _AIL_set_3D_user_data_12(param_1[iVar4 + 0x1f],0,0xffffffff);
        if (iVar4 < 0x10) goto LAB_0049be57;
        break;
      }
      iVar4 = iVar4 + 1;
      piVar6 = piVar6 + 1;
    } while (iVar4 < 0x10);
    iVar4 = 0;
    piVar6 = param_1 + 0xf;
    do {
      if ((*piVar6 != 0) && (iVar3 = _AIL_sample_user_data_8(*piVar6,0), param_2 == iVar3)) {
        _AIL_end_sample_4(param_1[iVar4 + 0xf]);
        _AIL_set_sample_user_data_12(param_1[iVar4 + 0xf],0,0xffffffff);
        break;
      }
      iVar4 = iVar4 + 1;
      piVar6 = piVar6 + 1;
    } while (iVar4 < 0x10);
LAB_0049be57:
    iVar4 = 0;
    do {
      if (((piVar5[-0x10] != 0) &&
          (iVar3 = _AIL_sample_user_data_8(piVar5[-0x10],0), param_2 == iVar3)) ||
         ((*piVar5 != 0 && (iVar3 = _AIL_3D_user_data_8(*piVar5,0), param_2 == iVar3)))) break;
      iVar4 = iVar4 + 1;
      piVar5 = piVar5 + 1;
    } while (iVar4 < 0x10);
    if (iVar4 == 0x10) {
      puVar1 = (uint *)(iVar2 + 0x18);
      *puVar1 = *puVar1 & 0xfffffffb;
    }
  }
  return;
}


