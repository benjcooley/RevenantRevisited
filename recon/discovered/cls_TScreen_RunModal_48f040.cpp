// FUN_0048f040 @ 0048f040 size=171

undefined4 __thiscall FUN_0048f040(int param_1,int param_2,uint param_3)

{
  int iVar1;
  uint uVar2;
  uint *local_c;
  int *local_8;
  uint local_4;
  
  FUN_0048ed90(param_2,0xffffffff);
  if (*(int *)(param_1 + 0x1c) < 1) {
    uVar2 = 0;
  }
  else {
    uVar2 = *(uint *)(param_1 + 0x2c + *(int *)(param_1 + 0x1c) * 4);
  }
  FUN_00492080(param_1 + 4);
  for (; (local_c != (uint *)0x0 && (local_4 < *local_c)); local_4 = local_4 + 1) {
    if (param_2 == *local_8) goto LAB_0048f09a;
    local_8 = local_8 + 1;
  }
  local_4 = 0xffffffff;
LAB_0048f09a:
  FUN_0048eea0(local_4,uVar2 | param_3);
  iVar1 = *(int *)(param_2 + 0x40);
  while (iVar1 != 0) {
    if (DAT_006682b8 != 0) {
      return 0;
    }
    FUN_004911b0(1);
    iVar1 = *(int *)(param_2 + 0x40);
  }
  if (DAT_006682b8 != 0) {
    return 0;
  }
  return *(undefined4 *)(param_2 + 0x5c);
}


