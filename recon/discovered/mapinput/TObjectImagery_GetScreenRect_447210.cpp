// FUN_00447210 @ 00447210 size=197

void __thiscall FUN_00447210(int *param_1,int param_2,int *param_3)

{
  short sVar1;
  short sVar2;
  short sVar3;
  uint uVar4;
  int iVar5;
  int *piStack_4;
  
  iVar5 = param_2;
  piStack_4 = param_1;
  uVar4 = (**(code **)(*param_1 + 0x3c))();
  if (uVar4 <= *(ushort *)(iVar5 + 0xc)) {
    param_3[3] = 0;
    param_3[1] = 0;
    param_3[2] = 0;
    *param_3 = 0;
    return;
  }
  FUN_0046eb40(&param_2,&piStack_4);
  uVar4 = (uint)*(ushort *)(iVar5 + 0xc);
  if (uVar4 < *(uint *)(*(int *)(param_1[1] + 0x54) + 4)) {
    iVar5 = *(int *)(param_1[1] + 0x54) + 8 + uVar4 * 0x4c;
  }
  else {
    iVar5 = 0;
  }
  if ((DAT_006682c4 == 0) && ((*(short *)(iVar5 + 0x2c) == 0 || (*(short *)(iVar5 + 0x2e) == 0)))) {
    *(undefined2 *)(iVar5 + 0x30) = 0x10;
    *(undefined2 *)(iVar5 + 0x32) = 0x20;
    *(undefined2 *)(iVar5 + 0x2c) = 0x20;
    *(undefined2 *)(iVar5 + 0x2e) = 0x20;
  }
  sVar1 = *(short *)(iVar5 + 0x2c);
  param_2 = param_2 - *(short *)(iVar5 + 0x30);
  *param_3 = param_2;
  sVar2 = *(short *)(iVar5 + 0x32);
  sVar3 = *(short *)(iVar5 + 0x2e);
  param_3[2] = sVar1 + -1 + param_2;
  iVar5 = (int)piStack_4 - (int)sVar2;
  param_3[1] = iVar5;
  param_3[3] = sVar3 + -1 + iVar5;
  return;
}


