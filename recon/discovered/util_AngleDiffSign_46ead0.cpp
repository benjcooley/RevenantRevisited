// FUN_0046ead0_AngleDiffFromFacing @ 0046ead0 size=97

int __thiscall FUN_0046ead0_AngleDiffFromFacing(int param_1,int param_2)

{
  byte bVar1;
  int iVar2;
  int local_c;
  int local_8;
  undefined4 local_4;
  
  local_4 = 0;
  local_c = *(int *)(param_2 + 0x10) - *(int *)(param_1 + 0x10);
  bVar1 = *(byte *)(param_1 + 0x36);
  local_8 = *(int *)(param_2 + 0x14) - *(int *)(param_1 + 0x14);
  iVar2 = FUN_0046dbe0_AngleFromDelta(&local_c);
  iVar2 = iVar2 - (uint)bVar1;
  if (0x7f < iVar2) {
    return iVar2 + -0x100;
  }
  if (iVar2 < -0x7f) {
    iVar2 = iVar2 + 0x100;
  }
  return iVar2;
}


