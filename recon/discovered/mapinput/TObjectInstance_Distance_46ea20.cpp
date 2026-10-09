// FUN_0046ea20 @ 0046ea20 size=103

int __thiscall FUN_0046ea20(int param_1,int param_2)

{
  int iVar1;
  byte bVar2;
  int iVar3;
  
  iVar3 = *(int *)(param_2 + 0x10) - *(int *)(param_1 + 0x10);
  if (iVar3 < 0) {
    iVar3 = *(int *)(param_1 + 0x10) - *(int *)(param_2 + 0x10);
  }
  iVar1 = *(int *)(param_2 + 0x14) - *(int *)(param_1 + 0x14);
  if (iVar1 < 0) {
    iVar1 = *(int *)(param_1 + 0x14) - *(int *)(param_2 + 0x14);
  }
  bVar2 = 0;
  if (iVar3 < 0) {
    iVar3 = -iVar3;
  }
  if (iVar1 < 0) {
    iVar1 = -iVar1;
  }
  for (; (0xff < iVar3 || (0xff < iVar1)); iVar1 = iVar1 >> 1) {
    iVar3 = iVar3 >> 1;
    bVar2 = bVar2 + 1;
  }
  return (uint)(byte)(&DAT_005e9200)[iVar1 + iVar3 * 0x100] << (bVar2 & 0x1f);
}


