// FUN_0046de60_Distance2D @ 0046de60 size=99

int FUN_0046de60_Distance2D(int *param_1,int *param_2)

{
  int iVar1;
  byte bVar2;
  int iVar3;
  
  iVar1 = *param_2 - *param_1;
  if (iVar1 < 0) {
    iVar1 = *param_1 - *param_2;
  }
  iVar3 = param_2[1] - param_1[1];
  if (iVar3 < 0) {
    iVar3 = param_1[1] - param_2[1];
  }
  bVar2 = 0;
  if (iVar1 < 0) {
    iVar1 = -iVar1;
  }
  if (iVar3 < 0) {
    iVar3 = -iVar3;
  }
  for (; (0xff < iVar1 || (0xff < iVar3)); iVar3 = iVar3 >> 1) {
    iVar1 = iVar1 >> 1;
    bVar2 = bVar2 + 1;
  }
  return (uint)(byte)(&DAT_005e9200)[iVar3 + iVar1 * 0x100] << (bVar2 & 0x1f);
}


