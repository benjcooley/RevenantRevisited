// FUN_00471150 @ 00471150 size=251

void __thiscall FUN_00471150(int *param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = param_1[0x21];
  if (iVar1 != param_2) {
    if ((((param_1[2] & 0x40000000U) == 0) &&
        (((short)param_1[1] != 9 || (param_1[0xe] != *(int *)param_1[0x13])))) &&
       ((DAT_0066829c == 0 ||
        (((-1 < DAT_0065a784 && (uVar2 = *(uint *)(DAT_0065a77c + DAT_0065a784 * 4), uVar2 != 0)) &&
         ((*(byte *)(uVar2 & (DAT_0065a784 < 0) - 1) & 0x10) != 0)))))) {
      if (iVar1 != 0) {
        *(undefined4 *)(iVar1 + 0xc) = 0;
        FUN_004922c0();
        FUN_004830f0(iVar1);
        param_1[0x21] = 0;
      }
      param_1[0x21] = param_2;
      *(int **)(param_2 + 0xc) = param_1;
      if (param_2 != 0) {
        if ((param_1[2] & 0x8000U) == 0) {
          (**(code **)(*param_1 + 0x40))(param_1[2] | 0x8000);
        }
        DAT_006669fc = DAT_006669fc | 5;
        (**(code **)(*param_1 + 0x40))(param_1[2] | 0x40000);
      }
      if (param_1[0x21] != 0) {
        FUN_004924f0();
        return;
      }
    }
    else if (param_2 != 0) {
      *(undefined4 *)(param_2 + 0xc) = 0;
    }
  }
  return;
}


