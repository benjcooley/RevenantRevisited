// FUN_0040ce60 @ 0040ce60 size=135

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __thiscall FUN_0040ce60(int *param_1,int param_2)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1[1] + 0x74);
  if (iVar1 == 0) {
    if (param_1[3] == 0) {
      iVar1 = *(int *)(param_1[1] + 0x60);
      if (iVar1 == 0) {
        iVar1 = FUN_00447ac0(1);
      }
      iVar1 = FUN_00407510(iVar1);
      if (iVar1 != 0) goto LAB_0040ce95;
    }
    else {
LAB_0040ce95:
      if (((param_1[0x27] != 0) && (-1 < param_2)) &&
         (iVar1 = (**(code **)(*param_1 + 0x3c))(), param_2 <= iVar1)) {
        iVar1 = ((int *)param_1[0x27])[param_2 * 2];
        if ((iVar1 == 0) && (iVar1 = *(int *)param_1[0x27], iVar1 == 0)) {
          return 0;
        }
        goto LAB_0040cec5;
      }
    }
    iVar1 = 0;
  }
  else {
LAB_0040cec5:
    if (((*(byte *)(iVar1 + 0x10) & 6) != 0) && (*(int *)(iVar1 + 0x18) == 0)) {
      *(undefined4 *)(iVar1 + 0x18) = _DAT_006668d0;
      return iVar1;
    }
  }
  return iVar1;
}


