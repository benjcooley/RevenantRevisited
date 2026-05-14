// FUN_00499ff0_TSector_DispatchNotifyToAll @ 00499ff0 size=152

void FUN_00499ff0_TSector_DispatchNotifyToAll(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int local_4;
  
  local_4 = 0;
  if (0 < DAT_00668578) {
    do {
      iVar1 = *(int *)(DAT_00668588 + local_4 * 4);
      if (iVar1 != 0) {
        for (iVar3 = 0; iVar3 < *(int *)(iVar1 + 0x144); iVar3 = iVar3 + 1) {
          piVar2 = *(int **)(*(int *)(iVar1 + 0x154) + iVar3 * 4);
          if (piVar2 == (int *)0x0) {
            piVar2 = *(int **)(iVar1 + 0x158);
          }
          piVar2 = *(int **)(*(int *)(iVar1 + 200) + *piVar2 * 4);
          if ((piVar2 != (int *)0x0) && ((short)piVar2[1] != 0xb)) {
            (**(code **)(*piVar2 + 0x10))(param_1,param_2);
          }
        }
      }
      local_4 = local_4 + 1;
    } while (local_4 < DAT_00668578);
  }
  FUN_0051fec0(param_1,param_2);
  FUN_00497e00(param_1,param_2);
  return;
}


