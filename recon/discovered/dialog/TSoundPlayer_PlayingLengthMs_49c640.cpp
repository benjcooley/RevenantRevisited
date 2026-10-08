// FUN_0049c640 @ 0049c640 size=104

undefined4 __thiscall FUN_0049c640(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  undefined4 local_8;
  int local_4;
  
  iVar2 = 0;
  local_8 = 0;
  piVar3 = (int *)(param_1 + 0x3c);
  local_4 = param_1;
  do {
    if (*piVar3 != 0) {
      iVar1 = _AIL_sample_user_data_8(*piVar3,0);
      if (param_2 == iVar1) {
        _AIL_sample_ms_position_12(*(undefined4 *)(local_4 + 0x3c + iVar2 * 4),&local_8,0);
        return local_8;
      }
    }
    iVar2 = iVar2 + 1;
    piVar3 = piVar3 + 1;
  } while (iVar2 < 0x10);
  return local_8;
}


