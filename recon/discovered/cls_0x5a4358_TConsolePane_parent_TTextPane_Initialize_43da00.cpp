// FUN_0043da00 @ 0043da00 size=205

undefined4 __fastcall FUN_0043da00(int *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  
  FUN_00491900();
  iVar1 = *(int *)(DAT_0065b020 + DAT_0065bc40 * 4);
  param_1[0x26] = 0x3c;
  param_1[0x21] = 0x3c;
  param_1[0x1b] = 0;
  param_1[0x1f] = 0;
  param_1[0x20] = 0;
  param_1[0x24] = 0;
  param_1[0x23] = 0;
  param_1[0x1e] = 0;
  param_1[0x1d] = 0;
  if (iVar1 == 0) {
    iVar1 = DAT_0065b024;
  }
  iVar1 = *(int *)(iVar1 + 0x50);
  param_1[0x27] = -1;
  iVar1 = param_1[4] / (iVar1 + 2);
  param_1[0x25] = iVar1;
  param_1[0x22] = iVar1;
  (**(code **)(*param_1 + 0x2c))(1);
  param_1[0x1c] = 0;
  param_1[0x1a] = 0;
  puVar2 = (undefined4 *)FUN_00482fb0(10000);
  param_1[0x18] = (int)puVar2;
  *(undefined1 *)puVar2 = 0x7b;
  *(undefined1 *)((int)puVar2 + 1) = 0;
  for (iVar1 = 0x9c4; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  puVar2 = (undefined4 *)FUN_00482fb0(10000);
  *(undefined1 *)puVar2 = 0x7b;
  param_1[0x19] = (int)puVar2;
  *(undefined1 *)((int)puVar2 + 1) = 0;
  for (iVar1 = 0x9c4; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  return 1;
}


