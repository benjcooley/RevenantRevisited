// FUN_0041c7f0 @ 0041c7f0 size=74

undefined4 * __thiscall FUN_0041c7f0(undefined4 *param_1,int param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  uint uVar2;
  int iVar3;
  
  param_1[2] = param_2;
  *param_1 = 0;
  param_1[1] = 0;
  param_1[3] = param_3;
  param_1[4] = 0;
  puVar1 = (undefined4 *)FUN_00482fb0(param_2 << 2);
  param_1[4] = puVar1;
  for (uVar2 = param_1[2] & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  }
  for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
    *(undefined1 *)puVar1 = 0;
    puVar1 = (undefined4 *)((int)puVar1 + 1);
  }
  return param_1;
}


