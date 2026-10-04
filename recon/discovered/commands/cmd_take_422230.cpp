// FUN_00422230 @ 00422230 size=187

undefined4 FUN_00422230(undefined4 param_1,int param_2)

{
  int *piVar1;
  undefined4 uVar2;
  
  uVar2 = 1;
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
    return 4;
  }
  piVar1 = (int *)FUN_00451fe0(*(undefined4 *)(param_2 + 0x28),param_1,0,0);
  if (piVar1 == (int *)0x0) {
    FUN_0041ee50(s_Unable_to_find_object__s_005cb33c,*(undefined4 *)(param_2 + 0x28));
    FUN_004795c0();
    return 0;
  }
  FUN_00479580();
  if (*(int *)(param_2 + 0x10) == 8) {
    uVar2 = *(undefined4 *)(param_2 + 0x14);
    FUN_00479580();
  }
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
    return 4;
  }
  uVar2 = (**(code **)(*piVar1 + 0x70))(param_1,*(undefined4 *)(param_2 + 0x28),uVar2);
  FUN_0041ee50(s__d__s_taken_005cb358,uVar2,*(undefined4 *)(param_2 + 0x28));
  FUN_00479580();
  return 0;
}


