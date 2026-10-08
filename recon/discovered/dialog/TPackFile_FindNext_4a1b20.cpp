// FUN_004a1b20 @ 004a1b20 size=214

undefined4 FUN_004a1b20(undefined4 param_1,undefined4 *param_2)

{
  undefined4 *puVar1;
  uint uVar2;
  undefined4 uVar3;
  
  WaitForSingleObject(DAT_0065b8ac,0xffffffff);
  if (param_2[0x46] == 0) {
    uVar3 = FUN_0058c74d(param_1,param_2);
    ReleaseMutex(DAT_0065b8ac);
    return uVar3;
  }
  uVar2 = FUN_004a0380(param_2 + 0x47,param_2[0x88]);
  param_2[0x88] = uVar2;
  if (-1 < (int)uVar2) {
    if ((uVar2 < *(uint *)(param_2[0x46] + 0x194)) &&
       (puVar1 = *(undefined4 **)(*(int *)(param_2[0x46] + 0x1a4) + uVar2 * 4),
       puVar1 != (undefined4 *)0x0)) {
      _strncpy((char *)(param_2 + 5),(char *)puVar1[4],0xff);
      uVar3 = *puVar1;
      *(undefined1 *)((int)param_2 + 0x113) = 0;
      *param_2 = 0;
      param_2[4] = uVar3;
      param_2[3] = 0;
      param_2[1] = 0;
      param_2[2] = 0;
    }
    ReleaseMutex(DAT_0065b8ac);
    return 0;
  }
  ReleaseMutex(DAT_0065b8ac);
  return 0xffffffff;
}


