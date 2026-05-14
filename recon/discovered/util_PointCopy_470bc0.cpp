// FUN_00470bc0_StoreLastTargetVec @ 00470bc0 size=104

void __thiscall FUN_00470bc0_StoreLastTargetVec(int param_1,int *param_2)

{
  byte bVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  byte bVar5;
  
  uVar2 = FUN_0046dbe0_AngleFromDelta(param_2);
  *(undefined4 *)(param_1 + 0xb0) = uVar2;
  iVar3 = *param_2;
  bVar5 = 0;
  if (iVar3 < 0) {
    iVar3 = -iVar3;
  }
  iVar4 = param_2[1];
  if (iVar4 < 0) {
    iVar4 = -iVar4;
  }
  for (; (0xff < iVar3 || (0xff < iVar4)); iVar4 = iVar4 >> 1) {
    iVar3 = iVar3 >> 1;
    bVar5 = bVar5 + 1;
  }
  bVar1 = (&DAT_005e9200)[iVar4 + iVar3 * 0x100];
  *(int *)(param_1 + 0xb8) = param_2[2];
  *(uint *)(param_1 + 0xb4) = (uint)bVar1 << (bVar5 & 0x1f);
  return;
}


