// FUN_0051e4d0 @ 0051e4d0 size=205

void __thiscall FUN_0051e4d0(int param_1,int *param_2)

{
  byte bVar1;
  byte *pbVar2;
  int iVar3;
  byte *pbVar4;
  int *piVar5;
  bool bVar6;
  
  if (param_2 != (int *)0x0) {
    pbVar4 = (byte *)(param_1 + 0x494);
    pbVar2 = (byte *)(param_2 + 1);
    do {
      bVar1 = *pbVar2;
      bVar6 = bVar1 < *pbVar4;
      if (bVar1 != *pbVar4) {
LAB_0051e513:
        iVar3 = (1 - (uint)bVar6) - (uint)(bVar6 != 0);
        goto LAB_0051e518;
      }
      if (bVar1 == 0) break;
      bVar1 = pbVar2[1];
      bVar6 = bVar1 < pbVar4[1];
      if (bVar1 != pbVar4[1]) goto LAB_0051e513;
      pbVar2 = pbVar2 + 2;
      pbVar4 = pbVar4 + 2;
    } while (bVar1 != 0);
    iVar3 = 0;
LAB_0051e518:
    if (iVar3 == 0) {
      pbVar4 = (byte *)(param_1 + 0x4c6);
      pbVar2 = (byte *)((int)param_2 + 0x36);
      do {
        bVar1 = *pbVar2;
        bVar6 = bVar1 < *pbVar4;
        if (bVar1 != *pbVar4) {
LAB_0051e54d:
          iVar3 = (1 - (uint)bVar6) - (uint)(bVar6 != 0);
          goto LAB_0051e552;
        }
        if (bVar1 == 0) break;
        bVar1 = pbVar2[1];
        bVar6 = bVar1 < pbVar4[1];
        if (bVar1 != pbVar4[1]) goto LAB_0051e54d;
        pbVar2 = pbVar2 + 2;
        pbVar4 = pbVar4 + 2;
      } while (bVar1 != 0);
      iVar3 = 0;
LAB_0051e552:
      if ((((iVar3 == 0) && (*param_2 == *(int *)(param_1 + 0x490))) &&
          (param_2[0x12] == *(int *)(param_1 + 0x4d8))) &&
         (param_2[0x13] == *(int *)(param_1 + 0x4dc))) {
        return;
      }
    }
    piVar5 = (int *)(param_1 + 0x490);
    for (iVar3 = 0x18; iVar3 != 0; iVar3 = iVar3 + -1) {
      *piVar5 = *param_2;
      param_2 = param_2 + 1;
      piVar5 = piVar5 + 1;
    }
    FUN_00584960(param_1,0);
  }
  return;
}


