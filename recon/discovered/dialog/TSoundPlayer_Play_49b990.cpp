// FUN_0049b990 @ 0049b990 size=1021

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall
FUN_0049b990(int *param_1,int param_2,int param_3,int param_4,int *param_5,int param_6,int param_7)

{
  int iVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  uint uVar7;
  int *piVar8;
  undefined1 local_24 [4];
  undefined1 local_20 [4];
  undefined1 local_1c [4];
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  int local_4;
  
  if ((((DAT_00668114 == 0) && (-1 < param_2)) && (param_2 <= param_1[7])) &&
     ((iVar1 = *(int *)(param_1[0xb] + param_2 * 4), iVar1 != 0 && (*param_1 != 0)))) {
    if (param_1[0xc] == 0) {
      iVar3 = 0;
      iVar4 = 0;
      local_4 = 0;
      local_c = 0;
      local_8 = 0;
    }
    else {
      _AIL_3D_position_16(param_1[0xc],local_24,local_20,local_1c);
      iVar3 = __ftol();
      local_c = iVar3;
      iVar4 = __ftol();
      local_8 = iVar4;
      local_4 = __ftol();
    }
    local_18 = iVar3;
    local_14 = iVar4;
    local_10 = local_4;
    if (param_5 != (int *)0x0) {
      local_10 = param_5[2];
      local_18 = *param_5;
      local_14 = param_5[1];
    }
    if ((*(byte *)(iVar1 + 0x18) & 0x10) != 0) {
      FUN_0049b650(param_2);
    }
    if (*(int *)(iVar1 + 0x10) != 0) {
      iVar3 = -1;
      iVar4 = 0;
      piVar8 = param_1 + 0x1f;
      do {
        if ((*piVar8 != 0) && (iVar5 = _AIL_3D_sample_status_4(*piVar8), iVar5 == 2)) {
          uVar6 = _AIL_3D_user_data_8(param_1[iVar4 + 0x1f],0);
          _AIL_set_3D_user_data_12(param_1[iVar4 + 0x1f],0,0xffffffff);
          FUN_0049c560(uVar6,0);
          iVar3 = iVar4;
          break;
        }
        iVar4 = iVar4 + 1;
        piVar8 = piVar8 + 1;
      } while (iVar4 < 0x10);
      bVar2 = false;
      if (((*(byte *)(iVar1 + 0x18) & 2) == 0) &&
         ((((iVar4 = FUN_0046de60(&local_18,&local_c), param_6 <= iVar4 || (param_1[0xd] != 0)) &&
           (-1 < iVar3)) && (DAT_0066811c == 0)))) {
        iVar4 = _AIL_set_3D_sample_file_8(param_1[iVar3 + 0x1f],*(undefined4 *)(iVar1 + 0x10));
        bVar2 = true;
      }
      else {
        iVar3 = -1;
        iVar4 = 0;
        piVar8 = param_1 + 0xf;
        do {
          if ((*piVar8 != 0) && (iVar5 = _AIL_sample_status_4(*piVar8), iVar5 == 2)) {
            uVar6 = _AIL_sample_user_data_8(param_1[iVar4 + 0xf],0);
            _AIL_set_sample_user_data_12(param_1[iVar4 + 0xf],0,0xffffffff);
            FUN_0049c560(uVar6,0);
            iVar3 = iVar4;
            break;
          }
          iVar4 = iVar4 + 1;
          piVar8 = piVar8 + 1;
        } while (iVar4 < 0x10);
        if (iVar3 < 0) {
          return 0;
        }
        _AIL_init_sample_4(param_1[iVar3 + 0xf]);
        iVar4 = _AIL_set_named_sample_file_20
                          (param_1[iVar3 + 0xf],*(undefined4 *)(iVar1 + 4),
                           *(undefined4 *)(iVar1 + 0x10),*(undefined4 *)(iVar1 + 0xc),0);
      }
      if (iVar4 != 0) {
        if (bVar2) {
          _AIL_set_3D_position_16
                    (param_1[iVar3 + 0x1f],(float)local_18,(float)local_14,(float)local_10);
          uVar7 = DAT_005d7aa0 + -0x7f + param_3;
          _AIL_set_3D_sample_volume_8(param_1[iVar3 + 0x1f],uVar7 & ((int)uVar7 < 0) - 1);
          _AIL_set_3D_sample_float_distances_20
                    (param_1[iVar3 + 0x1f],(float)param_7,(float)param_6,(float)param_7,
                     (float)param_6);
          _AIL_set_3D_sample_loop_count_8(param_1[iVar3 + 0x1f],param_4);
          _AIL_set_3D_user_data_12(param_1[iVar3 + 0x1f],0,param_2);
          _AIL_start_3D_sample_4(param_1[iVar3 + 0x1f]);
        }
        else {
          uVar7 = DAT_005d7aa0 + -0x7f + param_3;
          uVar7 = uVar7 & ((int)uVar7 < 0) - 1;
          if (DAT_0066811c != 0) {
            iVar4 = FUN_0046de60(&local_18,&local_c);
            if ((float)iVar4 < _DAT_005a3698) {
              FUN_0046de60(&local_18,&local_c);
            }
            uVar7 = __ftol();
          }
          _AIL_set_sample_volume_8(param_1[iVar3 + 0xf],uVar7);
          _AIL_set_sample_loop_count_8(param_1[iVar3 + 0xf],param_4);
          _AIL_set_sample_user_data_12(param_1[iVar3 + 0xf],0,param_2);
          _AIL_register_EOS_callback_8(param_1[iVar3 + 0xf],FUN_0049c480);
          _AIL_start_sample_4(param_1[iVar3 + 0xf]);
        }
        if (param_4 == 0) {
          *(undefined4 *)(iVar1 + 0x14) = 0x7fffffff;
          *(uint *)(iVar1 + 0x18) = *(uint *)(iVar1 + 0x18) | 4;
          return 1;
        }
        iVar3 = param_1[0x31];
        *(int *)(iVar1 + 0x14) = iVar3;
        param_1[0x31] = iVar3 + 1;
        *(uint *)(iVar1 + 0x18) = *(uint *)(iVar1 + 0x18) | 4;
        return 1;
      }
    }
  }
  return 0;
}


