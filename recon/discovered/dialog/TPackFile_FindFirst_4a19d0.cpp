// FUN_004a19d0 @ 004a19d0 size=321

undefined4 FUN_004a19d0(char *param_1,undefined4 *param_2)

{
  uint *puVar1;
  byte *pbVar2;
  undefined4 *puVar3;
  HANDLE hMutex;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  
  WaitForSingleObject(DAT_0065b8ac,0xffffffff);
  iVar5 = DAT_006687f8;
  iVar7 = 0;
  puVar1 = param_2 + 0x88;
  *puVar1 = 0xffffffff;
  if (0 < iVar5) {
    do {
      pbVar2 = *(byte **)(DAT_00668808 + iVar7 * 4);
      if (((pbVar2 != (byte *)0x0) && (*(int *)(pbVar2 + 400) != 0)) && ((*pbVar2 & 1) != 0)) {
        if (puVar1 == (uint *)0x0) {
          uVar4 = FUN_004a0380(param_1,0xffffffff);
        }
        else {
          uVar4 = FUN_004a0380(param_1,*puVar1);
          *puVar1 = uVar4;
        }
        if (-1 < (int)uVar4) {
          if (-1 < iVar7) {
            iVar5 = *(int *)(DAT_00668808 + iVar7 * 4);
            goto LAB_004a1a52;
          }
          break;
        }
      }
      iVar7 = iVar7 + 1;
    } while (iVar7 < DAT_006687f8);
  }
  iVar5 = 0;
LAB_004a1a52:
  param_2[0x46] = iVar5;
  if (iVar5 != 0) {
    if ((*puVar1 < *(uint *)(iVar5 + 0x194)) &&
       (puVar3 = *(undefined4 **)(*(int *)(iVar5 + 0x1a4) + *puVar1 * 4),
       puVar3 != (undefined4 *)0x0)) {
      _strncpy((char *)(param_2 + 5),(char *)puVar3[4],0xff);
      uVar6 = *puVar3;
      *(undefined1 *)((int)param_2 + 0x113) = 0;
      *param_2 = 0;
      param_2[4] = uVar6;
      param_2[3] = 0;
      param_2[1] = 0;
      param_2[2] = 0;
    }
    _strncpy((char *)(param_2 + 0x47),param_1,0x103);
    hMutex = DAT_0065b8ac;
    *(undefined1 *)((int)param_2 + 0x21f) = 0;
    ReleaseMutex(hMutex);
    return 1;
  }
  uVar6 = FUN_0058c680(param_1,param_2);
  ReleaseMutex(DAT_0065b8ac);
  return uVar6;
}


